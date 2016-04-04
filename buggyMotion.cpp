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



void buggyMotion::initMotion() {
	

	pinMode(LEFTMOTORDIR, OUTPUT);
	pinMode(RIGHTMOTORDIR, OUTPUT);
	pinMode(RIGHTMOTOR, OUTPUT);
	pinMode(LEFTMOTOR, OUTPUT);
	InitTimersSafe();
	_timersInitialised = true;
	_leftSpeed = 0;
	_rightSpeed = 0;
	_firstCall = true;
	_drifting = false;

}

void buggyMotion::drive(nC::Direction direction, nC::Drift drift)
{
	
	if (direction == nC::Stop) {
		stop();
		return;
	}


	if (_firstCall) {
		_firstCall = false;

		if (direction == nC::Direction::Forward || direction == nC::Direction::Backwards) {
			_leftSpeed = 35;
			_rightSpeed = 35;


		}
		if (direction == nC::Direction::Left || direction == nC::Direction::Right) {
			_leftSpeed = 50;
			_rightSpeed = 50;
		}
		SetPinFrequencySafe(LEFTMOTOR, _leftSpeed);
		SetPinFrequencySafe(RIGHTMOTOR, _rightSpeed);
		pwmWrite(LEFTMOTOR, 128);
		pwmWrite(RIGHTMOTOR, 128);
	}

	//If the buggy is drifting
	if (drift != nC::Drift::noDrift) {
		//Append the left and right motor speeds to solve drifting
		driftCorrect(direction, drift);
		_drifting = true;
	}
	else {
		_drifting = false;
	}

	
	

	//Call StepperControl with the correct parameters
	switch (direction)
	{
	case nC::Direction::Forward:
		stepperControl(nC::Direction::Forward, nC::Direction::Forward);
		break;
	case nC::Direction::Backwards:
		stepperControl(nC::Direction::Backwards, nC::Direction::Backwards);
		break;
	case nC::Direction::Left:
		stepperControl(nC::Direction::Backwards, nC::Direction::Forward);
		break;
	case nC::Direction::Right:
		stepperControl(nC::Direction::Forward, nC::Direction::Backwards);
		break;
	case nC::Direction::RightBackwardsOnly:
		stepperControl(nC::Direction::Stop, nC::Direction::Backwards);
		break;
	case nC::Direction::RightForwardOnly:
		stepperControl(nC::Direction::Stop, nC::Direction::Forward);
		break;
	case nC::Direction::LeftBackwardsOnly:
		stepperControl(nC::Direction::Backwards, nC::Direction::Stop);
		break;
	case nC::Direction::LeftForwardOnly:
		stepperControl(nC::Direction::Forward, nC::Direction::Stop);
		break;
	default:
		break;
	}

	
	
}
void buggyMotion::stepperControl(nC::Direction leftMotor, nC::Direction rightMotor) {

	setLeftMotorDirection(leftMotor);
	setRightMotorDirection(rightMotor);
	setLeftSpeed(_leftSpeed);
	setRightSpeed(_rightSpeed);
}
void buggyMotion::setLeftMotorDirection(nC::Direction dir)
{

	if (dir == nC::Direction::Forward || dir == nC::Direction::Right) {
		digitalWrite(LEFTMOTORDIR, LOW);
	}
	if (dir == nC::Direction::Backwards || dir == nC::Direction::Left) {
		digitalWrite(LEFTMOTORDIR, HIGH);
	}
	if (dir == nC::Direction::Stop) {
		pwmWrite(LEFTMOTOR, 0);
	}
}
void buggyMotion::setRightMotorDirection(nC::Direction dir)
{
	if (dir == nC::Direction::Forward || dir == nC::Direction::Right) {
		digitalWrite(RIGHTMOTORDIR, HIGH);
	}
	if (dir == nC::Direction::Backwards || dir == nC::Direction::Left) {
		digitalWrite(RIGHTMOTORDIR, LOW);
	}
	if (dir == nC::Direction::Stop) {
		pwmWrite(LEFTMOTOR, 0);
	}
}
void buggyMotion::setRightSpeed(int32_t freq)
{
	if (_drifting == true) {

		SetPinFrequencySafe(RIGHTMOTOR, freq);
	}
}
void buggyMotion::setLeftSpeed(int32_t freq)
{
	if (_drifting == true) {
		SetPinFrequencySafe(LEFTMOTOR, freq);
	}

}
void buggyMotion::driftCorrect(nC::Direction direction, nC::Drift drift)
{
	switch (direction)
	{
	case nC::Forward:
		if (drift==nC::Drift::leftDrift)
		{
			_leftSpeed++;
			_rightSpeed--;
		}
		else if (drift == nC::Drift::rightDrift)
		{
			_leftSpeed--;
			_rightSpeed++;
		}	
		break;
	case nC::Backwards:
		if (drift == nC::Drift::leftDrift)
		{
			_leftSpeed--;
			_rightSpeed++;
		}
		else if (drift == nC::Drift::rightDrift)
		{
			_leftSpeed++;
			_rightSpeed--;
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
void buggyMotion::stop()
{
	_leftSpeed = 0;
	_rightSpeed = 0;
	_firstCall = true;
	pwmWrite(LEFTMOTOR, _leftSpeed);
	pwmWrite(RIGHTMOTOR, _rightSpeed);

}