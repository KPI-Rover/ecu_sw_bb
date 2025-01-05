#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <cstddef>
#include <iostream>
#include <mutex>

class CommandProcessor {
public:
    static const uint8_t CMD_SET_MOTOR = 0x01;
    static const uint8_t CMD_SET_ALL_MOTORS = 0x02;

    void processCommand(const unsigned char *payload, std::size_t length);

private:
    std::mutex mutex;
};

#endif