#include "SerialReceiver.h"
#include <iostream>
#include <iomanip>
#include <cstring> 
#include <unistd.h>
#include <fcntl.h>

#define MAX_BUFFER_SIZE 255

SerialReceiver::SerialReceiver(CommandProcessor& processor) : commandProcessor(processor) {}

void* SerialReceiver::sReceiverThreadFunctionWrapper(void* arg) {
    reinterpret_cast<SerialReceiver*>(arg)->sReceiverThreadFunction();
    return nullptr;
}

void SerialReceiver::sReceiverThreadFunction() {
    uint8_t buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead;

    while (!stopThread) {
        
        bytesRead = read(serialFd, buffer, 1);
        if (bytesRead != 1) {
            continue;
        }
        
        uint16_t packageLength = buffer[0];
        if ((packageLength >= MAX_BUFFER_SIZE) || (packageLength < 4)) {
            continue;
        }
            
        bytesRead = read(serialFd, &buffer[1], packageLength - 1);
        if (bytesRead != (packageLength-1)) {
            continue;
        }

        std::cout << "Received: ";
        for (uint16_t i = 0; i < packageLength; ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]) << " ";
        }
        std::cout << std::endl;

        bool isCRCok = checkCRC(buffer, packageLength);
        if (!isCRCok) {
            continue;
        }

        commandProcessor.processCommand(&buffer[1], packageLength-3);
    }
}

void SerialReceiver::init(const char *serialPort, uint32_t baudRate) {

    serialFd = open(serialPort, O_RDWR | O_NOCTTY);
    if (serialFd == -1) {
        perror("ERROR: Not able to open serial port");
        exit(EXIT_FAILURE);
    }

    struct termios serialConfig;

    if (tcgetattr(serialFd, &serialConfig) < 0) {
        perror("ERROR: Not able to get serial port attributes");
        close(serialFd);
        exit(EXIT_FAILURE);
    }

    if (cfsetispeed(&serialConfig, B115200) < 0) {
        perror("ERROR: To set input speed of serial interface: ");
    }
    if (cfsetospeed(&serialConfig, B115200) < 0) {
        perror("ERROR: To set output speed of serial interface: ");
    }

    serialConfig.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    serialConfig.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    serialConfig.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
    serialConfig.c_cflag |= CS8; // 8 bits per byte (most common)
    serialConfig.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    serialConfig.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    serialConfig.c_lflag &= ~ICANON;
    serialConfig.c_lflag &= ~ECHO; // Disable echo
    serialConfig.c_lflag &= ~ECHOE; // Disable erasure
    serialConfig.c_lflag &= ~ECHONL; // Disable new-line echo
    serialConfig.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    serialConfig.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    serialConfig.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    serialConfig.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    serialConfig.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    serialConfig.c_cc[VTIME] = 3;
    serialConfig.c_cc[VMIN] = 0;

    tcsetattr(serialFd, TCSANOW, &serialConfig);
}

void SerialReceiver::start() {
    if (pthread_create(&sReceiverThread, nullptr, &SerialReceiver::sReceiverThreadFunctionWrapper, this) != 0) {
        perror("ERROR: Not able to create thread of SerialReceiver");
        exit(EXIT_FAILURE);
    }
}

void SerialReceiver::destroy() {
    stopThread = true;
    pthread_join(sReceiverThread, nullptr);
    close(serialFd);
}

bool SerialReceiver::checkCRC(const uint8_t *buffer, uint16_t length) {
    const uint16_t polynomial = 0xA001;
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length-2; ++i) {
        crc ^= static_cast<uint16_t>(buffer[i]);

        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
    }

    uint16_t receivedCRC;
    std::memcpy(&receivedCRC, buffer + length - 2, sizeof(uint16_t));

    // Print calculated and received CRC values in hex
    // std::cout << "Calculated CRC: 0x" << std::hex << std::setw(4) << std::setfill('0') << crc << std::dec << std::endl;
    // std::cout << "Received CRC:   0x" << std::hex << std::setw(4) << std::setfill('0') << receivedCRC << std::dec << std::endl;


    return crc == receivedCRC; 
}