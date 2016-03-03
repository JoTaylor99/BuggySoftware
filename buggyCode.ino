/*
 Name:		buggyCode.ino
 Created:	2/25/2016 4:20:15 PM
 Author:	Henry
*/


#include <Wire.h>
#include <TSL2561.h>
#include <NewPing.h>
#include "Config.h"
#include "buggyTop.h"
//#include "sensorDefs.h"
#include <Adafruit_MCP23017.h>
#include <SoftwareSerial.h>



buggyTop buggy1;

// the setup function runs once when you press reset or power the board
void setup() {
	//Serial.begin(9600);
	Serial.begin(115200); //Setup serial conenction over USB
	Serial1.begin(112500); // Setup serial connection over Xbee

	buggy1.init();
}

// the loop function runs over and over again until power down or reset
void loop() {
	buggy1.go();
}
