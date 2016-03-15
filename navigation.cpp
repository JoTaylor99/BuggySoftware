// 
// 
// 

#include "navigation.h"





navigation::navigation() {
	
}

navigation::~navigation() {
}

void navigation::initNavigation() {
	Serial.begin(115200); //Setup serial conenction over USB
	//Serial.begin(112500); // Setup serial connection over Xbee
	//-------Setup Pins used for sensor select as outputs---------------
	Sensor::initSensors();
	initMotion();
	
	Serial.print("Setup Complete!");

	//if (SMode == USB) {
	//	Serial.print("Setup Complete!");
	//}
	//else {
	//	Serial.print("Setup Complete!");
	//}
};

void navigation::navigate() {
	/*
	for (int i = 0; i < 6; i++) {
	Sensor::SelectSensor(i);
	values[i] = Sensors[i].GetReading();
	//Serial.println(Sensors[i].Normalised);
	}*/
	//int DefaultOrder[6] = { 1,2,3,4,5,6 };
	Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);

	//printbw(values);
	//sensor_events();
	//Serial.println();

	//Serial.println("---------------------");
	//delay(50);

	if (Serial.available() > 0) { // If a command has been sent
		String str = Serial.readString();
		if (str == "F") {
			sensor0_event = false;
			sensor1_event = false;
			flag0 = false;
			flag1 = false;
			start();
			MoveForward();
			smartAlignmentForward();
		}
		if (str == "B") {
			sensor4_event = false;
			sensor5_event = false;
			flag4 = false;
			flag5 = false;
			start();
			MoveBackward();
			SmartAlignmentB();
		}
		if (str == "R") {
			start();
			//RotateR();
			TurnRight();
			smartAlignmentRotation();
		}
		if (str == "L") {
			start();
			//RotateL();
			TurnLeft();
			smartAlignmentRotation();
		}
		if (str == "S") {

		}

		/*if (str == "D") {
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
		}*/
		if (str == "G") {
			BoxApproach();
		}
	}
};

void navigation::BoxApproach() {
	int Distance = 0;
	LeftSpeed = 60;
	RightSpeed = 60;
	NewPing Ultrasonic(TRIGGER, ECHO, MAXDISTANCE);
	drive(motorConfig::F, motorConfig::F, LeftSpeed, RightSpeed);
	while (true) {
		Distance = Ultrasonic.ping_cm();
		Serial.println(Distance);
		if (Distance > 3) {
			Serial.println("Approaching Box");
			Sensor::PollSensors(Sensors, Sensor::Front, 2);
			if (Sensors[0].Boolian == Sensors[1].Boolian) {
				Sensor::DriftDirection Dir = Sensor::Drifting(Sensors, LastCorrectLeft, LastCorrectRight);
				if (Dir == Sensor::DriftDirection::DRight) {
					Serial.println("Drifting Right");
					LeftSpeed -= CORRECTION_MAG;
					//RightSpeed += CORRECTION_MAG;
					drive(motorConfig::F, motorConfig::F, LeftSpeed, RightSpeed);
				}
				else if (Dir == Sensor::DriftDirection::DLeft) {
					Serial.println("Drifting Left");
					//LeftSpeed += CORRECTION_MAG;
					RightSpeed -= CORRECTION_MAG;
					drive(motorConfig::F, motorConfig::F, LeftSpeed, RightSpeed);
				}
			}
		}
		else {
			Serial.println("I Have Reached the box");
			drive(motorConfig::S, motorConfig::S, 0, 0);
			if (Distance == 0) {
				Serial.println("Either too far or too close");
			}
			return;
		}
		delay(50);
	}
}

//Bool function to indicate if the destination intersection has reached. True if the middle sensors are in the intersection or if perfect intersection achieved

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
		LastCorrectLeft = Sensors[1].Boolian;
		LastCorrectRight = Sensors[0].Boolian;
		return false;
	}
	else {
		Counter = 0;
		return false;
	}
}
//Function to define whether or not the destination node has been reached.
//If so it returns true and prints "Destination Reached" to the terminal, if not returns false.
//Based on the quadrant the buggy rotates in a backward motion to get back to the motion line with sesnors 4 and 5
bool navigation::reachedDestination() {
	if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) && (Sensors[2].Boolian != starting_intersection[2]) &&
		(Sensors[3].Boolian != starting_intersection[3]) && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
		Serial.println("Destination Reached");
		return true;
	}
	//  else if ((Sensors[2].Boolian != starting_intersection[2]) && (Sensors[3].Boolian != starting_intersection[3])
	//           && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
	//    Serial.println("Destination Reached2");
	//    return true;
	//  }
	else {
		return false;
	}
}

//captures and stores in an array all the sensor values at the initial node position
void navigation::start() {
	Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
	for (int n = 0; n < 6; n++) {
		starting_intersection[n] = Sensor::values[n];
	}
	Serial.println("I have the starting POSITION");
}

//----------------FORWARD ALIGNMENT----------------------------------------------------------------
//Checks if there was an event in the values of the first two sensors during the forward motion
void navigation::sensorEvents() {


	if (Sensors[0].Boolian != starting_intersection[0]) {
		sensor0_event = true;
		flag0 = true;
	}
	if (Sensors[1].Boolian != starting_intersection[1]) {
		sensor1_event = true;
		flag1 = true;
	}
}

//Checks if the buggy has passed the line of the destination intersection
void navigation::didIPassIntersectionLine() {
	//ideal-case Both sensors have passed the intersection line normally
	if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) || ((sensor0_event == true) && (sensor1_event == true)))
	{
		passed_intersection_line = true;
		Serial.println("Yes I passed the intersection line");
	}
	else {
		passed_intersection_line = false;
		Serial.println("No I havent passed intersection line");
	}
}


//Quadrants Description
//Forward  S0.S2 are in Q4, S1 and S3 are inQ1              //Backwards
//S4 Q3 and S5 Q2                            S4 at Q4, S5 at Q1, S3S1 at Q2, S2S0 at Q3
/* Destination Note                            Destination Note
Q2|Q1                                            Q1|Q2
----|-----     Q0 is the on line case              --|--
Q3|Q4                                            Q4 Q3
*/
// Function to identify the quadrant where the buggy is at the moment
void navigation::whereAmI() {
	if (Sensors[0].Boolian == Sensors[1].Boolian) {
		if (passed_intersection_line == true) {
			if (Sensors[0].Boolian != starting_intersection[0]) {
				Serial.println("I am at Q4");
				quadrant = 4;
			}
			else {
				Serial.println(" I am at Q1");
				quadrant = 1;
			}
		}
		else {
			if (Sensors[0].Boolian != starting_intersection[0]) {
				Serial.println("I am at Q2");
				quadrant = 2;
			}
			else {
				Serial.println("I am at Q3");
				quadrant = 3;
			}
		}
	}
	else {
		quadrant = 0; //S0 and S1 on line
	}
}


void navigation::findLineS23() {
	bool smart_line_s45 = false;
	switch (quadrant) {
	case 1: while (smart_line_s45 == false) {
				//Sensor::PollSensors(Sensors, Back, 2);
				Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
				if (Sensors[2].Boolian != (Sensors[3].Boolian)) {
					smart_line_s45 = true;
				}
				else {
					Serial.println("Q1 NEED TO KICK BACKWARDS RIGHT");
					Kick(KickDirection::Backward, KickDirection::Forward, 255, 0);
				}
	}
			break;
	case 2: while (smart_line_s45 == false) {
				// Sensor::PollSensors(Sensors, Back, 2);
				Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
				if (Sensors[2].Boolian != (Sensors[3].Boolian)) {
					smart_line_s45 = true;
				}
				else {
					Serial.println("Q2 NEED TO KICK BACKWARDS RIGHT");
					Kick(KickDirection::Backward, KickDirection::Forward, KICK_MAGNITUDE, 0);

				}
	}
			break;
	case 3:  while (smart_line_s45 == false) {
				 // Sensor::PollSensors(Sensors, Back, 2);
				 Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
				 if (Sensors[2].Boolian != (Sensors[3].Boolian)) {
					 smart_line_s45 = true;
				 }
				 else {
					 Serial.println("Q3 NEED TO KICK BACKWARDS LEFT");
					 Kick(KickDirection::Forward, KickDirection::Backward, 0, KICK_MAGNITUDE);

				 }
	}
			 break;
	case 4:  while (smart_line_s45 == false) {
				 // Sensor::PollSensors(Sensors, Back, 2);
				 Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
				 if (Sensors[2].Boolian != (Sensors[3].Boolian)) {
					 smart_line_s45 = true;
				 }
				 else {
					 Serial.println("Q4 NEED TO KICK BACKWARDS LEFT");
					 Kick(KickDirection::Forward, KickDirection::Backward, 0, KICK_MAGNITUDE);

				 }
	}
			 break;
	default:
		break;
	}
}



// Based on the quadrant the buggy rotates left or right to get back to the motion line with sesnors 1 and 2
void navigation::findLineS01() {
	bool smart_line_s01 = false;
	switch (quadrant) {
	case 1: while (smart_line_s01 == false) {
				//   Sensor::PollSensors(Sensors, Front, 2);
				Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
				if (Sensors[0].Boolian != (Sensors[1].Boolian)) {
					smart_line_s01 = true;
				}
				else {
					Serial.println("Q1 NEED TO ROTATE RIGHT");
					Kick(KickDirection::Right, KICK_MAGNITUDE);

				}
	}
			break;
	case 2: while (smart_line_s01 == false) {
				//Sensor::PollSensors(Sensors, Front, 2);
				Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
				if (Sensors[0].Boolian != (Sensors[1].Boolian)) {
					smart_line_s01 = true;
				}
				else {
					Serial.println("Q2 NEED TO ROTATE RIGHT");
					Kick(KickDirection::Right, KICK_MAGNITUDE);

				}
	}
			break;
	case 3: while (smart_line_s01 == false) {
				//Sensor::PollSensors(Sensors, Front, 2);
				Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
				if (Sensors[0].Boolian != (Sensors[1].Boolian)) {
					smart_line_s01 = true;
				}
				else {
					Serial.println("Q3 NEED TO ROTATE LEFT");
					Kick(KickDirection::Left, KICK_MAGNITUDE);

				}
	}
			break;
	case 4: while (smart_line_s01 == false) {
				//  Sensor::PollSensors(Sensors, Front, 2);
				Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
				if (Sensors[0].Boolian != (Sensors[1].Boolian)) {
					smart_line_s01 = true;
				}
				else {
					Serial.println("Q4 NEED TO ROTATE LEFT");
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
		if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1])) {
			passed_s01 = true;
		}
		else {
			Serial.println("Need to kick forward");
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}


void navigation::reachNote() {
	bool passed_s23 = false;
	while (passed_s23 == false) {
		//Sensor::PollSensors(Sensors, FrontM, 2);
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if (reachedDestination() == true) {
			return;
		}
		if ((Sensors[2].Boolian != starting_intersection[0]) && (Sensors[3].Boolian != starting_intersection[1])) {
			passed_s23 = true;
		}
		else {
			Serial.println("Need to kick forward on reach_node");
			if (LineCorrect() == false) {
				Kick(KickDirection::Forward, KICK_MAGNITUDE);
			}
		}
	}
}

//Smart Alignment Function for Forward movement.
void navigation::smartAlignmentForward() {
	bool perfect_intersection = false;
	whereAmI();
	while (perfect_intersection == false) {
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) && (Sensors[2].Boolian != starting_intersection[2]) &&
			(Sensors[3].Boolian != starting_intersection[3]) && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
			perfect_intersection = true;
			Serial.println(" FINISHED");
		}
		else {
			findLineS23();
			findLineS01();
			// on_line_all();
			if (reachedDestination() == true) {
				return;
			}
			passedNote();
			if (reachedDestination() == true) {
				return;
			}
			//reached_destination()
			if (reachedDestination() == true) {
				return;
			}
			reachNote();
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
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if (Sensors[0].Boolian == Sensors[1].Boolian) {
			if ((Sensors[0].Boolian == starting_intersection[0])) {
				Serial.println("I MUST ROTATE RIGHT_get_back_to_line");
				Kick(KickDirection::Right, KICK_MAGNITUDE);
			}
			else {
				Serial.println("I MUST ROTATE LEFT");
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
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if (Sensors[2].Boolian != Sensors[3].Boolian) {
			s2s3aligned = true;
			Serial.println("Back On Track!");
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
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if ((Sensors[2].Boolian != Sensors[3].Boolian) && (Sensors[2].Boolian == starting_intersection[2])) {
			s2s3aligned = true;
			Serial.print("Need to go forward to pass the node");
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
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if ((Sensors[2].Boolian != Sensors[3].Boolian) && (Sensors[2].Boolian != starting_intersection[2])) {
			s2s3aligned = true;
			Serial.println("Finished Movement");
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
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if (reachedDestination() == true) {
			Serial.println("FINISHED");
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
	if (Sensors[4].Boolian != starting_intersection[4]) {
		sensor4_event = true;
		flag4 = true;
	}
	if (Sensors[5].Boolian != starting_intersection[5]) {
		sensor5_event = true;
		flag5 = true;
	}
}

//Function to define whether or not the buggy has passed an interesction backwards.
//It currently does a serial print to define whether or not it has.
//May be better to change this function to a bool type and then print to the terminal if the return is true/false.
//Also the the variable passed_intersection_lineb may then not need to be alive for as long?
void navigation::didIPassIntersectionLineB() {
	Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
	//ideal-case Both sensors have passed the intersection line normally
	if ((Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5]) || ((sensor4_event == true) && (sensor5_event == true)))
	{
		passed_intersection_lineb = true;
		Serial.println("Yes I passed the intersection line");
	}
	else {
		passed_intersection_lineb = false;
		Serial.println("No I havent passed intersection line");
	}
}

void navigation::whereAmIB() {
	didIPassIntersectionLineB();
	if (Sensors[4].Boolian == Sensors[5].Boolian) {
		if (passed_intersection_lineb == true) {
			if (Sensors[4].Boolian != starting_intersection[4]) {
				Serial.println("I am at Q4");
				quadrant = 4;
			}
			else {
				Serial.println(" I am at Q1");
				quadrant = 1;
			}

		}
		else {
			if (Sensors[4].Boolian != starting_intersection[4]) {
				Serial.println("I am at Q2");
				quadrant = 2;
			}
			else {
				Serial.println("I am at Q3");
				quadrant = 3;
			}
		}
	}
	else {
		quadrant = 0; //S4 and S5 on line
	}
}


//Identifies whether sensors 4 and 5 have passed the node in the backward motion
void navigation::reachedNoteB() {
	bool passed_s45 = false;
	while (passed_s45 == false) {
		//Sensor::PollSensors(Sensors, Front, 2);
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if (reachedDestination() == true) {
			return;
		}
		if ((Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
			passed_s45 = true;
		}
		else {
			Serial.println("Need to kick steadily backward");
			if (LineCorrect() == false) {
				Kick(KickDirection::Backward, KICK_MAGNITUDE);
			}

		}
	}
}

//Funtion to figure out if passed the node backwards.
//If it has reached the node it will stop. If gone past it, it will kick forwards until reached.
void navigation::passedNoteB() {
	bool passed_s23 = false;
	while (passed_s23 == false) {
		//Sensor::PollSensors(Sensors, FrontM, 2);
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if (reachedDestination() == true) {
			return;
		}
	}
	if ((Sensors[2].Boolian != starting_intersection[0]) && (Sensors[3].Boolian != starting_intersection[1])) {
		passed_s23 = true;
	}
	else {
		Serial.println("Need to kick forward on reach_nodeb");
		if (LineCorrect() == false) {
			Kick(KickDirection::Forward, KICK_MAGNITUDE);
		}

	}
}


//Function to run the smart alignment when traveling backwards.
void navigation::SmartAlignmentB() {
	bool perfect_intersection = false;
	whereAmIB();
	while (perfect_intersection == false) {
		Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
		if ((Sensors[0].Boolian != starting_intersection[0]) && (Sensors[1].Boolian != starting_intersection[1]) && (Sensors[2].Boolian != starting_intersection[2]) &&
			(Sensors[3].Boolian != starting_intersection[3]) && (Sensors[4].Boolian != starting_intersection[4]) && (Sensors[5].Boolian != starting_intersection[5])) {
			perfect_intersection = true;
			Serial.println(" FINISHED");
		}
		else {
			findLineS23();
			if (reachedDestination() == true) {
				return;
			}
			findLineS01();
			if (reachedDestination() == true) {
				return;
			}
			reachedNoteB();
			if (reachedDestination() == true) {
				return;
			}
			passedNoteB();
			if (reachedDestination() == true) {
				return;
			}
		}
	}
}

//Function to turn left
void navigation::TurnLeft() {
	Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
	drive(motorConfig::B, motorConfig::F, 70, 70);

	//digitalWrite(M1, HIGH);
	//digitalWrite(M2, LOW);
	//analogWrite(E1, 65);
	//analogWrite(E2, 65);
	while (true) {
		Sensor::PollSensors(Sensors, Sensor::Front, 2);

		//Sensor::SelectSensor(1);
		//Sensors[1].GetReading();
		//Serial.println(Sensors[1].Boolian);
		//Sensor::LogicCheck(Sensors);
		if (Sensors[1].Boolian != Sensors[1].PreviousBool) {
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
	Sensor::PollSensors(Sensors, Sensor::DefaultOrder, 6);
	drive(motorConfig::F, motorConfig::B, 70, 70);
	while (true) {
		Sensor::PollSensors(Sensors, Sensor::Front, 2);
		if (Sensors[0].Boolian != starting_intersection[0]) {
			drive(motorConfig::S, motorConfig::S, 0, 0);

			//analogWrite(E1, 0);
			//analogWrite(E2, 0);
			return;
		}
		else {
		}
	}
}

//Function to move forwards one node
void navigation::MoveForward() {
	Serial.println("Moving Now!");
	int RCnt = 0;
	int LCnt = 0;
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
		if ((Sensors[2].Boolian != starting_intersection[2] && Sensors[3].Boolian != starting_intersection[3])
			)
		{
			//Don't kick
			Serial.println("I have stopped");
			drive(motorConfig::S, motorConfig::S, 0, 0);
			return;
		}
		else {
			if (Sensors[0].Boolian != Sensors[1].Boolian) {
				//On line
				LastCorrectLeft = Sensors[1].Boolian;
				LastCorrectRight = Sensors[0].Boolian;
				Kick(KickDirection::Forward, 200);//Kick forward but with a long kick
				RCnt = 0;
				LCnt = 0;

			}
			//LeftSpeed++;
			//RightSpeed++;
		}
		if (Sensors[0].Boolian == Sensors[1].Boolian) {
			if (Sensors[2].Boolian == Sensors[3].Boolian) {
				//Following wrong line case
				//LastCorrectLeft = !LastCorrectLeft;
				//LastCorrectRight = !LastCorrectRight;


			}
			if (RCnt > 20 || LCnt > 20) {
				RCnt = 0;
				LCnt = 0;
				//Kick(KickDirection::Forward, 200);//Kick forward but with a long kick
				LastCorrectLeft = !starting_intersection[1];
				LastCorrectRight = !starting_intersection[0];
			}
			else {


				Sensor::DriftDirection Dir = Sensor::Drifting(Sensors, LastCorrectLeft, LastCorrectRight);

				if (Dir == Sensor::DriftDirection::DRight) {
					RCnt++;
					Serial.println("Drifting Right");
					LeftSpeed -= CORRECTION_MAG;
					RightSpeed += CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed,RightSpeed);
					Kick(KickDirection::Backward, KickDirection::Forward, 200, 200);//Kick forward but with a long kick

				}
				else if (Dir == Sensor::DriftDirection::DLeft) {
					LCnt++;
					Serial.println("Drifting Left");
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
	int RCnt = 0;
	int LCnt = 0;
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
		if ((Sensors[2].Boolian != starting_intersection[2] && Sensors[3].Boolian != starting_intersection[3])
			)
		{
			//Don't kick
			Serial.println("I have stopped");
			drive(motorConfig::S, motorConfig::S, 0, 0);
			return;
		}
		else {
			if (Sensors[0].Boolian != Sensors[1].Boolian) {
				//On line
				LastCorrectLeft = Sensors[1].Boolian;
				LastCorrectRight = Sensors[0].Boolian;
				Kick(KickDirection::Backward, 200);//Kick Backward but with a long kick
				RCnt = 0;
				LCnt = 0;

			}
			//LeftSpeed++;
			//RightSpeed++;
		}
		if (Sensors[0].Boolian == Sensors[1].Boolian) {
			if (Sensors[2].Boolian == Sensors[3].Boolian) {
				//Following wrong line case
				//LastCorrectLeft = !LastCorrectLeft;
				//LastCorrectRight = !LastCorrectRight;


			}
			if (RCnt > 20 || LCnt > 20) {
				RCnt = 0;
				LCnt = 0;
				//Kick(KickDirection::Backward, 200);//Kick Backward but with a long kick
				LastCorrectLeft = !starting_intersection[1];
				LastCorrectRight = !starting_intersection[0];
			}
			else {


				Sensor::DriftDirection Dir = Sensor::Drifting(Sensors, LastCorrectLeft, LastCorrectRight);

				if (Dir == Sensor::DriftDirection::DRight) {
					RCnt++;
					Serial.println("Drifting Right");
					LeftSpeed -= CORRECTION_MAG;
					RightSpeed += CORRECTION_MAG;
					//MotorControl(Direction::F, Direction::F, LeftSpeed,RightSpeed);
					Kick(KickDirection::Forward, KickDirection::Backward, 200, 200);//Kick Backward but with a long kick

				}
				else if (Dir == Sensor::DriftDirection::DLeft) {
					LCnt++;
					Serial.println("Drifting Left");
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

}
