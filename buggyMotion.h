// buggyMotion.h

#ifndef _BUGGYMOTION_H
#define _BUGGYMOTION_H

#include "Config.h"
#include <arduino2.h>
#include <PWM.h>

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
	
	/// <summary>
	/// Friend declaration of the leftStepCounter ISR
	/// </summary>
	friend void leftStepCounter();

	/// <summary>
	/// Friend declaration of the rightStepCounter ISR
	/// </summary>
	friend void rightStepCounter();

protected:
	/// <summary>
	/// Main driving function. Called whenever the buggy's motion needs to be updated.
	/// </summary>
	/// <param name="direction">The direction you want the buggy to move</param>
	/// <param name="drift">If the buggy is drifting, set this parameter to be the direction the buggy is drifting relative to the direction it is going.</param>
	void drive(nC::Direction direction, nC::Drift drift = nC::Drift::noDrift);

	void getSpeeds(int32_t &leftSpeed, int32_t &rightSpeed);

	/// <summary>
	/// steps the desired distance in mm in the requested direction 
	/// Sets _leftWheelTask and/or _rightWheelTask to false
	/// </summary>
	/// <param name="direction">Direction to turn wheels</param>
	/// <param name="distance">Distance in mm to turn wheels MAX 250</param>
	void step(nC::Direction direction, uint8_t distance = DEFAULTMAXDISTANCE);

	/// <summary>
	/// steps the desired distance in mm in the requested direction, allows the setting of different targets for each wheel
	/// Sets _leftWheelTask and/or _rightWheelTask to false
	/// </summary>
	/// <param name="direction"></param>
	/// <param name="leftDistance">Distance in mm to turn left wheel MAX 250</param>
	/// <param name="rightDistance">Distance in mm to turn right wheel MAX 250</param>
	void stepSeparately(nC::Direction direction, uint8_t leftDistance = DEFAULTMAXDISTANCE, uint8_t rightDistance = DEFAULTMAXDISTANCE);


	/// <summary>
	/// If both leftWheelTask and rightWheelTask are true then return true
	/// </summary>
	/// <returns></returns>
	bool isMoveComplete();

	/// <summary>
	/// Calls a series of step commands to move the buggy horizontally left and right while remaining in the same position in the forward/backward axis.
	/// </summary>
	/// <param name="direction">left = 0, right = 1</param>
	/// <param name="distance">distance in mm for centre of buggy to move in specified direction</param>
	void moveHorizontally(bool direction, uint8_t distance);

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
	/// Stop, Resets the buggy's motion ready for the next direction. This must be called between direction changes to ensure speed defaults.
	/// Also detaches interrupts if enabled and clears appropriate flags, resets distance targets and counters.
	/// </summary>
	/// <param name="motorSelect"> 0 = default, stop both motors, 1 = stop left motor, 2 = stop right motor</param>
	void stop(uint8_t motorSelect = 0);


	void capSpeeds();

	/// <summary>
	/// Stores the left wheel's speed
	/// </summary>
	volatile int32_t _leftSpeed;
	/// <summary>
	/// Stores the right wheel's speed
	/// </summary>
	volatile int32_t _rightSpeed;
	/// <summary>
	/// Indicates whether the timers for controlling the pwm have been enabled
	/// </summary>
	bool _timersInitialised = false;
	/// <summary>
	/// Due to the control flow though the buggy, drive() may be called multiple times during a single motion. This flag indicates to the stepper control that this is the first time this instruction is being called
	/// </summary>
	volatile bool _firstCall = true;
	/// <summary>
	/// Storing the current drift, used for determening if setPinPWM needs to be updated
	/// </summary>
	bool _drifting;

	/// <summary>
	/// Stores the current direction of the buggy.
	/// </summary>
	nC::Direction CurrentDirection = nC::Direction::Stop;

	volatile uint8_t _driftCount = 0;

	nC::Drift _previousDrift = nC::Drift::noDrift;


	/// <summary>
	/// Returns the number of counts required to travel the number of millimeters passed
	/// note this is twice the number of steps due to the pin change interrupts counting both rising and falling edges.
	/// </summary>
	/// <param name="mmDistance">distance in mm</param>
	/// <returns>steps</returns>
	uint16_t getStepsFromDistance(uint8_t mmDistance);

	volatile uint16_t stepDistanceLeft = 0;
	volatile uint16_t stepDistanceRight = 0;

	volatile uint16_t stepTargetDistanceLeft = 1;
	volatile uint16_t stepTargetDistanceRight = 1;

	volatile bool _leftWheelTask = true;
	volatile bool _rightWheelTask = true;

	void pciSetup(uint8_t pin);
};


//Handler Left
extern void leftStepCounter();

//Handler Right
extern void rightStepCounter();

#endif

