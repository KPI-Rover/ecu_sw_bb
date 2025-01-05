#include "CommandProcessor.h"

#include <robotcontrol.h>


void CommandProcessor::processCommand(const unsigned char *payload, std::size_t length) {
    std::lock_guard<std::mutex> lock(mutex);

    int motor=0;
    int8_t speed=0;
    double duty=0.0;
    uint8_t command = payload[0];
    switch (command)
    {
    case CMD_SET_MOTOR : 
        std::cout << "CMD_SET_MOTOR" << std::endl;
        motor = static_cast<int>(payload[1]);
        speed = static_cast<int8_t>(payload[2]);
        if (speed != 0) {
            duty = speed / 100.0;
        }
        std::cout << "Motor: " << motor << " Speed: " << static_cast<int>(speed)<< " Duty: " << duty << std::endl;
        rc_motor_set(motor, duty);
        break;
    
    case CMD_SET_ALL_MOTORS :
        std::cout << "CMD_SET_ALL_MOTORS" << std::endl;
        speed = static_cast<int8_t>(payload[1]);
        std::cout << "Speed: " << speed << std::endl;
        if (speed != 0) {
            duty = speed / 100.0;
        }
        std::cout << "Duty: " << duty << std::endl;
        rc_motor_set(1,duty);
        rc_motor_set(2,duty);
        rc_motor_set(3,-duty);
        rc_motor_set(4,-duty);
        break;
    
    default:
        std::cout << "Unknown command :(" << std::endl;
        break;
    }
}