/*
 Name:		buggyCode.ino
 Created:	2/25/2016 4:20:15 PM
 Author:	Henry
*/

#include <NewPing.h>
#include <Wire.h>
#include "Config.h"
#include <SparkFunTSL2561.h>
#include "buggyTop.h"
#include <Adafruit_MCP23017.h>
#include <DFRobot_utility.h>
#include <utility/SerialProtocol.h>
//#ifdef QTRSINUSE
//#include <QTRSensors.h>
//#endif

buggyTop buggy1;

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	Wire.begin();
	Wire.setClock(400000L);
	buggy1.init();
}

// the loop function runs over and over again until power down or reset
void loop() {




	buggy1.go();
}

void serialEvent() {
	buggy1.Check();
}