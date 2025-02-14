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

	void motorGo(int newRPM) {
		if (newRPM >= MAX_RPM) {
			newRPM = MAX_RPM;
		}

		currentRPM = newRPM;
		//cout << "new rpm" << newRPM << endl; // debugging 
		currentDutyCycle = getDC(newRPM);
		if (motorNumber >= MOTOR_INVERTED) {
			currentDutyCycle = -currentDutyCycle;	
		}
		
		//cout << "go "  << currentDutyCycle  << endl; // debug

		rc_motor_set(motorNumber, currentDutyCycle);
	}

	void motorStop() {
		rc_motor_brake(motorNumber);
		rc_usleep(BRAKE_TIME);
		rc_motor_free_spin(motorNumber);
	}

	int getRPM () {
		return currentRPM;
	}

private:	


	int currentRPM;
	double currentDutyCycle;


	double getDC(int entryRPM) {
		double result = static_cast<double>(entryRPM) / MAX_RPM;
		cout << result << endl; // debug
		return result;
	}

};

class MotorProcessor {
public:
	Motor* motors;


	
	
	void init(const int* motorsArray);
	
	void setMotorRPM(int channel, int newRPM);
	void stopMotor(int channel);

	int getMotorRPM(int channel);

	void destroy();


};




#endif