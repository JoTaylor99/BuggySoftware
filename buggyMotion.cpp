// 
// 
// 

#include "buggyMotion.h"

buggyMotion::buggyMotion() {
}

buggyMotion::~buggyMotion() {
}

void buggyMotion::initMotion() {
	

	pinMode(LEFTMOTORDIR, OUTPUT);
	pinMode(RIGHTMOTORDIR, OUTPUT);
	pinMode(RIGHTMOTOR, OUTPUT);
	pinMode(LEFTMOTOR, OUTPUT);
	pinMode(LEFTMOTORCOUNT, INPUT);
	pinMode(RIGHTMOTORCOUNT, INPUT);
	InitTimersSafe();
	_timersInitialised = true;
	_leftSpeed = 0;
	_rightSpeed = 0;
	_firstCall = true;
	_drifting = false;
	_driftCount = 0;
	_previousDrift = nC::Drift::noDrift;
	stepDistanceLeft = 0;
	stepDistanceRight = 0;

	stepTargetDistanceLeft = 0;
	stepTargetDistanceRight = 0;

}

void buggyMotion::drive(nC::Direction direction, nC::Drift drift)
{
	if (direction == nC::Direction::LeftBackwardsOnly || direction == nC::Direction::LeftForwardOnly || direction == nC::Direction::RightBackwardsOnly || direction == nC::Direction::RightForwardOnly) {
		_firstCall = true;
	}
	if (direction == nC::Stop) {
		stop();
		return;
	}

	MOT_VPRINT(_leftSpeed);
	MOT_PRINT("\t");
	MOT_VPRINTLN(_rightSpeed);

	if (_firstCall) {
		if (direction == nC::Direction::Forward || direction == nC::Direction::Backwards) {
			_leftSpeed = 80;
			_rightSpeed = 80;


		}
		if (direction == nC::Direction::Left || direction == nC::Direction::Right) {
			_leftSpeed = 50;
			_rightSpeed = 50;
		}
		
		if (direction == nC::Direction::RightForwardOnly || direction == nC::Direction::RightBackwardsOnly || direction == nC::Direction::LeftForwardOnly || nC::Direction::LeftBackwardsOnly) {
			_leftSpeed = 40;
			_rightSpeed = 40;
		}
	}

	//If the buggy is drifting
	if (drift != nC::Drift::noDrift) {
		//Append the left and right motor speeds to solve drifting
		driftCorrect(direction, drift);
		_drifting = true;
	}
	else {
		_drifting = false;
		_driftCount = 0;
		if (direction == nC::Direction::Forward || direction == nC::Direction::Backwards) {
			
			_leftSpeed = 80;
			_rightSpeed = 80;


		}
		if (direction == nC::Direction::Left || direction == nC::Direction::Right) {
			_leftSpeed = 50;
			_rightSpeed = 50;
		}
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

	if (_firstCall) {
		SetPinFrequencySafe(LEFTMOTOR, _leftSpeed);
		SetPinFrequencySafe(RIGHTMOTOR, _rightSpeed);
		pwmWrite(LEFTMOTOR, 128);
		pwmWrite(RIGHTMOTOR, 128);
		_firstCall = false;
	}
	
	_previousDrift = drift;
}
void buggyMotion::getSpeeds(int32_t & leftSpeed, int32_t & rightSpeed)
{
	leftSpeed = _leftSpeed;
	rightSpeed = _rightSpeed;
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
		MOT_PRINTLN("LStop");
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
		MOT_PRINTLN("RStop");
		pwmWrite(LEFTMOTOR, 0);
	}
}
void buggyMotion::setRightSpeed(int32_t freq)
{
	if (_drifting == true || (_previousDrift != nC::Drift::noDrift)){
		capSpeeds();
		SetPinFrequencySafe(RIGHTMOTOR, freq);
	}
}
void buggyMotion::setLeftSpeed(int32_t freq)
{
	if (_drifting == true || (_previousDrift != nC::Drift::noDrift)) {
		capSpeeds();
		SetPinFrequencySafe(LEFTMOTOR, freq);
	}

}
void buggyMotion::driftCorrect(nC::Direction direction, nC::Drift drift)
{
	if (_driftCount > 2) {
		switch (direction)
		{
		case nC::Forward:
			if (drift == nC::Drift::leftDrift)
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
		_driftCount = 0;
		return;
	}
	_driftCount++;


}
void buggyMotion::stop(uint8_t motorSelect)
{

	_firstCall = true;
	_driftCount = 0;

	if (motorSelect == 0) {		//both
		_leftSpeed = 0;
		_rightSpeed = 0;
		pwmWrite(LEFTMOTOR, _leftSpeed);
		pwmWrite(RIGHTMOTOR, _rightSpeed);
	}
	else if (motorSelect == 1) {	//left
		_leftSpeed = 0;
		pwmWrite(LEFTMOTOR, _leftSpeed);
		}
	else if (motorSelect == 2) {	//right
		_rightSpeed = 0;
		pwmWrite(RIGHTMOTOR, _rightSpeed);
			}
		MOT_PRINTLN("St");
}

void buggyMotion::capSpeeds()
{
	if (_leftSpeed <35){
		_leftSpeed = 35;
	}
	else if(_leftSpeed>100) {
		_leftSpeed = 100;
	}
	if (_rightSpeed <35) {
		_rightSpeed = 35;
	}
	else if (_rightSpeed>100) {
		_rightSpeed = 100;
	}
}

uint16_t buggyMotion::getStepsFromDistance(uint16_t mmDistance) {
	return static_cast<uint16_t>(8*(mmDistance*THESCALEFACTOR));
}

bool buggyMotion::isMoveComplete() {
	if ((_leftWheelTask == true) && (_rightWheelTask == true)) {
		return true;
	}
	else { return false; }
}


void buggyMotion::stepSeparately(nC::Direction direction, uint16_t leftDistance, uint16_t rightDistance) {

	_firstCall = true;
	//if (leftDistance > DEFAULTMAXDISTANCE) { leftDistance = DEFAULTMAXDISTANCE; }
	//if (rightDistance > DEFAULTMAXDISTANCE) { rightDistance = DEFAULTMAXDISTANCE; }

	if ((direction == nC::LeftBackwardsOnly) || (direction == nC::LeftForwardOnly)) {
		rightDistance = 0;
	}
	else if ((direction == nC::RightBackwardsOnly) || (direction == nC::RightForwardOnly)) {
		leftDistance = 0;
	}

	stepTargetDistanceLeft = getStepsFromDistance(leftDistance);
	stepTargetDistanceRight = getStepsFromDistance(rightDistance);

	if (leftDistance != 0) {
		_leftWheelTask = false;
		pciSetup(LEFTMOTORCOUNT);
	}

	if (rightDistance != 0) {
		_rightWheelTask = false;
		pciSetup(RIGHTMOTORCOUNT);
	}

	drive(direction);

	#ifdef STEPWISE_BLOCKING
	while (!(isMoveComplete())) {}
	stop();
	#endif
}

void buggyMotion::step(nC::Direction direction, uint16_t distance) {
	stepSeparately(direction, distance, distance);
}

void buggyMotion::pciSetup(uint8_t pin)
{
	*digitalPinToPCMSK(pin) |= bit(digitalPinToPCMSKbit(pin));  // enable pin
	PCIFR |= bit(digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
	PCICR |= bit(digitalPinToPCICRbit(pin)); // enable interrupt for the group
}