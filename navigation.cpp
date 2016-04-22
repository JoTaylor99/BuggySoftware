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
	DEBUG_PRINTLN("Please place buggy at correct start location");
	while ((RVAL(sC::FR) != (false)) && (RVAL(sC::LTR) != (false)) && (RVAL(sC::LTL) != (true)) && (RVAL(sC::FL) != (true)) && (RVAL(sC::MR) != (false)) && (RVAL(sC::ML) != (true)) && (RVAL(sC::BR) != (true)) && (RVAL(sC::BL) != (false))) {
		Sensor::PollSensors(Sensors);
}
	DEBUG_PRINTLN("Buggy correctly placed to start");
	DEBUG_PRINTLN("Setup Complete!");
	passedBoxNumber = 4;
	passedBoxInversion = false;

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
				//boxApproach();
				////pass recieved boxnumber and recieved box inversion information
				//box boxs;
				//dockFailureCounter = 0;
				//if ((boxs.begin(passedBoxNumber, passedBoxInversion)) == false)
				//{
				//	return;
				//}
				//if (!boxs.docked()) {
				//	dockFailureCounter++;
				//	if (dockFailureCounter == 2) {
				//			//DC write variables here
				//			//Will send this ^
				//			boxBeGone();
				//			return;
				//		}
				//}
				//else {
				//	boxs.interrogateBox();
				//}
				//boxBeGone();
				box boxs;
				if ((boxs.begin(4, 0)) == true) {
					if (boxs.docked() == true) {
						NAV_PRINTLN("Docked correctly");
					}
					else {
						NAV_PRINTLN("Improperly Docked");
					}
				}
				else {
					NAV_PRINTLN("Init failure");
				}
			} else if (str == "S") {
				drive(nC::Direction::Stop);

			}
			else if (str == "H") {
				Sensor::printCurrent();
			}
			else if (str == "Z") {
				Sensor::PollSensors(Sensors);
				Sensor::printCurrent();
			}
			else if (str == "K") {
				Sensor::PollSensors(Sensors);
				step(nC::Direction::Left, 40);
				Sensor::PollSensors(Sensors);
				step(nC::Direction::Right, 80);
				Sensor::PollSensors(Sensors);
				step(nC::Direction::Left, 42);
				Sensor::PollSensors(Sensors);
				Sensor::printCurrent();

			}
			else if (str == "N") {
				moveHorizontally(nC::Direction::Left, 40);
			}
			else if (str == "M") {
				moveHorizontally(nC::Direction::Right, 40);
			}
			else {
				drive(nC::Direction::Stop);
			}
};

void navigation::boxApproach() {
	start();
	moveForward();
}

//captures and stores in an array all the sensor values at the initial node position
void navigation::start() {
#ifndef HARDCODEDSTARTVALUES
	Sensor::PollSensors(Sensors);
#ifndef SENSOR_MEMORY_SAVE
	memcpy(startingValues, Sensor::values, NUM_SENSORS);
#else
	startingValues = Sensor::values;
#endif
#else
	
	STARTVAL(sC::FL) = !STARTVAL(sC::FL);
	STARTVAL(sC::LTL) = !STARTVAL(sC::LTL);
	STARTVAL(sC::LTR) = !STARTVAL(sC::LTR);
	STARTVAL(sC::FR) = !STARTVAL(sC::FR);
	STARTVAL(sC::ML) = !STARTVAL(sC::ML);
	STARTVAL(sC::MR) = !STARTVAL(sC::MR);
	STARTVAL(sC::BL) = !STARTVAL(sC::BL);
	STARTVAL(sC::BR) = !STARTVAL(sC::BR);
#endif
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
		NAV_PRINTLN("D");
		//Deviation from the course
		if ((RVAL(sC::FL) == STARTVAL(sC::FL)) && (RVAL(sC::FR) == STARTVAL(sC::FR))) {
			//pattern of arena not flipped yet
			if (RVAL(sC::LTL) != STARTVAL(sC::LTL)) {
				drive(nC::Direction::Forward,nC::Drift::rightDrift);
			}
			else {
				drive(nC::Direction::Forward, nC::Drift::leftDrift);

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
			drive(nC::Direction::Backwards, nC::Drift::rightDrift);

			return true;
		}
		else {
			//drive backwards right 
			drive(nC::Direction::Backwards, nC::Drift::leftDrift);

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
/// Case 1 Middle Sensors have moved accidentally  behind the destination intersection line
///		if (ML== STARTVAL(ML))|| (MR == STARTVAL(MR)){
///			if (ML!=MR){
///				Both the middle sensors are behind the line
///				Drive forward
///			}
///			else{
///				if (ML==STARTVAL(ML){
///						LEFT MIDDLE SENSOR BEHIND THE INTERSECTION
///						DRIVE LEFT WHEEL FORWARD ONLY
///				}
///				else {
///					RIGHT MIDDLE SENSOR BEHIND THE INTERSECTION
///					DRIVE RIGHT WHEEL FORWAR ONLY
///				}
///			}
///		}	
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
	// Case 1 Middle Sensors have moved accidentally  behind the destination intersection line
	if ((((RVAL(sC::LTL)) == (RVAL(sC::LTR))) == (RVAL(sC::FR))) && ((RVAL(sC::BR)) == (RVAL(sC::BL)))) {
		//case for moving horizontally left
		NAV_PRINTLN("HL");
		moveHorizontally(nC::Direction::Left, 40);
	}
	else if ((((RVAL(sC::LTL)) == (RVAL(sC::LTR))) == (RVAL(sC::FL))) && ((RVAL(sC::BR)) == (RVAL(sC::BL)))) {
		//case for moving horizontally right
		NAV_PRINTLN("HR");
		moveHorizontally(nC::Direction::Right, 40);
	}
	else if ((RVAL(sC::ML) == STARTVAL(sC::ML)) || (RVAL(sC::MR) == STARTVAL(sC::MR))) {
		
		if (RVAL(sC::ML)!= RVAL(sC::MR)){
			NAV_PRINTLN("MW(B)");
			drive(nC::Direction::Forward);
		}
		else{
			if (RVAL(sC::ML) == STARTVAL(sC::ML)) {
				NAV_PRINTLN("MW(L)");
			//	LEFT MIDDLE SENSOR BEHIND THE INTERSECTION
				drive(nC::Direction::LeftForwardOnly);
			}
			else {
				NAV_PRINTLN("MW(R)");
				//	RIGHT MIDDLE SENSOR BEHIND THE INTERSECTION
				drive(nC::Direction::RightForwardOnly);
			}
		}
	}

	//// Case 2 ((LTL == LTR && LTL==BL && LTL ==BR)
	//else if ((RVAL(sC::LTL) == RVAL(sC::LTR)) && (RVAL(sC::LTL) == RVAL(sC::BR)) && (RVAL(sC::LTL) == RVAL(sC::BL))) {
	//	if (RVAL(sC::LTL) == STARTVAL(sC::LTL)) {
	//		drive(nC::Direction::Left);
	//	}
	//	else {
	//		drive(nC::Direction::Right);
	//	}
	//}
	//Case 3 (LTL!= LTR) && (BR==BL)   
	else if ((RVAL(sC::LTL) != RVAL(sC::LTR)) && (RVAL(sC::BL) == RVAL(sC::BR))) {
		NAV_PRINTLN("FC, BW");
		if (RVAL(sC::BL) == STARTVAL(sC::BL)) {
			//LEFT WHEEL FORWARD, RIGHT WHEEL STOP
			drive(nC::Direction::LeftForwardOnly);
		}
		else {
			//LEFT WHEEL STOP, RIGHT WHEEL FORWARD
			drive(nC::Direction::RightForwardOnly);
		}
	}
	//Case 3.1 (LTL==LTR) && (BR !=BL)
	else if ((RVAL(sC::LTL) == RVAL(sC::LTR)) && (RVAL(sC::BL) != RVAL(sC::BR))) {
		NAV_PRINTLN("FW, BC");
		if (RVAL(sC::LTL) != STARTVAL(sC::LTL)) {
			//LEFT WHEEL STOP, RIGHT WHEEL BACKWARD
			drive(nC::Direction::RightBackwardsOnly);
		}
		else {
			//LEFT WHEEL BACKWARD, RIGHT WHEEL STOP
			drive(nC::Direction::LeftBackwardsOnly);
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
/// <summary>
/// Turn Left Algorithm
/// While (true){
///		if ( LTL == STARTVAL(LTL)) {
///			ROTATE LEFT --Actual Rotation movement
///		}
///		else{
///			stop;
///			break;
///		}
/// }
/// while (true){
///		if finished movement
///		{stop;
///		 break;
///		 }
///		else {
///			adjustOntheSpot
///		}
/// }
/// </summary>
void navigation::turnLeft() {
	Sensor::PollSensors(Sensors);
	Sensor::printCurrent();
	while (true) {
		Sensor::PollSensors(Sensors);
		if (((RVAL(sC::LTL))!= (STARTVAL(sC::LTL))) && ((RVAL(sC::BR)) != (STARTVAL(sC::BR)))) {
			drive(nC::Direction::Stop);
			//step(nC::Direction::Forward, 8);
			break;
		}
		else {
			drive(nC::Direction::Left);
		}
	}
	Sensor::PollSensors(Sensors);
	while (true) {
		Sensor::PollSensors(Sensors);
		if ((RVAL(sC::FL) != STARTVAL(sC::FL)) &&
			(RVAL(sC::LTL) != STARTVAL(sC::LTL)) &&
			(RVAL(sC::LTR) != STARTVAL(sC::LTR)) &&
			(RVAL(sC::FR) != STARTVAL(sC::FR)) &&
			(RVAL(sC::ML) != STARTVAL(sC::ML)) &&
			(RVAL(sC::MR) != STARTVAL(sC::MR)) &&
			(RVAL(sC::BL) != STARTVAL(sC::BL)) &&
			(RVAL(sC::BR) != STARTVAL(sC::BR))) {
			NAV_PRINTLN("Success");
			drive(nC::Direction::Stop);
			delay(14);
			if ((((RVAL(sC::FL)) && (RLASTVAL(sC::FL))) != STARTVAL(sC::FL)) &&
				(((RVAL(sC::LTL)) && (RLASTVAL(sC::LTL))) != STARTVAL(sC::LTL)) &&
				(((RVAL(sC::LTR)) && (RLASTVAL(sC::LTR))) != STARTVAL(sC::LTR)) &&
				(((RVAL(sC::FR)) && (RLASTVAL(sC::FR))) != STARTVAL(sC::FR)) &&
				(((RVAL(sC::ML)) && (RLASTVAL(sC::ML))) != STARTVAL(sC::ML)) &&
				(((RVAL(sC::MR)) && (RLASTVAL(sC::MR))) != STARTVAL(sC::MR)) &&
				(((RVAL(sC::BL)) && (RLASTVAL(sC::BL))) != STARTVAL(sC::BL)) &&
				(((RVAL(sC::BR)) && (RLASTVAL(sC::BR))) != STARTVAL(sC::BR))) {
				break;
			}
		}
		else {
			adjustOnTheSpot();
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
	Sensor::PollSensors(Sensors);
	Sensor::printCurrent();
	while (true) {
		Sensor::PollSensors(Sensors);
		if (((RVAL(sC::LTR)) != (STARTVAL(sC::LTR))) && ((RVAL(sC::BL)) != (STARTVAL(sC::BL)))) {
			drive(nC::Direction::Stop);
			//step(nC::Direction::Forward, 8);
			break;
		}
		else {
			drive(nC::Direction::Right);
		}
	}
	Sensor::PollSensors(Sensors);
	while (true) {
		Sensor::PollSensors(Sensors);
		if ((RVAL(sC::FL) != STARTVAL(sC::FL)) &&
			(RVAL(sC::LTL) != STARTVAL(sC::LTL)) &&
			(RVAL(sC::LTR) != STARTVAL(sC::LTR)) &&
			(RVAL(sC::FR) != STARTVAL(sC::FR)) &&
			(RVAL(sC::ML) != STARTVAL(sC::ML)) &&
			(RVAL(sC::MR) != STARTVAL(sC::MR)) &&
			(RVAL(sC::BL) != STARTVAL(sC::BL)) &&
			(RVAL(sC::BR) != STARTVAL(sC::BR))) {
			NAV_PRINTLN("Success");
			drive(nC::Direction::Stop);
			delay(14);
			if ((((RVAL(sC::FL)) && (RLASTVAL(sC::FL))) != STARTVAL(sC::FL)) &&
				(((RVAL(sC::LTL)) && (RLASTVAL(sC::LTL))) != STARTVAL(sC::LTL)) &&
				(((RVAL(sC::LTR)) && (RLASTVAL(sC::LTR))) != STARTVAL(sC::LTR)) &&
				(((RVAL(sC::FR)) && (RLASTVAL(sC::FR))) != STARTVAL(sC::FR)) &&
				(((RVAL(sC::ML)) && (RLASTVAL(sC::ML))) != STARTVAL(sC::ML)) &&
				(((RVAL(sC::MR)) && (RLASTVAL(sC::MR))) != STARTVAL(sC::MR)) &&
				(((RVAL(sC::BL)) && (RLASTVAL(sC::BL))) != STARTVAL(sC::BL)) &&
				(((RVAL(sC::BR)) && (RLASTVAL(sC::BR))) != STARTVAL(sC::BR))) {
				break;
			}
		}
		else {
			adjustOnTheSpot();
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
	while (true) {
		Sensor::PollSensors(Sensors);
		//m1 = micros()
		if (navigation::reachedDestination()) {
			NAV_PRINTLN("Destination reached");

			drive(nC::Direction::Stop);
			break;
		}
		//else if ((RVAL(sC::ML) != RVAL(sC::MR)) && ((RVAL(sC::ML) != STARTVAL(sC::ML)))) {
			//NAV_PRINTLN("Adjust on Spot");	
		//	drive(nC::Direction::Stop);
			//break;
			//adjustOnTheSpot();
		//}
		else {
			if (!navigation::driftingWhenForward()) {
				drive(nC::Direction::Forward);
			}
		}
	}
	Sensor::PollSensors(Sensors);
	while (true) {
		Sensor::PollSensors(Sensors);
		if (compareAllWithStart()) {
			 NAV_PRINTLN("Success");
			 drive(nC::Direction::Stop);
			 delay(14);
			 if (compareAllToLast()) {
				 NAV_PRINTLN("DONE");
				 break;
			 }
		}
		else {
			adjustOnTheSpot();
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
		Sensor::PollSensors(Sensors);
		while (true) {
			Sensor::PollSensors(Sensors);
			if (compareAllWithStart()) {
				NAV_PRINTLN("Success");
				drive(nC::Direction::Stop);
				delay(14);
				if (compareAllToLast()) {
					break;
				}
			}
			else {
				adjustOnTheSpot();
			}
		}
}

#ifdef SENSOR_MEMORY_SAVE
bool navigation::startValIs(sC::sensorNumber position) {
	return bitRead(startingValues, position);
}
#endif

/// <summary>
/// Victory roll defined as:
/// 6 x turnLeft (or Right)
/// 1 x moveForward
/// 2 x turnLeft (or Right)
/// 1 x moveForward
/// </summary>
void navigation::victoryRoll() {
	for (int x = 0; x < 6; x++) {
		start();
		turnLeft();
	}
	start();
	moveForward();
	for (int x = 0; x < 2; x++) {
		start();
		turnLeft();
	}
	start();
	moveForward();
}

bool navigation::compareAllWithStart() {
	if ((RVAL(sC::FL) != STARTVAL(sC::FL)) &&
		(RVAL(sC::LTL) != STARTVAL(sC::LTL)) &&
		(RVAL(sC::LTR) != STARTVAL(sC::LTR)) &&
		(RVAL(sC::FR) != STARTVAL(sC::FR)) &&
		(RVAL(sC::ML) != STARTVAL(sC::ML)) &&
		(RVAL(sC::MR) != STARTVAL(sC::MR)) &&
		(RVAL(sC::BL) != STARTVAL(sC::BL)) &&
		(RVAL(sC::BR) != STARTVAL(sC::BR))) {
		return true;
	}
	else { return false; }
}

bool navigation::compareAllToLast() {
	if (((RVAL(sC::FL)) == (RLASTVAL(sC::FL))) &&
		((RVAL(sC::LTL)) == (RLASTVAL(sC::LTL))) &&
		((RVAL(sC::LTR)) == (RLASTVAL(sC::LTR))) &&
		((RVAL(sC::FR)) == (RLASTVAL(sC::FR))) &&
		((RVAL(sC::ML)) == (RLASTVAL(sC::ML))) &&
		((RVAL(sC::MR)) == (RLASTVAL(sC::MR))) &&
		((RVAL(sC::BL)) == (RLASTVAL(sC::BL))) &&
		((RVAL(sC::BR)) == (RLASTVAL(sC::BR)))) {
		return true;
	}
	else { return false; }
}

void navigation::boxBeGone() {
	start();
	moveBackward();
	}