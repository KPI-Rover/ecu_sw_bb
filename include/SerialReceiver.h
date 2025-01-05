#ifndef SERIALRECEIVER_H
#define SERIALRECEIVER_H

#include "CommandProcessor.h"
#include <pthread.h>
#include <termios.h>

class SerialReceiver {
public:
    SerialReceiver(CommandProcessor& processor);
    void init(const char *serialPort, uint32_t baudRate);
    void start();
    void destroy();

private:
    int serialFd;
    pthread_t sReceiverThread;
    bool stopThread = false;
    CommandProcessor& commandProcessor;

    static void* sReceiverThreadFunctionWrapper(void* arg);
    void sReceiverThreadFunction();
    bool checkCRC(const uint8_t *buffer, uint16_t length);
};

#endif