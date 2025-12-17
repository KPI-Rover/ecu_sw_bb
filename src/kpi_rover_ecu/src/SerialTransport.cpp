#include "SerialTransport.h"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "loggingIncludes.h"

SerialTransport::SerialTransport(const std::string& device, int baudRate)
    : device_(device), baudRate_(baudRate), fd_(-1), running_(false) {}

SerialTransport::~SerialTransport() { Destroy(); }

int SerialTransport::Init() {
    fd_ = open(device_.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd_ < 0) {
        LOG_ERROR << "Error opening " << device_ << ": " << strerror(errno);
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd_, &tty) != 0) {
        LOG_ERROR << "Error from tcgetattr: " << strerror(errno);
        return -1;
    }

    speed_t speed;
    switch (baudRate_) {
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 57600: speed = B57600; break;
        case 115200: speed = B115200; break;
        default:
            LOG_WARNING << "Unsupported baudrate " << baudRate_ << ", defaulting to 115200";
            speed = B115200;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;  // 8-bit chars
    tty.c_cflag |= (CLOCAL | CREAD);             // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);           // shut off parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_iflag &= ~IGNBRK;                  // disable break processing
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);  // shut off xon/xoff ctrl

    tty.c_lflag = 0;  // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;  // no remapping, no delays

    tty.c_cc[VMIN] = 0;   // read doesn't block
    tty.c_cc[VTIME] = 5;  // 0.5 seconds read timeout

    if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
        LOG_ERROR << "Error from tcsetattr: " << strerror(errno);
        return -1;
    }

    LOG_INFO << "Serial port " << device_ << " initialized at " << baudRate_;
    return 0;
}

void SerialTransport::Start() {
    running_ = true;
    readThread_ = std::thread(&SerialTransport::ReadThread, this);
}

void SerialTransport::Destroy() {
    running_ = false;
    if (readThread_.joinable()) {
        readThread_.join();
    }
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
}

std::string SerialTransport::GetClientIp() { return "serial"; }

bool SerialTransport::Send(const std::vector<std::uint8_t>& data) {
    if (fd_ < 0) return false;

    // Frame: [AA] [Length] [Payload...] [CRC_L] [CRC_H]
    // Length includes itself (1) + Payload (N) + CRC (2) = N + 3
    size_t payloadSize = data.size();
    size_t frameSize = payloadSize + 3;

    if (frameSize > 255) {
        LOG_ERROR << "Payload too large for serial frame: " << payloadSize;
        return false;
    }

    std::vector<uint8_t> frame;
    frame.reserve(frameSize + 1);
    frame.push_back(0xAA); // Start byte
    frame.push_back(static_cast<uint8_t>(frameSize));
    frame.insert(frame.end(), data.begin(), data.end());

    // Calculate CRC over Length + Payload (exclude AA at index 0)
    uint16_t crc = CalculateCRC16(frame.data() + 1, frame.size() - 1);
    frame.push_back(static_cast<uint8_t>(crc & 0xFF));         // Low byte
    frame.push_back(static_cast<uint8_t>((crc >> 8) & 0xFF));  // High byte

    std::stringstream ss;
    for (const auto& byte : frame) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    LOG_INFO << "Sending serial frame: " << ss.str();

    ssize_t written = write(fd_, frame.data(), frame.size());
    if (written != static_cast<ssize_t>(frame.size())) {
        LOG_ERROR << "Failed to write full frame to serial";
        return false;
    }
    return true;
}

void SerialTransport::ReadThread() {
    while (running_) {
        uint8_t startByte;
        ssize_t n = read(fd_, &startByte, 1);
        if (n > 0) {
            if (startByte != 0xAA) {
                continue;
            }

            uint8_t lenByte;
            n = read(fd_, &lenByte, 1);
            if (n <= 0) continue;

            size_t frameLen = lenByte;
            if (frameLen < 3) {
                // Invalid frame length, ignore
                continue;
            }

            std::vector<uint8_t> frame;
            frame.reserve(frameLen + 1);
            frame.push_back(startByte);
            frame.push_back(lenByte);

            size_t bytesToRead = frameLen - 1; // Length includes itself, so read remaining
            while (bytesToRead > 0) {
                uint8_t buf[256];
                ssize_t r = read(fd_, buf, bytesToRead);
                if (r > 0) {
                    frame.insert(frame.end(), buf, buf + r);
                    bytesToRead -= r;
                } else {
                    // Timeout or error
                    break;
                }
            }

            if (frame.size() == frameLen + 1) {
                // Verify CRC
                // CRC is at the end (last 2 bytes)
                uint16_t receivedCrc = frame[frame.size() - 2] | (frame[frame.size() - 1] << 8);
                
                // Calculate CRC over Length + Payload (exclude AA at index 0, exclude CRC at end)
                // frame: [AA] [Length] [Payload...] [CRC_L] [CRC_H]
                // CRC data starts at index 1 (Length), length is frameLen - 2 (Length + Payload)
                uint16_t calculatedCrc = CalculateCRC16(frame.data() + 1, frameLen - 2);

                if (receivedCrc == calculatedCrc) {
                    // Extract payload: from index 2 (after AA, Length) to end - 2 (before CRC)
                    std::vector<uint8_t> payload(frame.begin() + 2, frame.end() - 2);
                    messageQueue_.Push(payload);
                } else {
                    LOG_WARNING << "CRC mismatch on serial frame";
                }
            }
        }
        // If n <= 0 (timeout or nothing), loop continues
    }
}

bool SerialTransport::Receive(std::vector<std::uint8_t>& data) {
    return messageQueue_.Pop(data, 50); // 50ms timeout
}

uint16_t SerialTransport::CalculateCRC16(const uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t pos = 0; pos < length; pos++) {
        crc ^= (uint16_t)data[pos];      // XOR byte with lower byte of CRC
        for (int i = 8; i != 0; i--) {    // Loop for each bit
            if ((crc & 0x0001) != 0) {    // If LSB is set
                crc >>= 1;               // Shift right
                crc ^= 0xA001;           // XOR with polynomial
            } else {                     // If LSB is not set
                crc >>= 1;               // Just shift right
            }
        }
    }
    return crc;
}
