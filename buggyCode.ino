/*
 Name:		buggyCode.ino
 Created:	2/25/2016 4:20:15 PM
 Author:	Henry
*/




#include <DFRobot_utility.h>
#include <utility/SerialProtocol.h>
#include <arduino2.h>
#include <PWM.h>
#include <NewPing.h>
#include <Wire.h>
#include "Config.h"
#include <Adafruit_MCP23017.h>
#include <SparkFunTSL2561.h>
#include "buggyTop.h"

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
	//delay(10);
	while (Serial.available() > 0) {
		while (Serial.available() < 8) {
			//Wait to receive a full frame
		}
		buggy1.Check();
		for (int i = 0; i < 8; i++)
		{
			Serial.read();
		}
	}

}