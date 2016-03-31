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

	/// <summary>
	/// Sets the PWM value of both motors
	/// </summary>
	/// <param name="motorSpeed">PWM value to set, range 0-255</param>
	void setMotorSpeed(byte motorSpeed);

	/// <summary>
	/// Sets the PWM value of each motor
	/// </summary>
	/// <param name="motorASpeed">Motor A PWM value to set, must be 0-255</param>
	/// <param name="motorBSpeed">Motor B PWM value to set, must be 0-255</param>
	void setMotorSpeed(byte motorASpeed, byte motorBSpeed);

	/// <summary>
	/// Sets the direction of each motor
	/// If stop is passed to both parameters stop will occur immediately
	/// If stop is passed to ONE OF the parameters the direction assignment for the other will be made and then stop will be asserted.
	/// </summary>
	/// <param name="motorADirection">Motor A Direction to set</param>
	/// <param name="motorBDirection">Motor B Direction to set</param>
	void setMotorDirection(mC::Direction motorADirection, mC::Direction motorBDirection);

	/// <summary>
	/// Sets the direction of both motors, function called with stop to stop all
	/// </summary>
	/// <param name="motorDirection">Direction to set</param>
	void setMotorDirection(mC::Direction motorDirection);
		
	/// <summary>
	/// Motor init function
	/// Sets up pin directions
	/// </summary>
	void initMotors();

	void MotorControl(mC::Direction LDir, mC::Direction RDir, byte LSpeed, byte RSpeed);
private:
	static bool _motorInitComplete;

	
	// Direction Pin Motor A
	const byte dirPinA = DIRPINA;

	
	// Speed Pin Motor A
	// Must be a PWM pin
	const byte speedPinA = SPEEDPINA;

	// Direction Pin Motor B
	const byte dirPinB = DIRPINB;

	// Speed Pin Motor B
	// Must be a PWM pin
	const byte speedPinB = SPEEDPINB; // Needs to be a PWM pin to be able to control motor speed

	/// <summary>
	/// checks to ensure all initialization functions have been run
	/// if any have not been called then the relevant init function will be called.
	/// </summary>
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
