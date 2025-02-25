#ifndef MOTORSPROCESSOR_H
#define MOTORSPROCESSOR_H

#include "config.h"


class Motor {
public:
    int motorNumber;
    //int brakeTime;

    
    Motor(int assignedNumber) {
        motorNumber = assignedNumber;
    };	

    int motorGo(int newRPM) {
        

        currentRPM = newRPM;
        //cout << "new rpm" << newRPM << endl; // debugging 
        currentDutyCycle = getDC(newRPM);
        if (motorNumber >= MOTOR_INVERTED) {
            currentDutyCycle = -currentDutyCycle;	
        }
        
        //cout << "go "  << currentDutyCycle  << endl; // debug

        if (rc_motor_set(motorNumber, currentDutyCycle) != 0) {
            cout << "[ERROR][RC] rc_motor_set" << endl;
            return -1;
        }

        return 0;
    }

    int motorStop() {
        if (rc_motor_brake(motorNumber) != 0) {
            cout << "[ERROR][RC] rc_motor_brake" << endl;
            return -1;
        }

        rc_usleep(BRAKE_TIME);

        if (rc_motor_free_spin(motorNumber) != 0) {
            cout << "[ERROR][RC] rc_motor_free_spin" << endl;
            return -1;
        }
        
        currentRPM = 0;

        return 0;
    }

    int getRPM () {
        return currentRPM;
    }

private:	


    int currentRPM;
    double currentDutyCycle;


    double getDC(int entryRPM) {
        double result = static_cast<double>(entryRPM) / MAX_RPM;
        //cout << result << endl; // debug
        return result;
    }

};

class MotorProcessor {
public:
    Motor* motors;


    
    
    int init(const int* motorsArray);
    
    int setMotorRPM(int channel, int newRPM);
    int stopMotor(int channel);

    int getMotorRPM(int channel);

    void destroy();


};




#endif