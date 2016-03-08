// 
// 
// 

#include "buggyMotion.h"

buggyMotion::buggyMotion() {
}

buggyMotion::~buggyMotion() {
}

bool buggyMotion::_motionInitComplete = false;

void buggyMotion::Kick(KickDirection dir, int Magnitude) {
	if (dir == KickDirection::Forward) {
		MotorControl(motorConfig::Direction::F, motorConfig::Direction::F, Magnitude, Magnitude);
		delay(DEFAULT_KICK_TIME);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, Magnitude, Magnitude);
	}
	else if (dir == KickDirection::Backward) {
		MotorControl(motorConfig::Direction::B, motorConfig::Direction::B, Magnitude, Magnitude);

		delay(DEFAULT_KICK_TIME);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Left) {
		MotorControl(motorConfig::Direction::B, motorConfig::Direction::F, Magnitude, Magnitude);

		delay(DEFAULT_KICK_TIME);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Right) {
		MotorControl(motorConfig::Direction::F, motorConfig::Direction::B, Magnitude, Magnitude);

		delay(DEFAULT_KICK_TIME);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, Magnitude, Magnitude);

	}
	return;
}
void buggyMotion::Kick(KickDirection dir, int Magnitude, uint16_t time) {
	if (dir == KickDirection::Forward) {
		MotorControl(motorConfig::Direction::F, motorConfig::Direction::F, Magnitude, Magnitude);

		delay(time);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Backward) {
		MotorControl(motorConfig::Direction::B, motorConfig::Direction::B, Magnitude, Magnitude);

		delay(time);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Left) {
		MotorControl(motorConfig::Direction::B, motorConfig::Direction::F, Magnitude, Magnitude);

		delay(time);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, Magnitude, Magnitude);

	}
	else if (dir == KickDirection::Right) {
		MotorControl(motorConfig::Direction::F, motorConfig::Direction::B, Magnitude, Magnitude);

		delay(time);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, Magnitude, Magnitude);

	}
	return;
}
void buggyMotion::Kick(KickDirection dir, int LeftMagnitude, int RightMagnitude) {
	if (dir == KickDirection::Forward) {
		MotorControl(motorConfig::Direction::F, motorConfig::Direction::F, LeftMagnitude, RightMagnitude);

		delay(DEFAULT_KICK_TIME);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, LeftMagnitude, RightMagnitude);

	}
	else if (dir == KickDirection::Backward) {
		MotorControl(motorConfig::Direction::B, motorConfig::Direction::B, LeftMagnitude, RightMagnitude);
		delay(DEFAULT_KICK_TIME);
		//analogWrite(E1, 0);
		//digitalWrite(M1, HIGH);
		//analogWrite(E2, 0);
		//digitalWrite(M2, HIGH);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, LeftMagnitude, RightMagnitude);

	}
	else if (dir == KickDirection::Left) {
		MotorControl(motorConfig::Direction::B, motorConfig::Direction::F, LeftMagnitude, RightMagnitude);
		delay(DEFAULT_KICK_TIME);
		//analogWrite(E1, 0);
		//digitalWrite(M1, HIGH);
		//analogWrite(E2, 0);
		//digitalWrite(M2, HIGH);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, LeftMagnitude, RightMagnitude);

	}
	else if (dir == KickDirection::Right) {
		MotorControl(motorConfig::Direction::F, motorConfig::Direction::B, LeftMagnitude, RightMagnitude);
		delay(DEFAULT_KICK_TIME);
		//analogWrite(E1, 0);
		//digitalWrite(M1, HIGH);
		//analogWrite(E2, 0);
		//digitalWrite(M2, HIGH);
		MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, LeftMagnitude, RightMagnitude);

	}
	return;
}
void buggyMotion::Kick(KickDirection LeftDir, KickDirection RightDir, int LeftMag, int RightMag) {
	if (LeftDir == RightDir) {
		Kick(LeftDir, LeftMag, RightMag);
	}
	else {
		Kick(LeftDir, LeftMag, 0);
		Kick(RightDir, 0, RightMag);
	}
}

void buggyMotion::initMotion() {
	initMotors();
	_motionInitComplete = true;
}

void buggyMotion::drive(motorConfig::Direction LDir, motorConfig::Direction RDir, byte LSpeed, byte RSpeed) {
	MotorControl(LDir, RDir, LSpeed, RSpeed);
}