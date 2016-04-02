// buggyMotion.h

#ifndef _BUGGYMOTION_H
#define _BUGGYMOTION_H

#include "Config.h"

class buggyMotion  {

public:
	
	buggyMotion();
		
	~buggyMotion();
	
	void initMotion();

protected:

	void drive(nC::Direction, nC::Drift);

private:
	void stepperControl(nC::Direction leftMotor, nC::Direction rightMotor, uint16_t leftSpeed, uint16_t rightSpeed);
	void setLeftMotorDirection(nC::Direction dir);
	void setRightMotorDirection(nC::Direction dir);
	void setRightSpeed(int32_t freq);
	void setLeftSpeed(int32_t freq);
	void driftCorrect(nC::Direction direction, nC::Drift drift);
	void stop();


	int32_t _leftspeed;
	int32_t _rightspeed;
	bool _timersInitialised = false;
	bool _firstCall = true;
	nC::Direction CurrentDirection = nC::Direction::Stop;
};


#endif

