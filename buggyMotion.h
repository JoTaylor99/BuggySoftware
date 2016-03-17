// buggyMotion.h

#ifndef _BUGGYMOTION_H
#define _BUGGYMOTION_H

#include "Config.h"
#include "motor.h"

class buggyMotion : private motor {

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

	enum KickDirection :uint8_t {
		Forward, Backward, Left, Right
	};

	void Kick(KickDirection dir, int Magnitude);
	void Kick(KickDirection dir, int Magnitude, uint16_t time);
	void Kick(KickDirection dir, int LeftMagnitude, int RightMagnitude);
	void Kick(KickDirection LeftDir, KickDirection RightDir, int LeftMag, int RightMag);

	void drive(motorConfig::Direction LDir, motorConfig::Direction RDir, byte LSpeed = 0, byte RSpeed = 0);
private:
	static bool _motionInitComplete;
	

	/* checkMotionConfigCorrect function
	* checks to ensure all initialization functions have been run,
	*	if any have not been called the an object will be created and the relevant init function run again.
	*	return type void
	*/

	void checkMotionConfigCorrect();

};


#endif

