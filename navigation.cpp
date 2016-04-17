// 
// 
// 

#include "navigation.h"

//Note sensors are read using RVAL(sensorNumber) e.g. RVAL(sC::LTL), RVAL(sC::FR), etc.
//Previous values are read using RLASTVAL(sensorNumber)
//Starting values are read using STARTVAL(sensorNumber)
//test commit changes

navigation::navigation() {
	
}

navigation::~navigation() {
}

void navigation::initNavigation() {
	//-------Setup Pins used for sensor select as outputs---------------
	
	Sensor::initSensors();
	initMotion();
	
	//box assessBox;
	//assessBox.init();

	Sensor::PollSensors(Sensors);
#ifdef SENSOR_DEBUG
	Sensor::PollSensors(Sensors);
	Sensor::PollSensors(Sensors);
	Sensor::PollSensors(Sensors);
	Sensor::PollSensors(Sensors);
	Sensor::PollSensors(Sensors);
	Sensor::PollSensors(Sensors);
#endif

	DEBUG_PRINTLN("Setup Complete!");

};

void navigation::navigate(String str) {

		Sensor::PollSensors(Sensors);
			
			if (str == "F") {
				start();
				moveForward();
				/*unsigned long tim = 0;
				unsigned long tim2 = 0;

				drive(nC::Direction::Stop, nC::Drift::noDrift);
				tim = micros();
				
				drive(nC::Direction::Forward, nC::Drift::noDrift);
				tim2 = micros();
				Serial.println(tim2 - tim,DEC);

				tim = micros();
				drive(nC::Direction::Forward, nC::Drift::noDrift);
				tim2 = micros();
				Serial.println(tim2 - tim, DEC);

				tim = micros();
				drive(nC::Direction::Stop, nC::Drift::noDrift);
				tim2 = micros();
				Serial.println(tim2 - tim, DEC);

*/
			} else if (str == "B") {
				start();
				moveBackward();
			} else if (str == "R") {
				start();
				turnRight();
			} else if (str == "L") {
				start();
				turnLeft();
			}
			else if (str == "G") {
				boxApproach();
#ifdef BOX_DEBUG
				//pass recieved boxnumber and recieved box inversion information
				box boxs;
				boxs.begin(BOXNUM, BOXINV);
				if (!boxs.docked()) {
					//redock unless already failed twice
				}
				else {
					boxs.interrogateBox();
				}
#endif
			} else if (str == "S") {
				drive(nC::Direction::Stop);

			}
			else {
				drive(nC::Direction::Stop);
			}
};

void navigation::boxApproach() {
		uint8_t Distance = 0;
		NewPing Ultrasonic(TRIGGER, ECHO, MAXDISTANCE);
		drive(nC::Direction::Forward);
		while (true) {
			Distance = Ultrasonic.ping_cm();
			DEBUG_VPRINTLN(Distance);
			if (Distance > 3) {
				DEBUG_PRINTLN("Approaching Box");
				Sensor::PollSensors(Sensors, Sensor::Front, 2);
			} else {
				DEBUG_PRINTLN("I Have Reached the box");
				drive(nC::Direction::Stop);
				if (Distance == 0) {
					DEBUG_PRINTLN("Either too far or too close");
				}
				return;
			}
			delay(50);
		}
	}

//captures and stores in an array all the sensor values at the initial node position
void navigation::start() {
	Sensor::PollSensors(Sensors);
	int32_t lspeed, rspeed;
	getSpeeds(lspeed, rspeed);
	NAV_VPRINTLN(lspeed); NAV_VPRINTLN(rspeed);
#ifndef SENSOR_MEMORY_SAVE
	memcpy(startingValues, Sensor::values, NUM_SENSORS);
#else
	startingValues = Sensor::values;
#endif
	DEBUG_PRINTLN("I have the starting POSITION");
}
//Determines if the buggy reached the destination intersection correctly
bool navigation::reachedDestination() {
	if ( 
		(RVAL(sC::ML) != STARTVAL(sC::ML))&&  
		(RVAL(sC::MR) != STARTVAL(sC::MR))) {
		return true;
	}
	else
	{
		return false;
	}
}

//Determines if the centre of rotation of the buggy has just arrived at the top of the destination Intersection
bool navigation::buggyCentreOnTopofDestIntersection() {
	if ((RVAL(sC::FL) != STARTVAL(sC::FL)) &&
		(RVAL(sC::FR) != STARTVAL(sC::FR)) &&
		(RVAL(sC::ML) != STARTVAL(sC::ML)) &&
		(RVAL(sC::MR) != STARTVAL(sC::MR)) &&
		((RLASTVAL(sC::ML) == STARTVAL(sC::ML)) || (RLASTVAL(sC::MR) == STARTVAL(sC::MR)))) {
		return true;
	}
	else {
		return false;
	}
}
//checks if centre of rotation of the buggy is behind the intersection while front of the buggy has passed
bool navigation::buggyCentreBehindDestIntersection() {
	if ((RVAL(sC::FL) != STARTVAL(sC::FL)) &&
		(RVAL(sC::FR) != STARTVAL(sC::FR)) &&
		(RVAL(sC::ML) == STARTVAL(sC::ML)) &&
		(RVAL(sC::MR) == STARTVAL(sC::MR))) {
		return true;
	}
	else {
		return false;
	}
}

/// driftingWhenForward Algorithm
/// If (LTL!= LTR )
///		Buggy follows correctly the line
///		return false;
/// else {
///			if (FR == START(FR)  AND FL == START(FL){
///				Front of the buggy has not passed the destination intersection line yet
///				Arena pattern has not flipped yet
///				If (LTL!= START(LTL){
///					drive Left so that LTL becomes LTL=START(LTL);
///				}
///				else {
///					drive right so that LTR becomes LTR = START(LTR);
///				}
///			}
///			else{
///				Arena pattern has flipped/ front of the buggy has passed intersection line
///				if (If LTL== START(LTL){
///					drive left so that LTL becomes LTL!= START(LTL)
///					}
///				else{
///					drive right so that LTR becomes LTR!= START(LTR)
///			}
///		return true;
/// }
bool navigation::driftingWhenForward() {
	if ((RVAL(sC::LTL) && RLASTVAL(sC::LTL)) != (RVAL(sC::LTR) && RLASTVAL(sC::LTR))) {
		return false;
	}
	else{
		//Deviation from the course
		if ((RVAL(sC::FL) == STARTVAL(sC::FL)) && (RVAL(sC::FR) == STARTVAL(sC::FR))) {
			//pattern of arena not flipped yet
			if (RVAL(sC::LTL) != STARTVAL(sC::LTL)) {
				drive(nC::Direction::Forward,nC::Drift::leftDrift);
			}
			else {
				drive(nC::Direction::Forward, nC::Drift::rightDrift);

			}

		}
		else {
			if (RVAL(sC::LTL) == STARTVAL(sC::LTL)) {
				drive(nC::Direction::Forward, nC::Drift::rightDrift);

			}
			else{
				drive(nC::Direction::Forward, nC::Drift::leftDrift);

			}
		}
		return true;
	}
}
/// <summary>
/// driftingWhenBackward Algorithm
/// Aims to avoid the switching pattern of the arena 
/// by following the line backwards with LTL and LTR until FR and FL flipps
/// Then it switches to follow the line backwards with BR and BL that already crossed the destination intersection
/// check documentation for the reason 
///     if FR AND FL have not flipped values (i.e. front of the buggy has not passed the initial intersection line {
///			follow the line backwards with LTL and LTR 
///			If(LTL != LTR){
///				Buggy follows correctly the line
///				return false;
///			}
///			else if (LTL ==START(LTL){
///				Drive Backwards Left so that LTR will move to the RIGHT
///				return true;
///			}
///			else {
///					Drive Backwards Right so that LTL will move to the	LEFT
///					return true;
///			}
///		}
///		else{ Arena pattern has flipped
///			follow the line with BR and BL
///			if (BL!=BR){
///				Buggy follows the line backwards correctly
///				return false;
///			}
///			else if (BL==STARTVAL BL){
///				drive backwards Left so that  BL will move to the left
///				return true;
///			else{
///					Drive Backwards Right so that BR will move to the Right
///					return true;
///			}
///		}
/// </summary>
/// <returns></returns>
bool navigation::driftingWhenBackward() {
	if ((RVAL(sC::FL) == STARTVAL(sC::FL)) && (RVAL(sC::FR) == STARTVAL(sC::FR))) {
		//Front of the buggy has not passed the initial intersection line follow the line Backwards with LTL and LTR
		if (RVAL(sC::LTL) != RVAL(sC::LTR)) {
			//correct position
			return false;
		}
		else if (RVAL(sC::LTL) == STARTVAL(sC::LTL)) {
			//drive BACKWARDS LEFT( NOT  JUST LEFT)
			drive(nC::Direction::Backwards, nC::Drift::leftDrift);

			return true;
		}
		else {
			//drive backwards right 
			drive(nC::Direction::Backwards, nC::Drift::rightDrift);

			return true;
		}
	}
	else{
		//Front of the buggy has passed initial intersection line + Bottom of the buggy has passed destination intersection line
	    // Pattern of arena changed, follow the line backwards with BR and BL
		if (RVAL(sC::BL) != RVAL(sC::BR)){
			//correct position
			return false;
		}
		else if (RVAL(sC::BL) == STARTVAL(sC::BL)) {
			//Drive Backwards Left
			drive(nC::Direction::Backwards, nC::Drift::leftDrift);

			return true;
		}
		else {
			//drive backwards right 
			drive(nC::Direction::Backwards, nC::Drift::rightDrift);

			return true;
		}
	}
}



/// <summary>
/// adjustOnTheSpot is called when ML and MR have just crossed the destination intersection line
/// Aims to get the buggy to have correct finish position 
/// adjustOnTheSpot algorithm
/// Case 1 horizontal movement because centre of rotation is not exactly at the top of the intersection 
///		   but its on the horizontal destination intersection line
/// if ((LTL==LTR)&& (BL==BR) && (LTL!=BL){
///		IF (LTL==STARTVAL(LTL){
///			Move Horizontally Left
///		}
///		else{
///			Move Horizontally Right
///		}
/// }
/// Case 2 Rotation on the spot. Centre of Rotation is at the top of destination intersection 
///		   But the facade or the buggy are at an angle (not straight following the line)
/// else if ((LTL == LTR && LTL==BL && LTL ==BR) {
///		if (LTL==STARTVAL(LTL){
///			ROTATE LEFT
///		}
///		else{
///			ROTATE RIGHT
///		}
/// }
/// Case 3 Just one wrong reading of the 4 sensors . That means LTL!= LTR && BR==BL  OR LTL==LTR && BR !=BL
/// THEN accordingly with the case let one wheel stable and move forward or backward the other one to fix the situation
///	else if  (LTL!= LTR && BL==BR){
///		If (BL==STARTVAL(BL){
///			LEFT WHEEL FORWARD, RIGHT WHEEL STOP
///		}
///		else {
///			LEFT WHEEL STOP, RIGHT WHEEL FORWARD
///		}
/// 
/// else if (LTL==LTR && BR !=BL){
///		If (LTL!=STARTVAL (LTL){
///			LEFT WHEEL STOP, RIGHT WHEEL BACKWARDS
///		}
///		else {
///				LEFT WHEEL BACKWARDS, RIGHT WHEEL STOP
///		}
/// }
/// else {
///		null;
/// }
/// </summary>
void navigation::adjustOnTheSpot(){
	//Case 1 if ((LTL==LTR)&& (BL==BR) && (LTL!=BL)
	if ((RVAL(sC::LTL) == RVAL(sC::LTR)) && (RVAL(sC::BL) == RVAL(sC::BR)) && (RVAL(sC::LTL) != RVAL(sC::BL))) {
		if (RVAL(sC::LTL) == STARTVAL(sC::LTL)) {
			//Move Horizontally to the left
		}
		else {
			//Move Horizontally to the Right
		}
	}
	// Case 2 ((LTL == LTR && LTL==BL && LTL ==BR)
	else if ((RVAL(sC::LTL) == RVAL(sC::LTR)) && (RVAL(sC::LTL) == RVAL(sC::BR)) && (RVAL(sC::LTL) == RVAL(sC::BL))) {
		if (RVAL(sC::LTL) == STARTVAL(sC::LTL)) {
			//drive(nC::Direction::Left);
		}
		else {
			//drive(nC::Direction::Right);
		}
	}
	//Case 3 (LTL!= LTR) && (BR==BL)   
	else if ((RVAL(sC::LTL) != RVAL(sC::LTR)) && (RVAL(sC::BL) == RVAL(sC::BR))) {
		if (RVAL(sC::BL) == STARTVAL(sC::BL)) {
			//LEFT WHEEL FORWARD, RIGHT WHEEL STOP
		}
		else {
			//LEFT WHEEL STOP, RIGHT WHEEL FORWARD
		}
	}
	//Case 3.1 (LTL==LTR) && (BR !=BL)
	else if ((RVAL(sC::LTL) == RVAL(sC::LTR)) && (RVAL(sC::BL) != RVAL(sC::BR))) {
		if (RVAL(sC::LTL) != STARTVAL(sC::LTL)) {
			//LEFT WHEEL STOP, RIGHT WHEEL BACKWARD
		}
		else {
			//LEFT WHEEL BACKWARD, RIGHT WHEEL STOP
		}
	}
}

//Function to turn left
/// <summary>
/// Algorithm turnLeft1
/// While (true)
///		If the buggy centre of rotation is at the top of intersection{
///			if buggy has perfectly final position{
///				Break
///				Finish Movement
///			}
///			else{
///			Adjust on the spot
///			}
///		}
///		else if ( LTL == STARTVAL(LTL)) {
///			ROTATE LEFT --Actual Rotation movement
///		}
///		else if (RVAL(LTL)== RVAL (LTR)){
///			//Overshoot between LTL and LTR
///			if (RVAL(LTR) == STARTVAL(LTR) {
///				ROTATE RIGHT  (OVERSHOOT TO THE LEFT)
///			}
///			else{
///				ROTATE LEFT (OVERSHOOT TO THE RIGHT)
///			}
///		}
///		else if  ((RVAL(BL)==RVAL(BR))|| (RVAL (ML) == RVAL (MR) ){
///				Overshoot between the middle or Bottom Sensors
///				Follow line forward to fix overshoot
///		}
///		else{
///			//Ensure Centre of Rotation is at the top of intersection
///			 if ((RVAL(ML)!= STARTVAL(ML)) {
///				 FOLLOW THE LINE BACKWARDS
///			 }
///			 else{
///				FOLLOW THE LINE FORWARDS
///			}
///		}
/// }	 
/// </summary>

/// <summary>
/// Assumes the centre of rotation of the buggy remains stable on top of the intersection during the rotation
/// Turnleft2Algorithm
/// While (true)
///		if buggy has perfectly final position{
///			Break
///			Finish Movement
///		}
///		else if ( LTL == STARTVAL(LTL)) {
///			ROTATE LEFT --Actual Rotation movement
///		}
///		else if (RVAL(LTL)== RVAL (LTR)){
///			//Overshoot between LTL and LTR
///			if (RVAL(LTR) == STARTVAL(LTR) {
///				ROTATE RIGHT  (OVERSHOOT TO THE LEFT)
///			}
///			else{
///				ROTATE LEFT (OVERSHOOT TO THE RIGHT)
///			}
///		}
///		else{
///			adjustOnTheSpot
///		}
/// </summary>
void navigation::turnLeft() {
	while (true) {
		Sensor::PollSensors(Sensors);
		if (navigation::reachedDestination()) {
			drive(nC::Direction::Stop);
			break;
		}
		else if (RVAL(sC::LTL) == STARTVAL(sC::LTL)) {
			drive(nC::Direction::Left);
		}
		else if (RVAL(sC::LTL) == RVAL(sC::LTR)) {
			//overshoot between LTL and LTR
			if (RVAL(sC::LTR) == STARTVAL(sC::LTR)) {
				drive(nC::Direction::Right);
			}
			else {
				drive(nC::Direction::Left);
			}
		}
		else {
			navigation::adjustOnTheSpot();
		}
	}
}




/// <summary>
/// Same algorithm as RotateLeft
/// Only difference is the direction of rotation  which is Right
/// 
/// </summary>
//Function to turn right.
void navigation::turnRight() {
	while (true) {
		Sensor::PollSensors(Sensors);
		if (navigation::reachedDestination()) {
			drive(nC::Direction::Stop);
			break;
		}
		else if (RVAL(sC::LTR) == STARTVAL(sC::LTR)) {
			drive(nC::Direction::Right);
		}
		else if (RVAL(sC::LTL) == RVAL(sC::LTR)) {
			//overshoot between LTL and LTR
			if (RVAL(sC::LTR) == STARTVAL(sC::LTR)) {
				drive(nC::Direction::Right);
			}
			else {
				drive(nC::Direction::Left);
			}
		}
		else {
			navigation::adjustOnTheSpot();
		}
	}
}


//Function to move forwards one node
/// <summary>
/// moveForward Algorithm
/// while (true){
///		Poll Sensors
///		if buggy reached destination{
///			stop
///			break;
///		}
///		else if buggy is at the top of the intersection but not perfectly aligned {
///			stop
///			adjustOnThespot with small movements adjust the buggy to the correct final position
///		}
///		else if drifting {
///			fix drifting
///		}
///		else{
///			drive forward;
///		}
///	}
/// 
/// </summary>
void navigation::moveForward() {
	NAV_PRINTLN("Moving Now!");
	while (true) {
		
		Sensor::PollSensors(Sensors);
		//m1 = micros()
		NAV_PRINTLN("In forward loop");
		if (navigation::reachedDestination()) {
			NAV_PRINTLN("Destination reached");

			drive(nC::Direction::Stop);
			break;
		}
		else if ((RVAL(sC::ML) != RVAL(sC::MR)) && ((RVAL(sC::ML) != STARTVAL(sC::ML)))) {
			NAV_PRINTLN("Adjust on Spot");
			
			drive(nC::Direction::Stop);
			break;
			//adjustOnTheSpot();
		}
		else if (!navigation::driftingWhenForward()) {
			NAV_PRINTLN("Who knows");
			drive(nC::Direction::Forward);
		}
	}
}
//Function to move backward a node
/// <summary>
/// moveBackward Algorithm /slightly changed from the data flow chart to avoid going backwards and forwards at the destination intersection
/// while(true){
///		Poll Sensors
///		if the centre of rotation is at the top of the destination  intersection 
///	(i.e. if ((RVAL(FL)!=STARTVAL(FL) && RVAL(FR)!=STARTVAL(FR))&&
///			  (RVAL(ML)!=STARTVAL(ML) && RVAL(MR)!=STARTVAL(MR))&& 
///			  ((LASTVAL(ML)==STARTVAL(ML) || LASTVAL(MR)==STARTVAL(MR)){
///			 if (RVAL(ALL)!= STARTVAL(ALL)){
///			    STOP 
///			    BREAK 
///			   	FINISH
///			}
///			else {
///				adjustOnTheSpot;
///			}
///	else if ML and MR have passed the destination intersection ( ML ==STARTVAL(ML)&& MR==STARTVAL(MR) && FL!=STARTVAL(FL)){
///				that means centre of rotation of the buggy is behind the intersection and the buggy needs to go forward
///				if (!drift){
///					go forward
///				}
///				else{
///					fix drift
///				}
/// }
/// else {
///		if (!drift)
///			go backwards to reach the destination intersection
///		else{
///			fix drift
///		}
/// }	
/// </summary>
void navigation::moveBackward() {
	while (true) {
		Sensor::PollSensors(Sensors);

		if ((RVAL(sC::BL) != STARTVAL(sC::BL)) && (RVAL(sC::BR) != STARTVAL(sC::BR)) && (RVAL(sC::ML) == STARTVAL(sC::ML)) && (RVAL(sC::MR) == STARTVAL(sC::MR))) {
			drive(nC::Stop);
			NAV_PRINTLN("Crossed Over");
			break;
		}
		else {
			NAV_PRINTLN("In front of intersection");

			if (!navigation::driftingWhenBackward()) {
				drive(nC::Direction::Backwards);
			}
		}
	}

	Sensor::PollSensors(Sensors);

		while ((RVAL(sC::ML) == STARTVAL(sC::ML)) && (RVAL(sC::MR) == STARTVAL(sC::MR))) {
			Sensor::PollSensors(Sensors);
			if (!navigation::driftingWhenForward()) {
				drive(nC::Direction::Forward);
			}
		}

		drive(nC::Stop);
}


#ifdef SENSOR_MEMORY_SAVE
bool navigation::startValIs(sC::sensorNumber position) {
	return bitRead(startingValues, position);
}
#endif

// Functions removed 25/03/16
/*//Bool function to indicate if the destination intersection has reached. True if the middle sensors are in the intersection or if perfect intersection achieved

//Detects if you are drifting while kicking and corrects it. Returns false
bool navigation::LineCorrect() {
	if (Sensor::Drifting(Sensors, LastCorrectLeft, LastCorrectRight) == Sensor::DriftDirection::DLeft) {
		if (Counter < COUNTER_LEN) {
			Kick(KickDirection::Forward, KickDirection::Backward, KICK_MAGNITUDE, KICK_MAGNITUDE);
			Counter++;
			return true;
		}
		else {
			Counter = 0;
			return false;
		}
	}
	else if (Sensor::Drifting(Sensors, LastCorrectLeft, LastCorrectRight) == Sensor::DriftDirection::DRight) {
		if (Counter < COUNTER_LEN) {
			Kick(KickDirection::Backward, KickDirection::Forward, KICK_MAGNITUDE, KICK_MAGNITUDE);
			Counter++;
			return true;
		}
		else {
			Counter = 0;
			return false;
		}
	}
	else if (Sensor::Drifting(Sensors, LastCorrectLeft, LastCorrectRight) == Sensor::DriftDirection::None) {
		Counter = 0;
		LastCorrectLeft = RVAL(sC::LTR);
		LastCorrectRight = RVAL(sC::FR);
		return false;
	}
	else {
		Counter = 0;
		return false;
	}
}
//Function to define whether or not the destination node has been reached.
//If so it returns true and prints "Destination Reached" to the terminal, if not returns falfse.
//Based on the quadrant the buggy rotates in a backward motion to get back to the motion line with sesnors 4 and 5
bool navigation::reachedDestination() {
	if (((RVAL(sC::FR)) != (START_VAL(sC::FR))) && (RVAL(sC::LTR) != START_VAL(sC::LTR)) && (RVAL(sC::LTL) != START_VAL(sC::LTL)) &&
		(RVAL(sC::FL) != START_VAL(sC::FL)) && (RVAL(sC::MR) != START_VAL(sC::MR)) && (RVAL(sC::ML) != START_VAL(sC::ML))) {
		DEBUG_PRINTLN("Destination Reached");
		return true;
	}
	//  else if ((RVAL(sC::LTL) != START_VAL(sC::LTL)) && (RVAL(sC::FL) != START_VAL(sC::FL))
	//           && (RVAL(sC::MR) != START_VAL(sC::MR)) && (RVAL(sC::ML) != START_VAL(sC::ML))) {
	//    Serial.println("Destination Reached2");
	//    return true;
	//  }
	else {
		return false;
	}
}
//----------------FORWARD ALIGNMENT----------------------------------------------------------------
//Checks if there was an event in the values of the first two sensors during the forward motion
void navigation::sensorEvents() {


	if (RVAL(sC::FR) != START_VAL(sC::FR)) {
		sensor0_event = true;
		flag0 = true;
	}
	if (RVAL(sC::LTR) != START_VAL(sC::LTR)) {
		sensor1_event = true;
		flag1 = true;
	}
}
//Checks if the buggy has passed the line of the destination intersection
//Optimised
bool navigation::didIPassIntersectionLine(Direction Dir) {
	bool passed_intersection_line = false;
	if (Dir == Forward) {
		//ideal-case Both sensors have passed the intersection line normally
		if ((RVAL(sC::FR) != START_VAL(sC::FR)) && (RVAL(sC::LTR) != START_VAL(sC::LTR)) || ((sensor0_event == true) && (sensor1_event == true)))
		{
			passed_intersection_line = true;
			DEBUG_PRINTLN("Yes I passed the intersection line");
		}
		else {
			passed_intersection_line = false;
			DEBUG_PRINTLN("No I havent passed intersection line");
		}
	}
	else {//Backward
		Sensor::PollSensors(Sensors);
		//ideal-case Both sensors have passed the intersection line normally
		if ((RVAL(sC::MR) != START_VAL(sC::MR)) && (RVAL(sC::ML) != START_VAL(sC::ML)) || ((sensor4_event == true) && (sensor5_event == true)))
		{
			passed_intersection_line = true;
			DEBUG_PRINTLN("Yes I passed the intersection line");
		}
		else {
			passed_intersection_line = false;
			DEBUG_PRINTLN("No I havent passed intersection line");
		}
	}


}*/
//Quadrants Description
//Forward  S0.S2 are in Q4, S1 and S3 are inQ1              //Backwards
//S4 Q3 and S5 Q2                            S4 at Q4, S5 at Q1, S3S1 at Q2, S2S0 at Q3
// Destination Node                            Destination Node
//Q2|Q1                                            Q1|Q2
//----|-----     Q0 is the on line case              --|--
//Q3|Q4                                            Q4 Q3
//
// Function to identify the quadrant where the buggy is at the moment
//Optimised
/*
uint8_t navigation::whereAmI(Direction Dir) {
	uint8_t i;
	bool passed_intersection = didIPassIntersectionLine(Dir);
	uint8_t quadrant;
	if (Dir == Backward) {
		i = 4;
	}
	else {
		i = 0;
	}

	if (RVAL(i) == RVAL(i + 1)) {
		if (passed_intersection == true) {
			if (RVAL(i) != START_VAL(i)) {
				DEBUG_PRINTLN("I am at Q4");
				quadrant = 4;
			}
			else {
				DEBUG_PRINTLN(" I am at Q1");
				quadrant = 1;
			}
		}
		else {
			if (RVAL(i) != START_VAL(i)) {
				DEBUG_PRINTLN("I am at Q2");
				quadrant = 2;
			}
			else {
				DEBUG_PRINTLN("I am at Q3");
				quadrant = 3;
			}
		}
	}
	else {
		quadrant = 0; //Relevant sensors on line (S0 & S1 if forwards, S4 & S5 if backwards). 

	}
	return quadrant;
}
void navigation::findLineS23(uint8_t quadrant) {
	bool smart_line_s45 = false;
	switch (quadrant) {
	case 1: while (smart_line_s45 == false) {
		//Sensor::PollSensors(Sensors, Back, 2);
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::LTL) != (RVAL(sC::FL))) {
			smart_line_s45 = true;
		}
		else {
			DEBUG_PRINTLN("Q1 NEED TO KICK BACKWARDS RIGHT");
			Kick(KickDirection::Backward, KickDirection::Forward, 255, 0);
		}
	}
			break;
	case 2: while (smart_line_s45 == false) {
		// Sensor::PollSensors(Sensors, Back, 2);
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::LTL) != (RVAL(sC::FL))) {
			smart_line_s45 = true;
		}
		else {
			DEBUG_PRINTLN("Q2 NEED TO KICK BACKWARDS RIGHT");
			Kick(KickDirection::Backward, KickDirection::Forward, KICK_MAGNITUDE, 0);

		}
	}
			break;
	case 3:  while (smart_line_s45 == false) {
		// Sensor::PollSensors(Sensors, Back, 2);
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::LTL) != (RVAL(sC::FL))) {
			smart_line_s45 = true;
		}
		else {
			DEBUG_PRINTLN("Q3 NEED TO KICK BACKWARDS LEFT");
			Kick(KickDirection::Forward, KickDirection::Backward, 0, KICK_MAGNITUDE);

		}
	}
			 break;
	case 4:  while (smart_line_s45 == false) {
		// Sensor::PollSensors(Sensors, Back, 2);
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::LTL) != (RVAL(sC::FL))) {
			smart_line_s45 = true;
		}
		else {
			DEBUG_PRINTLN("Q4 NEED TO KICK BACKWARDS LEFT");
			Kick(KickDirection::Forward, KickDirection::Backward, 0, KICK_MAGNITUDE);

		}
	}
			 break;
	default:
		break;
	}
}
// Based on the quadrant the buggy rotates left or right to get back to the motion line with sesnors 1 and 2
void navigation::findLineS01(uint8_t quadrant) {
	bool smart_line_s01 = false;
	switch (quadrant) {
	case 1: while (smart_line_s01 == false) {
		//   Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::FR) != (RVAL(sC::LTR))) {
			smart_line_s01 = true;
		}
		else {
			DEBUG_PRINTLN("Q1 NEED TO ROTATE RIGHT");
			Kick(KickDirection::Right, KICK_MAGNITUDE);

		}
	}
			break;
	case 2: while (smart_line_s01 == false) {
		//Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::FR) != (RVAL(sC::LTR))) {
			smart_line_s01 = true;
		}
		else {
			DEBUG_PRINTLN("Q2 NEED TO ROTATE RIGHT");
			Kick(KickDirection::Right, KICK_MAGNITUDE);

		}
	}
			break;
	case 3: while (smart_line_s01 == false) {
		//Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::FR) != (RVAL(sC::LTR))) {
			smart_line_s01 = true;
		}
		else {
			DEBUG_PRINTLN("Q3 NEED TO ROTATE LEFT");
			Kick(KickDirection::Left, KICK_MAGNITUDE);

		}
	}
			break;
	case 4: while (smart_line_s01 == false) {
		//  Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::FR) != (RVAL(sC::LTR))) {
			smart_line_s01 = true;
		}
		else {
			DEBUG_PRINTLN("Q4 NEED TO ROTATE LEFT");
			Kick(KickDirection::Left, KICK_MAGNITUDE);
		}
	}
			break;
	default:
		break;
	}
}
//Identifies whether sensors 0 and 1 have passed the node in the forward motion
void navigation::passedNote() {
	bool passed_s01 = false;
	while (passed_s01 == false) {
		Sensor::PollSensors(Sensors, Sensor::Front, 2);
		//Sensor::PollSensors(Sensors, DefaultOrder, 6);
		if (reachedDestination() == true) {
			return;
		}
		if ((RVAL(sC::FR) != START_VAL(sC::FR)) && (RVAL(sC::LTR) != START_VAL(sC::LTR))) {
			passed_s01 = true;
		}
		else {
			DEBUG_PRINTLN("Need to kick forward");
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}
//Function to tell if have reached the node or not yet.
//Optimised
void navigation::reachedNode(Direction Dir) {
	//If forward, S2 &S3, if backward, S4&S5
	bool passed = false;
	while (passed == false) {
		//Sensor::PollSensors(Sensors, FrontM, 2);
		Sensor::PollSensors(Sensors);
		if (reachedDestination() == true) {
			return;
		}
		if (Dir == Forward) {
			if ((RVAL(sC::LTL) != START_VAL(sC::FR)) && (RVAL(sC::FL) != START_VAL(sC::LTR))) {
				passed = true;
			}
			else {
				DEBUG_PRINTLN("Need to kick steadily forward on reach_node");
				if (LineCorrect() == false) {
					Kick(KickDirection::Forward, KICK_MAGNITUDE);
				}
			}
		}
		else {
			if ((RVAL(sC::MR) != START_VAL(sC::MR)) && (RVAL(sC::ML) != START_VAL(sC::ML))) {
				passed = true;
			}
			else {
				DEBUG_PRINTLN("Need to kick steadily backward");
				if (LineCorrect() == false) {
					Kick(KickDirection::Backward, KICK_MAGNITUDE);
				}
			}
		}
	}
}
//Smart Alignment Function for Forward/Backward movement.
//Optimised
void navigation::smartAlignment(Direction Dir) {
	bool perfect_intersection = false;
	uint8_t quadrant = whereAmI(Dir);
	while (perfect_intersection == false) {
		Sensor::PollSensors(Sensors);
		if ((RVAL(sC::FR) != START_VAL(sC::FR)) && (RVAL(sC::LTR) != START_VAL(sC::LTR)) && (RVAL(sC::LTL) != START_VAL(sC::LTL)) &&
			(RVAL(sC::FL) != START_VAL(sC::FL)) && (RVAL(sC::MR) != START_VAL(sC::MR)) && (RVAL(sC::ML) != START_VAL(sC::ML))) {
			perfect_intersection = true;
			DEBUG_PRINTLN(" FINISHED");
		}
		else {
			findLineS23(quadrant);
			if (reachedDestination() == true) {
				return;
			}
			findLineS01(quadrant);
			// on_line_all();
			if (reachedDestination() == true) {
				return;
			}
			if (Dir == Forward) {
				passedNote();
			}
			else {
				passedNoteB();
			}
			if (reachedDestination() == true) {
				return;
			}
			reachedNode(Dir);
			if (reachedDestination() == true) {
				return;
			}
		}
	}
}
//----------------ROTATION ALIGNMENT----------------------------------------------------------------------------------------------------------------------------------------------------
//Rotation strategy
//1) Get back to the line with s0 and s1
//2) Follow the line by going forwards  to align the entire buggy with it
//3) steadily go backwards, avoid drift away from the line to find the intersection
//// Brings the buggy back to line at the rotation movement --Horizontal alignment
void navigation::getBackToLineRotation() {
	bool back_to_line = false;
	while (back_to_line == false) {
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::FR) == RVAL(sC::LTR)) {
			if ((RVAL(sC::FR) == START_VAL(sC::FR))) {
				DEBUG_PRINTLN("I MUST ROTATE RIGHT_get_back_to_line");
				Kick(KickDirection::Right, KICK_MAGNITUDE);
			}
			else {
				DEBUG_PRINTLN("I MUST ROTATE LEFT");
				Kick(KickDirection::Left, KICK_MAGNITUDE);
			}
		}
		else {
			back_to_line = true;
		}
	}
}
//Aligns the middle sensors s2 and s3 with the line
void navigation::forwardAlignmentOnRotation() {
	bool s2s3aligned = false;
	while (s2s3aligned == false) {
		Sensor::PollSensors(Sensors);
		if (RVAL(sC::LTL) != RVAL(sC::FL)) {
			s2s3aligned = true;
			DEBUG_PRINTLN("Back On Track!");
		}
		else {
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}
//Kick back until S2 and S3 are behind the note
void navigation::backwardsToIntersection() {
	bool s2s3aligned = false;
	while (s2s3aligned == false) {
		Sensor::PollSensors(Sensors);
		if ((RVAL(sC::LTL) != RVAL(sC::FL)) && (RVAL(sC::LTL) == START_VAL(sC::LTL))) {
			s2s3aligned = true;
			DEBUG_PRINT("Need to go forward to pass the node");
		}
		else {
			if (LineCorrect() == false) {
				Kick(KickDirection::Backward, KICK_MAGNITUDE);
			}
		}
	}
}
//Kick forward until s2 and s3 have passed the intersection
void navigation::forwardsToIntersection() {
	bool s2s3aligned = false;
	while (s2s3aligned == false) {
		Sensor::PollSensors(Sensors);
		if ((RVAL(sC::LTL) != RVAL(sC::FL)) && (RVAL(sC::LTL) != START_VAL(sC::LTL))) {
			s2s3aligned = true;
			DEBUG_PRINTLN("Finished Movement");
		}
		else {
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}
//Function to align the buggy upon rotation.
void navigation::smartAlignmentRotation() {
	bool alignmentComplete = false;
	while (alignmentComplete == false) {
		Sensor::PollSensors(Sensors);
		if (reachedDestination() == true) {
			DEBUG_PRINTLN("FINISHED");
			alignmentComplete = true;
		}
		else {
			getBackToLineRotation();
			if (reachedDestination() == true) {
				return;
			}
			forwardAlignmentOnRotation();
			if (reachedDestination() == true) {
				return;
			}
			backwardsToIntersection();
			if (reachedDestination() == true) {
				return;
			}
			forwardsToIntersection();
			if (reachedDestination() == true) {
				return;
			}
		}
	}
}
//------BACKWARD ALIGNMENT---------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Function to flag sensor events during backwards alignment.
void navigation::sensorEventsB() {
	if (RVAL(sC::MR) != START_VAL(sC::MR)) {
		sensor4_event = true;
		flag4 = true;
	}
	if (RVAL(sC::ML) != START_VAL(sC::ML)) {
		sensor5_event = true;
		flag5 = true;
	}
}
//Funtion to figure out if passed the node backwards.
//If it has reached the node it will stop. If gone past it, it will kick forwards until reached.
void navigation::passedNoteB() {
	bool passed_s23 = false;
	while (passed_s23 == false) {
		//Sensor::PollSensors(Sensors, FrontM, 2);
		Sensor::PollSensors(Sensors);
		if (reachedDestination() == true) {
			return;
		}
	}
	if ((RVAL(sC::LTL) != START_VAL(sC::FR)) && (RVAL(sC::FL) != START_VAL(sC::LTR))) {
		passed_s23 = true;
	}
	else {
		DEBUG_PRINTLN("Need to kick forward on reach_nodeb");
		if (LineCorrect() == false) {
			Kick(KickDirection::Forward, KICK_MAGNITUDE);
		}

	}
}
*/
/*
void navigation::navigate(String str) {

Sensor::PollSensors(Sensors);

if (str == "F") {
sensor0_event = false;
sensor1_event = false;
flag0 = false;
flag1 = false;
start();
MoveForward();
} else if (str == "B") {
sensor4_event = false;
sensor5_event = false;
flag4 = false;
flag5 = false;
start();
MoveBackward();
} else if (str == "R") {
start();
TurnRight();
} else if (str == "L") {
start();
TurnLeft();
} 		//if (str == "D") {
where_am_i();
sensor0_event = false;
sensor1_event = false;
sensor4_event = false;
sensor5_event = false;
bool flag0 = false;
bool flag1 = false;
bool flag4 = false;
bool flag5 = false;
}
if (str == "A") {
smart_alignment();
}//
			else if (str == "G") {
				BoxApproach();
				box assessBox;
				byte error = assessBox.interrogateBox(boxConfig::boxNumber, boxConfig::boxInverted);
				if (error == 1) { //Backup and attempt re-docking unless already tried twice// }
			}
			else if (str == "S") {
				//drive(motorConfig::S, motorConfig::S);
			}
			else {
				//drive(motorConfig::S, motorConfig::S);
			}

};

void navigation::BoxApproach() {
	uint8_t Distance = 0;
	LeftSpeed = 60;
	RightSpeed = 60;
	NewPing Ultrasonic(TRIGGER, ECHO, MAXDISTANCE);
	drive(motorConfig::F, motorConfig::F, LeftSpeed, RightSpeed);
	while (true) {
		Distance = Ultrasonic.ping_cm();
		DEBUG_VPRINTLN(Distance);
		if (Distance > 3) {
			DEBUG_PRINTLN("Approaching Box");
			Sensor::PollSensors(Sensors, Sensor::Front, 2);
			if (RVAL(sC::LTR) == RVAL(sC::LTL)) {
				Sensor::DriftDirection Dir = Sensor::Drifting(Sensors, LastCorrectLeft, LastCorrectRight);
				if (Dir == Sensor::DriftDirection::DRight) {
					DEBUG_PRINTLN("Drifting Right");
					LeftSpeed -= CORRECTION_MAG;
					//RightSpeed += CORRECTION_MAG;
					drive(motorConfig::F, motorConfig::F, LeftSpeed, RightSpeed);
				}
				else if (Dir == Sensor::DriftDirection::DLeft) {
					DEBUG_PRINTLN("Drifting Left");
					//LeftSpeed += CORRECTION_MAG;
					RightSpeed -= CORRECTION_MAG;
					drive(motorConfig::F, motorConfig::F, LeftSpeed, RightSpeed);
				}
			}
		}
		else {
			DEBUG_PRINTLN("I Have Reached the box");
			drive(motorConfig::S, motorConfig::S, 0, 0);
			if (Distance == 0) {
				DEBUG_PRINTLN("Either too far or too close");
			}
			return;
		}
		delay(50);
	}
}

//captures and stores in an array all the sensor values at the initial node position
void navigation::start() {
	Sensor::PollSensors(Sensors);
#ifndef SENSOR_MEMORY_SAVE
	memcpy(starting_values, Sensor::values, NUM_SENSORS);
#else
	starting_values = Sensor::values;
#endif
	DEBUG_PRINTLN("I have the starting POSITION");
}

//Function to turn left
void navigation::TurnLeft() {
	Sensor::PollSensors(Sensors);
	drive(motorConfig::B, motorConfig::F, 70, 70);

	//digitalWrite(M1, HIGH);
	//digitalWrite(M2, LOW);
	//analogWrite(E1, 65);
	//analogWrite(E2, 65);
	while (true) {
		Sensor::PollSensors(Sensors, Sensor::Front, 2);

		//Sensor::SelectSensor(1);
		//Sensors[1].GetReading();
		//Serial.println(RVAL(sC::LTR));
		//Sensor::LogicCheck(Sensors);
		if (RVAL(sC::LTR) != RLASTVAL(sC::LTR)) {
			drive(motorConfig::S, motorConfig::S, 0, 0);

			//analogWrite(E1, 0);
			//analogWrite(E2, 0);
			return;
		}
		else {
		}
	}
}

//Function to turn right.
void navigation::TurnRight() {
	//digitalWrite(M1, LOW);
	//digitalWrite(M2, HIGH);
	//analogWrite(E1, 65);
	//analogWrite(E2, 65);
	Sensor::PollSensors(Sensors);
	drive(motorConfig::F, motorConfig::B, 70, 70);
	while (true) {
		Sensor::PollSensors(Sensors, Sensor::Front, 2);
		if (RVAL(sC::sC::FR) != START_VAL(sC::FR)) {
			drive(motorConfig::S, motorConfig::S, 0, 0);

			//analogWrite(E1, 0);
			//(E2, 0);
			return;
		}
		else {
		}
	}
}

//Function to move forwards one node
void navigation::MoveForward() {
	DEBUG_PRINTLN("Moving Now!");
	uint8_t RCnt = 0;
	uint8_t LCnt = 0;
	//int cnt = 0;
	//LeftSpeed = 100;
	//RightSpeed = 100;
	//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
	//delay(30);
	//LeftSpeed = 60;
	//RightSpeed = 60;
	//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
	while (true) {
		Sensor::PollSensors(Sensors, Sensor::FrontNMiddle, 4);
		sensorEvents();
		if ((RVAL(sC::LTL) != START_VAL(sC::LTL) && RVAL(sC::FL) != START_VAL(sC::FL))
			)
		{
			//Don't kick
			DEBUG_PRINTLN("I have stopped");
			drive(motorConfig::S, motorConfig::S, 0, 0);
			return;
		}
		else {
			if (RVAL(sC::FR) != RVAL(sC::LTR)) {
				//On line
				LastCorrectLeft = RVAL(sC::LTR);
				LastCorrectRight = RVAL(sC::FR);
				Kick(KickDirection::Forward, 200);//Kick forward but with a long kick
				RCnt = 0;
				LCnt = 0;

			}
			//LeftSpeed++;
			//RightSpeed++;
		}
		if (RVAL(sC::FR) == RVAL(sC::LTR)) {
			if (RVAL(sC::LTL) == RVAL(sC::FL)) {
				//Following wrong line case
				//LastCorrectLeft = !LastCorrectLeft;
				//LastCorrectRight = !LastCorrectRight;


			}
			if (RCnt > 20 || LCnt > 20) {
				RCnt = 0;
				LCnt = 0;
				//Kick(KickDirection::Forward, 200);//Kick forward but with a long kick
				LastCorrectLeft = !START_VAL(sC::LTR);
				LastCorrectRight = !START_VAL(sC::FR);
			}
			else {


				Sensor::DriftDirection Dir = Sensor::Drifting(Sensors, LastCorrectLeft, LastCorrectRight);

				if (Dir == Sensor::DriftDirection::DRight) {
					RCnt++;
					DEBUG_PRINTLN("Drifting Right");
					LeftSpeed -= CORRECTION_MAG;
					RightSpeed += CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed,RightSpeed);
					Kick(KickDirection::Backward, KickDirection::Forward, 200, 200);//Kick forward but with a long kick

				}
				else if (Dir == Sensor::DriftDirection::DLeft) {
					LCnt++;
					DEBUG_PRINTLN("Drifting Left");
					LeftSpeed += CORRECTION_MAG;
					RightSpeed -= CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
					Kick(KickDirection::Forward, KickDirection::Backward, 200, 200);//Kick forward but with a long kick

				}
			}
		}
		else {

		}
	}

}

void navigation::MoveBackward() {
	uint8_t RCnt = 0;
	uint8_t LCnt = 0;
	//int cnt = 0;
	//LeftSpeed = 100;
	//RightSpeed = 100;
	//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
	//delay(30);
	//LeftSpeed = 60;
	//RightSpeed = 60;
	//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
	while (true) {
		Sensor::PollSensors(Sensors, Sensor::FrontNMiddle, 4);
		sensorEventsB();
		if ((RVAL(sC::LTL) != START_VAL(sC::LTL) && RVAL(sC::FL) != START_VAL(sC::FL))
			)
		{
			//Don't kick
			DEBUG_PRINTLN("I have stopped");
			drive(motorConfig::S, motorConfig::S, 0, 0);
			return;
		}
		else {
			if (RVAL(sC::FR) != RVAL(sC::LTR)) {
				//On line
				LastCorrectLeft = RVAL(sC::LTR);
				LastCorrectRight = RVAL(sC::FR);
				Kick(KickDirection::Backward, 200);//Kick Backward but with a long kick
				RCnt = 0;
				LCnt = 0;

			}
			//LeftSpeed++;
			//RightSpeed++;
		}
		if (RVAL(sC::FR) == RVAL(sC::LTR)) {
			if (RVAL(sC::LTL) == RVAL(sC::FL)) {
				//Following wrong line case
				//LastCorrectLeft = !LastCorrectLeft;
				//LastCorrectRight = !LastCorrectRight;


			}
			if (RCnt > 20 || LCnt > 20) {
				RCnt = 0;
				LCnt = 0;
				//Kick(KickDirection::Backward, 200);//Kick Backward but with a long kick
				LastCorrectLeft = !START_VAL(sC::LTR);
				LastCorrectRight = !START_VAL(sC::FR);
			}
			else {


				Sensor::DriftDirection Dir = Sensor::Drifting(Sensors, LastCorrectLeft, LastCorrectRight);

				if (Dir == Sensor::DriftDirection::DRight) {
					RCnt++;
					DEBUG_PRINTLN("Drifting Right");
					LeftSpeed -= CORRECTION_MAG;
					RightSpeed += CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed,RightSpeed);
					Kick(KickDirection::Forward, KickDirection::Backward, 200, 200);//Kick Backward but with a long kick

				}
				else if (Dir == Sensor::DriftDirection::DLeft) {
					LCnt++;
					DEBUG_PRINTLN("Drifting Left");
					LeftSpeed += CORRECTION_MAG;
					RightSpeed -= CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed, RightSpeed);
					Kick(KickDirection::Backward, KickDirection::Forward, 200, 200);//Kick Backward but with a long kick

				}
			}
		}
		else {

		}
	}

}*/