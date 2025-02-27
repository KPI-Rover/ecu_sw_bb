#include "motorsProcessor.h"
#include "config.h"

int MotorProcessor::init(const int* motorsArray) {
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
    
    Motor* result = new Motor[4] {
        Motor(*(motorsArray + 0)),
        Motor(*(motorsArray + 1)),
        Motor(*(motorsArray + 2)),
        Motor(*(motorsArray + 3))
    
    };
    
    if (result == nullptr) {
        perror("[ERROR][RC] allocating memory");
        return -1;
    }
    motors = result;

    return 0;
}

int MotorProcessor::setMotorRPM(int channel, int newRPM) {
    if (abs(newRPM) > MIN_RPM) {
        if (newRPM >= MAX_RPM) {
            cout << "[INFO][RC] Set RPM to max value" << endl;
            if (motors[channel-MOTOR_ID_START].motorGo(MAX_RPM) != 0) {
                cout << "[ERROR][RC] Error while stet new RPM" << endl;
                return -1;
            }
        } else {
            if (motors[channel-MOTOR_ID_START].motorGo(newRPM)  != 0) {
                cout << "[ERROR][RC] Error while stet new RPM" << endl;
                return -1;
            }
        }
        
    } else {
        cout << "[INFO][RC] Set RPM to stop because RPM less than MIN_RPM" << endl;

        if (motors[channel-MOTOR_ID_START].motorStop() != 0) {
            cout << "[ERROR][RC] Error while stet new RPM" << endl;
            return -1;
        }
    }

    return 0;

}

int MotorProcessor::stopMotor(int channel) {
    
    if (motors[channel-MOTOR_ID_START].motorStop()  != 0) {
        cout << "[ERROR][RC] Error while stoppping motor" << endl;
        return -1;
    }
    //cout << "[INFO][RC] Set RPM to stop because of command" << endl;

    return 0;
}

void MotorProcessor::destroy() {
    delete[] motors;
    rc_motor_cleanup();
}

int MotorProcessor::getMotorRPM(int channel) {
    return motors[channel-MOTOR_ID_START].getRPM();
}


//MotorProcessor proc;



