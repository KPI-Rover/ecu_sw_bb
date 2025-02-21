#include "motorsProcessor.h"
#include "config.h"

void MotorProcessor::init(const int* motorsArray) {
    if(rc_kill_existing_process(2.0)<-2) {
        perror("[ERROR][RC] rc_kill_existing process: ");
        exit(EXIT_FAILURE);
    }
        // start signal handler so we can exit cleanly

        if(rc_enable_signal_handler()==-1){
            fprintf(stderr,"[ERROR][RC] failed to start signal handler\n");
                //return nullptr;
            exit(EXIT_FAILURE);
        }	
    
    
    if (rc_motor_init_freq(RC_MOTOR_DEFAULT_PWM_FREQ) == -1) {
        perror("[ERROR][RC] failed to start with frequency");
        exit(EXIT_FAILURE);
    }
    
    Motor* result = new Motor[4] {
        Motor(*(motorsArray + 0)),
        Motor(*(motorsArray + 1)),
        Motor(*(motorsArray + 2)),
        Motor(*(motorsArray + 3))
    
    };
    
    if (result == nullptr) {
        perror("[ERROR][RC] allocating memory");
        exit(EXIT_FAILURE);
    }
    motors = result;


}

void MotorProcessor::setMotorRPM(int channel, int newRPM) {
    if (newRPM > MIN_RPM) {
        if (newRPM >= MAX_RPM) {
            cout << "[INFO][RC] Set RPM to max value" << endl;
            motors[channel-MOTOR_ID_START].motorGo(MAX_RPM);
        } else {
            motors[channel-MOTOR_ID_START].motorGo(newRPM);
        }
        
    } else {
        cout << "[INFO][RC] Set RPM to stop because RPM less than MIN_RPM" << endl;
        motors[channel-MOTOR_ID_START].motorStop();
    }

}

void MotorProcessor::stopMotor(int channel) {
    motors[channel-MOTOR_ID_START].motorStop();
}

void MotorProcessor::destroy() {
    delete[] motors;
    rc_motor_cleanup();
}

int MotorProcessor::getMotorRPM(int channel) {
    return motors[channel-MOTOR_ID_START].getRPM();
}


//MotorProcessor proc;



