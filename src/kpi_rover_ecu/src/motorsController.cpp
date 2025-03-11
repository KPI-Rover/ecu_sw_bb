#include "motorsController.h"
#include "config.h"
#include "motor.h"
#include "motorConfig.h"

int MotorController::init(MotorConfig _motors[], uint8_t _motorNumber) {
    if(rc_kill_existing_process(2.0)<-2) {
        perror("[ERROR][RC] rc_kill_existing process: ");
        return -1;
    }
        // start signal handler so we can exit cleanly

    if(rc_enable_signal_handler()==-1){
        fprintf(stderr,"[ERROR][RC] failed to start signal handler\n");
            //return nullptr;
        return -1;
    }	
    
    
    if (rc_motor_init_freq(RC_MOTOR_DEFAULT_PWM_FREQ) == -1) {
        perror("[ERROR][RC] failed to start with frequency");
        return -1;
    }

    if (rc_encoder_init()) {
        perror("[ERROR][RC] failed to start with frequency");
        return -1;
    }

    motorNumber = _motorNumber;

    for (int i = 0; i < motorNumber; ++i) {
        motors.push_back(Motor(_motors[i].getNumber(), _motors[i].isInverted()));
    }

    return 0;
}

int MotorController::setMotorRPM(int channel, int newRPM) {
    if (abs(newRPM) > MIN_RPM) {
        if (newRPM >= MAX_RPM) {
            cout << "[INFO][RC] Set RPM to max value" << endl;
            if (motors[channel].motorGo(MAX_RPM) != 0) {
                cout << "[ERROR][RC] Error while stet new RPM" << endl;
                return -1;
            }
        } else {
            if (motors[channel].motorGo(newRPM)  != 0) {
                cout << "[ERROR][RC] Error while stet new RPM" << endl;
                return -1;
            }
        }
        
    } else {
        if (abs(newRPM) != 0) {
            cout << "[INFO][RC] Set RPM to stop because RPM less than MIN_RPM" << endl;
        }
        
        if (motors[channel].motorStop() != 0) {
            cout << "[ERROR][RC] Error while set new RPM" << endl;
            return -1;
        }
    }

    return 0;

}

int MotorController::stopMotor(int channel) {
    
    if (motors[channel].motorStop()  != 0) {
        cout << "[ERROR][RC] Error while stoppping motor" << endl;
        return -1;
    }
    //cout << "[INFO][RC] Set RPM to stop because of command" << endl;

    return 0;
}

void MotorController::destroy() {
    //delete[] motors;
    rc_motor_cleanup();
    rc_encoder_cleanup();
}

int MotorController::getMotorRPM(int channel) {
    return motors[channel].getRPM();
}


//MotorProcessor proc;



