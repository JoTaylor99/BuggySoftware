#include "buggyTop.h" //Top level file for the Buggy Code.

buggyTop::buggyTop(){};

buggyTop::~buggyTop(){};

navigation buggy;

void buggyTop::init(){
	/*Code for testing box analysis, actual structure requires team b and c discussion*/
	COMPILE_DATE;
	COMPILE_TIME;
	buggy.initNavigation();

};

void buggyTop::go(){
	/* Code here is needed either here or in a not yet written communications class to separate an incomming command string from controlling PC into useful variables
	*  For the time being this is being done in the navigate function in the navigation class.
	*/

	String str;

	if (Serial.available() > 0) { // If a command has been sent
		str = Serial.readString();
		buggy.navigate(str);
	}
	/*Code for testing box analysis, actual structure requires team b and c discussion*/

		//box assessBox;
		//bool boxDirection = false;
		//byte boxNumber = 0;

		//Serial.println("Which box are you approaching (Enter box number):");
		//if (Serial.available() > 0) {
		//	boxNumber = Serial.read();
		//}
		//Serial.println("Are you approaching from the knob side? (1 = Yes, 2 = No):");
		//if (Serial.available() > 0) {
		//	boxDirection = Serial.read();
		//}

		//boxValues::returnData info = assessBox.interrogateBox(boxNumber, boxDirection);
	
};

//Handler Left
void leftStepCounter() {
	buggy.stepDistanceLeft++;
	if (buggy.stepDistanceLeft == buggy.stepTargetDistanceLeft) {
		buggy.stop(1);
	}
}

//Handler Right
void rightStepCounter(){
	buggy.stepDistanceRight++;
	if (buggy.stepDistanceRight == buggy.stepTargetDistanceLeft) {
		buggy.stop(2);
	}
}
