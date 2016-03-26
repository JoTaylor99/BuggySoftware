// 
// 
// 

#include "motor.h"

motor::motor(){};
motor::~motor(){};

bool motor::_motorInitComplete = false;

void motor::initMotors() {
	GP.begin();      // use default address 0


	pinMode(speedPinA, OUTPUT);

	pinMode(speedPinB, OUTPUT);

	GP.pinMode(dir1PinA, OUTPUT);
	GP.pinMode(dir2PinA, OUTPUT);
	GP.pinMode(dir1PinB, OUTPUT);
	GP.pinMode(dir2PinB, OUTPUT);

	_motorInitComplete = true;
};

void motor::MotorControl(mC::Direction LDir, mC::Direction RDir, byte LSpeed = 0, byte RSpeed = 0){
	if (LSpeed < 0) {
		LSpeed = 0;
	}
	else if (LSpeed >255) {
		LSpeed = 255;
	}
	if (RSpeed < 0) {
		RSpeed = 0;
	}
	else if (RSpeed >255) {
		RSpeed = 255;
	}
	if (LDir == mC::F) {

		GP.digitalWrite(dir1PinB, LOW);
		GP.digitalWrite(dir2PinB, HIGH);
		analogWrite(speedPinB, LSpeed);

	}
	else if (LDir == mC::B) {

		GP.digitalWrite(dir1PinB, HIGH);
		GP.digitalWrite(dir2PinB, LOW);
		analogWrite(speedPinB, LSpeed);

	}
	else {
		GP.digitalWrite(dir1PinB, LOW);
		GP.digitalWrite(dir2PinB, LOW);
		analogWrite(speedPinB, 0);
	}


	if (RDir == mC::F) {
		GP.digitalWrite(dir1PinA, HIGH);
		GP.digitalWrite(dir2PinA, LOW);
		analogWrite(speedPinA, RSpeed);
	}
	else if (RDir == mC::B) {
		GP.digitalWrite(dir1PinA, LOW);
		GP.digitalWrite(dir2PinA, HIGH);
		analogWrite(speedPinA, RSpeed);
	}
	else {
		GP.digitalWrite(dir1PinA, LOW);
		GP.digitalWrite(dir2PinA, LOW);
		analogWrite(speedPinA, 0);

	}
};