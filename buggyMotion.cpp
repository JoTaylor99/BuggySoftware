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
	InitTimersSafe();
	_timersInitialised = true;
	_leftspeed = 0;
	_rightspeed = 0;
	_firstCall = true;
}
void buggyMotion::drive(nC::Direction direction, nC::Drift drift = nC::Drift::noDrift)
{
	if (direction == nC::Stop) {
		stop();
		return;
	}


	if (_firstCall) {
		_firstCall = false;
		if (direction == nC::Direction::Forward || direction == nC::Direction::Backwards) {
			_leftspeed = 80;
			_rightspeed = 80;
		}
		if (direction == nC::Direction::Left || direction == nC::Direction::Right) {
			_leftspeed = 50;
			_rightspeed = 50;
		}
	}

	//If the buggy is drifting
	if (drift != nC::Drift::noDrift) {
		//Append the left and right motor speeds to solve drifting
		driftCorrect(direction, drift);
	}

	
	

	//Call StepperControl with the correct parameters
	switch (direction)
	{
	case nC::Direction::Forward:
		stepperControl(nC::Direction::Forward, nC::Direction::Forward, _leftspeed, _rightspeed);
		break;
	case nC::Direction::Backwards:
		stepperControl(nC::Direction::Backwards, nC::Direction::Backwards, _leftspeed, _rightspeed);
		break;
	case nC::Direction::Left:
		stepperControl(nC::Direction::Backwards, nC::Direction::Forward, _leftspeed, _rightspeed);
		break;
	case nC::Direction::Right:
		stepperControl(nC::Direction::Forward, nC::Direction::Forward, _leftspeed, _rightspeed);
		break;

	default:
		break;
	}

	


}
void buggyMotion::stepperControl(nC::Direction leftMotor, nC::Direction rightMotor, uint16_t leftSpeed, uint16_t rightSpeed) {
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
void buggyMotion::stop()
{
	_leftspeed = 0;
	_rightspeed = 0;
	_firstCall = true;
	SetPinFrequencySafe(LEFTMOTOR, 0);
	SetPinFrequencySafe(RIGHTMOTOR, 0);

}