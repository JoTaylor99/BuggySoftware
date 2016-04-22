#ifndef _NAVIGATION_H
#define _NAVIGATION_H

#include "Config.h"
#include "buggyMotion.h"
#include "Sensor.h"
#include "box.h"
#include <NewPing.h>


class navigation : public buggyMotion {

	//Test commit changes.
public:
	/* Navigation constructor*/
	navigation();

	/* Navigation destructor*/
	~navigation();

	/* Navigation init function
	*  Void return type
	*  Will depends on communications class for sending error codes or debug statements
	*  Sets up
	*/
	void initNavigation();

	/* navigate function
	* return type is byte
	*
	*/
	void navigate(String str);

	uint8_t passedBoxNumber;

	bool passedBoxInversion;

#ifdef QTRSINUSE
	Sensor Sensors[9] = { FRPIN, LTRPIN, LTLPIN, FLPIN, MRPIN, MLPIN, BRPIN, BLPIN, { (unsigned int)7, sensorConfig::QTR } };
#else
	Sensor Sensors[8] = { FRPIN, LTRPIN, LTLPIN, FLPIN, MRPIN, MLPIN, BRPIN, BLPIN };
#endif
	
private:
	
#ifdef SENSOR_MEMORY_SAVE
	bool startValIs(sC::sensorNumber position);
	uint8_t startingValues = 0;
#else
	bool startingValues[NUM_SENSORS] = {};
#endif

	static bool _navInitComplete;

	void boxApproach();

	void start(); //Captures and stores in an array all the sensor values at the initial node position

	  /// <summary>
	  /// driftingWhenForward
	 /// Determines if the buggy  drifts away from its course in the forward motion.
	 /// If the buggy is drifting, it fixes the drift and returns true 
	 /// Else if the buggy is in the correct position it returns false
	 /// </summary>
	 /// <returns></returns>
	bool driftingWhenForward();

	/// <summary>
	/// driftingWhenBackward
	/// Determines if the buggy  drifts away from its course in the backward motion.
	/// If the buggy is drifting, it fixes the drift and returns true 
	/// Else if the buggy is in the correct position it returns false
	/// </summary>
	/// <returns></returns>
	bool driftingWhenBackward();

	/// <summary>
	/// adjustOnTheSpot
	/// Is called when ML and MR have just passed the destination intersection
	/// ((i.e RVAL(ML)!=STARTVAL(ML) && RVAL(MR) != STARTVAL(MR))&& (LASTVAL(
	/// checks the readings of LTL/LTR and BL/BR and rotates or moves horizontally (if applicable) on the spot
	/// to ensure LTL!=LTR and BL!=BR which means the buggy reached the destination successfully.
	/// </summary>
	void adjustOnTheSpot();
	
	//checks if the readings of all the sensors are opposite with their corresponding starting reading (i.e. the buggy has reached its destination)
	bool reachedDestination();

	//checks if the centre of rotation of the buggy is at the top of the destination intersection
	bool buggyCentreOnTopofDestIntersection();

	// Checks if the centre of rotation is behhind the destination intersection 
	//but the front of the buggy has passed the destination intersection
	bool buggyCentreBehindDestIntersection();

	void turnLeft();
	void turnRight();
	void moveForward(bool approachingBox = 0);
	void moveBackward();
	void victoryRoll();
	void boxBeGone();

	bool compareAllWithStart();
	bool compareAllToLast();

	uint8_t dockFailureCounter = 0;
	
};

#endif
