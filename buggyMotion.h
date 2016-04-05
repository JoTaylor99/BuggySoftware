// buggyMotion.h

#ifndef _BUGGYMOTION_H
#define _BUGGYMOTION_H

#include "Config.h"

/// <summary>
/// buggyMotion is the parent class of navigation and contains the code for controlling stepper motors
/// </summary>
class buggyMotion  {

public:
	/// <summary>
	/// Constructor
	/// </summary>
	buggyMotion();
	/// <summary>
	/// Destructor
	/// </summary>
	~buggyMotion();
	
	/// <summary>
	/// Initialises motion, enbales the pwm timers and sets the initial state of control flags
	/// </summary>
	void initMotion();
	
protected:
	/// <summary>
	/// Main driving function. Called whenever the buggy's motion needs to be updated.
	/// </summary>
	/// <param name="direction">The direction you want the buggy to move</param>
	/// <param name="drift">If the buggy is drifting, set this parameter to be the direction the buggy is drifting relative to the direction it is going.</param>
	void drive(nC::Direction direction, nC::Drift drift = nC::Drift::noDrift);

	void getSpeeds(int32_t &leftSpeed, int32_t &rightSpeed);


private:
	/// <summary>
	/// Provides control of each motors direction, the speed of the motors will be determined by the variables _leftSpeed and _rightSpeed
	/// </summary>
	/// <param name="leftMotor">The direction the left wheel will turn</param>
	/// <param name="rightMotor">The direction the right wheel will turn</param>
	void stepperControl(nC::Direction leftMotor, nC::Direction rightMotor);
	/// <summary>
	/// Used for controlling specifically the Left motor's direction
	/// </summary>
	/// <param name="dir">The direction for the motor</param>
	void setLeftMotorDirection(nC::Direction dir);
	/// <summary>
	/// Used for controlling specifically the Right motor's direction
	/// </summary>
	/// <param name="dir">The direction for the motor</param>
	void setRightMotorDirection(nC::Direction dir);
	/// <summary>
	/// Sets the speed of the right motor
	/// </summary>
	/// <param name="freq">The frequency of the pwm signal that will be generated to drive the right stepper</param>
	void setRightSpeed(int32_t freq);
	/// <summary>
	/// Sets the speed of the left motor
	/// </summary>
	/// <param name="freq">The frequency of the pwm signal that will be generated to drive the right stepper</param>
	void setLeftSpeed(int32_t freq);
	/// <summary>
	/// Called whenever a drift is detected, and will append the variables _leftSpeed and _rigthSpeed to counter the drift
	/// </summary>
	/// <param name="direction">The direction the buggy is moving</param>
	/// <param name="drift">The direction the buggy is drifting relative to the current direction of the buggy to be the front.</param>
	void driftCorrect(nC::Direction direction, nC::Drift drift);
	/// <summary>
	/// Stop, Resets the buggy's motion ready for the next direction. This must be called between direction changes to ensure speed
	/// </summary>
	void stop();

	/// <summary>
	/// Stores the left wheel's speed
	/// </summary>
	int32_t _leftSpeed;
	/// <summary>
	/// Stores the right wheel's speed
	/// </summary>
	int32_t _rightSpeed;
	/// <summary>
	/// Indicates weither the timers for controlling the pwm have been enabled
	/// </summary>
	bool _timersInitialised = false;
	/// <summary>
	/// Due to the control flow though the buggy, drive() may be called multiple times during a single motion. This flag indicates to the stepper control that this is the first time this instruction is being called
	/// </summary>
	bool _firstCall = true;
	/// <summary>
	/// Storing the current drift, used for determening if setPinPWM needs to be updated
	/// </summary>
	bool _drifting;

	/// <summary>
	/// Stores the current direction of the buggy.
	/// </summary>
	nC::Direction CurrentDirection = nC::Direction::Stop;

	uint8_t _driftCount = 0;

	nC::Drift _previousDrift = nC::Drift::noDrift;
};


#endif

