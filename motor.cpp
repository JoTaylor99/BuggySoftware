// 
// 
// 

#include "motor.h"

motor::motor(){};
motor::~motor(){};

bool motor::_motorInitComplete = false;

void motor::initMotors() {

	//Set pin modes
	pinMode(speedPinA, OUTPUT);
	pinMode(speedPinB, OUTPUT);
	pinMode(dirPinA, OUTPUT);
	pinMode(dirPinB, OUTPUT);

	//Initially set direction pins forward
	digitalWrite(dirPinA, HIGH);
	digitalWrite(dirPinB, HIGH);

	_motorInitComplete = true;
};

void motor::setMotorSpeed(byte motorSpeed) {
	setMotorSpeed(motorSpeed, motorSpeed);
}

void motor::setMotorSpeed(byte motorASpeed, byte motorBSpeed) {
	
	analogWrite(speedPinA, motorASpeed);
	analogWrite(speedPinB, motorBSpeed);
}

void motor::setMotorDirection(mC::Direction motorADirection, mC::Direction motorBDirection) {

	if (motorADirection == motorBDirection) {
		setMotorDirection(motorADirection);
	}
	else if (motorADirection == mC::F) {
		digitalWrite(dirPinA, HIGH);
	}
	else if (motorADirection == mC::B) {
		digitalWrite(dirPinA, LOW);
	}

	if (motorBDirection == mC::F) {
		digitalWrite(dirPinB, HIGH);
	}
	else if (motorBDirection == mC::B) {
		digitalWrite(dirPinB, LOW);
	}

	if ((motorADirection == mC::S) || (motorBDirection == mC::S)) {
		setMotorDirection(motorADirection);
	}
}

void motor::setMotorDirection(mC::Direction motorDirection) {
	if (motorDirection == mC::S) {
		setMotorSpeed(0);
	}
	else if (motorDirection == mC::F) {
		digitalWrite(dirPinA, HIGH);
		digitalWrite(dirPinB, HIGH);
	}
	else if (motorDirection == mC::B) {
		digitalWrite(dirPinA, LOW);
		digitalWrite(dirPinB, LOW);
	}
}