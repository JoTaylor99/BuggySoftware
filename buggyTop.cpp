#include "buggyTop.h" //Top level file for the Buggy Code.

buggyTop::buggyTop(){};

buggyTop::~buggyTop(){};

void buggyTop::init(){
	/*Code for testing box analysis, actual structure requires team b and c discussion*/
	box assessBox;
	assessBox.init();
};

void buggyTop::go(){
	/*Code for testing box analysis, actual structure requires team b and c discussion*/
	box assessBox;
	bool boxDirection = false;
	byte boxNumber = 0;
	
	Serial.println("Which box are you approaching (Enter box number):");
	if (Serial.available()>0) {
		boxNumber = Serial.read();
	}
	Serial.println("Are you approaching from the knob side? (1 = Yes, 2 = No):");
	if (Serial.available()>0) {
		boxDirection = Serial.read();
	}

	boxValues::returnData info = assessBox.interrogateBox(boxNumber, boxDirection);
	
};