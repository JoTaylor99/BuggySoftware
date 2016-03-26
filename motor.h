// motor.h

#ifndef _MOTOR_H
#define _MOTOR_H


#include "Config.h"
#include <Adafruit_MCP23017.h>

class motor {

public:
	/* motor constructor*/
	motor();

	/* motor destructor*/
	~motor();

protected:

	void MotorControl(mC::Direction LDir, mC::Direction RDir, byte LSpeed, byte RSpeed);

	/* motor init function
	*  Void return type
	*  Will depends on communications class for sending error codes or debug statements
	*  Sets up
	*/
	void initMotors();

private:
	Adafruit_MCP23017 GP;
	static bool _motorInitComplete;

	const byte dir1PinA = DIR1PINA;
	const byte dir2PinA = DIR2PINA;
	const byte speedPinA = SPEEDPINA; // Needs to be a PWM pin to be able to control motor speed

	// Motor 2
	const byte dir1PinB = DIR1PINB;
	const byte dir2PinB = DIR2PINB;
	const byte speedPinB = SPEEDPINB; // Needs to be a PWM pin to be able to control motor speed
	 
	/* checkMotorConfigCorrect function
	* checks to ensure all initialization functions have been run,
	*	if any have not been called the an object will be created and the relevant init function run again.
	*	return type void
	*/

	void checkMotorConfigCorrect();

};

#endif


/* UNUSED VARIABLES
*
* int M1Speed declared in global scope and not presently referenced
* int M1Speed = 0;
*
* int M2Speed declared in global scope and not presently referenced
* int M2Speed = 0;
*
*
* UNUSED FUNCTIONS
*
*function  declared in global scope but not defined or ever called:
*void MotorRamp(Direction Dir, int LMag, int RMag);
*
*
*
*/
