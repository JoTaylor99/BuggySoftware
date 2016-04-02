// buggyMotion.h

#ifndef _BUGGYMOTION_H
#define _BUGGYMOTION_H

#include "Config.h"

class buggyMotion  {

public:
	/* buggyMotion constructor*/
	buggyMotion();

	/* buggyMotion destructor*/
	~buggyMotion();

	/* buggyMotion init function
	*  Void return type
	*  Will depends on communications class for sending error codes or debug statements
	*  
	*/
	void initMotion();

protected:


	void drive(nC::Direction, nC::Drift);



	////Drive functions two types:
	///// <summary>
	///// Drive function 1, for standard movements (Forwards, Backwards, Left, Right).
	///// Calls specific functions for each direction
	///// </summary>
	///// <param name="direction"></param>
	//void drive(nC::Direction direction);
	///// <summary>
	///// Drive function 2, for movements where you want a certain amount of steps.
	///// This is for forwards & backwards only.
	///// 800 Steps per revolution of the wheel.
	///// </summary>
	///// <param name="direction"></param>
	///// <param name="Step"></param>
	//void drive(nC::Direction direction, byte Step);
	///// <summary>
	///// Function to call relevant drift functions. (Left or right)
	///// </summary>
	///// <param name="drift"></param>
	//void drift(nC::Drift drift);

	




private:
	static bool _motionInitComplete;
	void ForwardMovement();
	void BackwardMovement();
	void LeftMovement();
	void RightMovement();
	void Stop();
	void StepForward(byte Steps);
	void StepBackward(byte Steps);
	void DriftRight();
	void DriftLeft();
	void Reset();

	void StepperControl(nC::Direction leftMotor, nC::Direction rightMotor, uint16_t leftSpeed, uint16_t rightSpeed);
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
	/* checkMotionConfigCorrect function
	* checks to ensure all initialization functions have been run,
	*	if any have not been called the an object will be created and the relevant init function run again.
	*	return type void
	*/

	void checkMotionConfigCorrect();

};


#endif

//functions removed 25/03
/*

enum KickDirection :uint8_t {
Forward, Backward, Left, Right
};

void Kick(KickDirection dir, int Magnitude);
void Kick(KickDirection dir, int Magnitude, uint16_t time);
void Kick(KickDirection dir, int LeftMagnitude, int RightMagnitude);
void Kick(KickDirection LeftDir, KickDirection RightDir, int LeftMag, int RightMag);

*/
