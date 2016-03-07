/*
 Name:		buggyCode.ino
 Created:	2/25/2016 4:20:15 PM
 Author:	Henry
*/

#include "buggyTop.h"


buggyTop buggy1;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	buggy1.init();

}

// the loop function runs over and over again until power down or reset
void loop() {
	buggy1.go();
}
