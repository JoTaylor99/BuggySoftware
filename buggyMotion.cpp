// 
// 
// 

#include "buggyMotion.h"
#include <arduino2.h>
#include <PWM.h>
buggyMotion::buggyMotion() {
}

buggyMotion::~buggyMotion() {
}

bool buggyMotion::_motionInitComplete = false;

void buggyMotion::initMotion() {
	//initMotors();
	InitTimersSafe();
	_motionInitComplete = true;
	_leftspeed = 50;
	_rightspeed = 50;
}

void buggyMotion::drive(nC::Direction direction, nC::Drift drift)
{
	if (drift != nC::Drift::noDrift) {
		switch (drift)
		{
		case nC::leftDrift:
			
			

			break;
		case nC::rightDrift:
			break;
		default:
			break;
		}
	}

		switch (direction)
		{
		case nC::Direction::Forward:
			StepperControl(nC::Direction::Forward, nC::Direction::Forward, 50, 50);
			break;
		case nC::Direction::Backwards:
			StepperControl(nC::Direction::Backwards, nC::Direction::Backwards, 50, 50);
			break;
		case nC::Direction::Left:
			StepperControl(nC::Direction::Backwards, nC::Direction::Forward, 50, 50);
			break;
		case nC::Direction::Right:
			StepperControl(nC::Direction::Forward, nC::Direction::Forward, 50, 50);
			break;

		default:
			break;
		}

	


}

void buggyMotion::StepperControl(nC::Direction leftMotor, nC::Direction rightMotor, uint16_t leftSpeed, uint16_t rightSpeed) {
	setLeftMotorDirection(leftMotor);
	setRightMotorDirection(rightMotor);
	setLeftSpeed(leftSpeed);
	setRightSpeed(rightSpeed);
}
void buggyMotion::setLeftMotorDirection(nC::Direction dir)
{
	if (dir == nC::Direction::Forward || dir == nC::Direction::Right) {
		digitalWrite(LEFTMOTORDIR, HIGH);
	}
	if (dir == nC::Direction::Backwards || dir == nC::Direction::Left) {
		digitalWrite(LEFTMOTORDIR, LOW);
	}
}
void buggyMotion::setRightMotorDirection(nC::Direction dir)
{
	if (dir == nC::Direction::Forward || dir == nC::Direction::Right) {
		digitalWrite(LEFTMOTORDIR, LOW);
	}
	if (dir == nC::Direction::Backwards || dir == nC::Direction::Left) {
		digitalWrite(LEFTMOTORDIR, HIGH);
	}
}
void buggyMotion::setRightSpeed(int32_t freq)
{
	SetPinFrequencySafe(RIGHTMOTOR, freq);
}
void buggyMotion::setLeftSpeed(int32_t freq)
{
	SetPinFrequencySafe(LEFTMOTOR, freq);
}
void buggyMotion::driftCorrect(nC::Direction direction, nC::Drift drift)
{
	switch (direction)
	{
	case nC::Forward:
		if (drift==nC::Drift::leftDrift)
		{
			_leftspeed++;
			_rightspeed--;
		}
		else if (drift == nC::Drift::rightDrift)
		{
			_leftspeed--;
			_rightspeed++;
		}	
		break;
	case nC::Backwards:
		if (drift == nC::Drift::leftDrift)
		{
			_leftspeed--;
			_rightspeed++;
		}
		else if (drift == nC::Drift::rightDrift)
		{
			_leftspeed++;
			_rightspeed--;
		}
		break;
	case nC::Left:

		break;
	case nC::Right:
		break;
	default:
		break;
	}

}





//
//void buggyMotion::drive(nC::Direction direction) {
//	Reset();
//	if (direction == nC::Forward) {
//		ForwardMovement();
//	}
//	else if (direction == nC::Backwards) {
//		BackwardMovement();
//	}
//	else if (direction == nC::Right) {
//		RightMovement();
//	}
//	else {
//		LeftMovement();
//	}
//}
//
//void buggyMotion::drive(nC::Direction direction, byte Step) {
//	if (direction == nC::Backwards) {
//		StepBackward(Step);
//	}
//	else {
//		StepForward(Step);
//	}
//}
//
//void buggyMotion::drift(nC::Drift drift) {
//	if (drift == nC::leftDrift) {
//		DriftLeft();
//	}
//	else {
//		DriftRight();
//	}
//
//}
//
//void buggyMotion::ForwardMovement() {
//	analogWrite(MotorL, 127.5);
//	analogWrite(MotorR, 127.5);
//	digitalWrite2(dirL, HIGH);
//	digitalWrite2(dirR, HIGH);
//
//}
//
//void buggyMotion::BackwardMovement() {
//	analogWrite(MotorL, 127.5);
//	analogWrite(MotorR, 127.5);
//	digitalWrite2(dirL, LOW);
//	digitalWrite2(dirR, LOW);
//}
//
//void buggyMotion::Stop() {
//	Reset();
//	digitalWrite2(MotorL, LOW);
//	digitalWrite2(MotorR, LOW);
//}
//
//void buggyMotion::LeftMovement() {
//	analogWrite(MotorL, 127.5);
//	analogWrite(MotorR, 127.5);
//	digitalWrite2(dirL, LOW);
//	digitalWrite2(dirR, HIGH);
//}
//
//void buggyMotion::RightMovement() {
//	analogWrite(MotorL, 127.5);
//	analogWrite(MotorR, 127.5);
//	digitalWrite2(dirL, HIGH);
//	digitalWrite2(dirR, LOW);
//}
//
//void buggyMotion::StepForward(byte Steps) {
//	//This will move forward a set distance (Clicks = Steps/4).
//	//800 Steps per revolution (200 Clicks).
//	digitalWrite2(dirL, HIGH);
//	digitalWrite2(dirR, HIGH);
//	for (; Steps > 0; Steps--) {
//		digitalWrite2(MotorL, HIGH);
//		digitalWrite2(MotorR, HIGH);
//		delay(10);
//		digitalWrite2(MotorL, LOW);
//		digitalWrite2(MotorR, LOW);
//	}
//}
//
//void buggyMotion::StepBackward(byte Steps) {
//	//This will move forward a set distance (Clicks = Steps/4).
//	//800 Steps per revolution (200 Clicks).
//	digitalWrite2(dirL, LOW);
//	digitalWrite2(dirR, LOW);
//	for (; Steps > 0; Steps--) {
//		digitalWrite2(MotorL, HIGH);
//		digitalWrite2(MotorR, HIGH);
//		delay(10);
//		digitalWrite2(MotorL, LOW);
//		digitalWrite2(MotorR, LOW);
//	}
//}
//
//
//void buggyMotion::DriftRight() {
//	////Make Right wheel go faster & Left wheel go slower:
//	//TCCR2B = TCCR2B & B11111000 | B00000100;    // set timer 2 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
//	//TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz
//	//											//Turn towards the left.
//	//LeftMovement();
//}
//
//void buggyMotion::DriftLeft() {
//	////Make Left wheel go faster & Right wheel go slower:
//	//TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz (The DEFAULT)
//	//TCCR2B = TCCR2B & B11111000 | B00000111;    // set timer 2 divisor to  1024 for PWM frequency of    30.64 Hz
//	//											//Turn towards the left.
//	//LeftMovement();
//}
//
//void buggyMotion::Reset() {
//	//TCCR1B = 0;
//	//TCCR2B = 0;
//	//TCCR1B = TCCR1B & B11111000 | B00000100;    // set timer 1 divisor to   256 for PWM frequency of   122.55 Hz
//	//TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to   256 for PWM frequency of   122.55 Hz
//}

//functions removed 25/03
/*

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
//digitalWrite2(M1, HIGH);
//analogWrite(E2, 0);
//digitalWrite2(M2, HIGH);
MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, LeftMagnitude, RightMagnitude);

}
else if (dir == KickDirection::Left) {
MotorControl(motorConfig::Direction::B, motorConfig::Direction::F, LeftMagnitude, RightMagnitude);
delay(DEFAULT_KICK_TIME);
//analogWrite(E1, 0);
//digitalWrite2(M1, HIGH);
//analogWrite(E2, 0);
//digitalWrite2(M2, HIGH);
MotorControl(motorConfig::Direction::S, motorConfig::Direction::S, LeftMagnitude, RightMagnitude);

}
else if (dir == KickDirection::Right) {
MotorControl(motorConfig::Direction::F, motorConfig::Direction::B, LeftMagnitude, RightMagnitude);
delay(DEFAULT_KICK_TIME);
//analogWrite(E1, 0);
//digitalWrite2(M1, HIGH);
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
*/