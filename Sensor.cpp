// 
// 
// 

#include "Sensor.h"

#ifdef QTRSINUSE
static QTRSensorsRC rc[2];
#endif

SFE_TSL2561 tsl;
Adafruit_MCP23017 sensGPIO;


const sC::sensorNumber Sensor::DefaultOrder[8] = { sC::FR, sC::LTR, sC::LTL, sC::FL, sC::MR, sC::ML, sC::BR, sC::BL };
const sC::sensorNumber Sensor::Front[4] = { sC::FR, sC::LTR, sC::LTL, sC::FL };
const sC::sensorNumber Sensor::Back[2] = { sC::BR, sC::BL };
const sC::sensorNumber Sensor::Middle[2] = { sC::MR, sC::ML };
const sC::sensorNumber Sensor::FrontNMiddle[6] = { sC::FR, sC::LTR, sC::LTL, sC::FL, sC::MR, sC::ML };

Sensor::Sensor(uint8_t Pin) {
	_pin = Pin;
	Max = MAX_DEFAULT;
	Min = MIN_DEFAULT;
	Normalised = 0;
	tileWhite = false;
	_s = sC::SensorType::TSL;
}; 

Sensor::Sensor(uint8_t index, sC::SensorType S) {
	_pin = index;
	Max = MAX_DEFAULT;
	Min = MIN_DEFAULT;
	Normalised = 0;
	tileWhite = false;
	_s = S;
};

Sensor::~Sensor(){};

bool Sensor::_sensorInitComplete = false;

//Reads the Raw value from whichever sensor is enabled
void Sensor::ReadRaw() {
	if(_s == sC::SensorType::TSL) {
		Raw = 0;
		if (!tsl.getData(Raw)) {
			ERROR_PRINTLN("I2C ERROR");
		}
		SEN_VPRINTLN(Raw);
	}
	else {
		#ifdef QTRSINUSE
		unsigned int *a;
		rc[_pin].readCalibrated(a);
		Raw = (uint16_t)a;
		#endif
		}
}

//Updates the sensor's range of values for correct scaling
void Sensor::UpdateRange() {
	if (Raw >= Max) {
		Max = Raw;
	}
	
	if (Raw <= Min) {
		Min = Raw;
	}
}

//Normalise the sensor's reading on a scale between it's minimum and maximum
void Sensor::Normalise() {
	double n = 1, m = 1; //Internal variable for calculating normalised value
	n = Raw - Min;
	m = (n / Max) * NORMALISED_MAX;
	Normalised = m;
}

//Checks the given sensor values and returns and array of bools.
bool Sensor::GetReading() {
	ReadRaw();
	UpdateRange();
	Normalise();
	toTileColour();
	return tileWhite;
}

//Switches the I2C address of the desired sensor and removes others
void Sensor::SelectSensor(byte sensorNumber) {
	if (sensorNumber == sC::FR) { sensGPIO.writeGPIOA(0x7F); SEN_PRINTLN("FR Selected"); }
	else if (sensorNumber == sC::LTR) { sensGPIO.writeGPIOA(0xBF); SEN_PRINTLN("LTR Selected"); }
	else if (sensorNumber == sC::LTL) { sensGPIO.writeGPIOA(0xDF); SEN_PRINTLN("LTL Selected"); }
	else if (sensorNumber == sC::FL) { sensGPIO.writeGPIOA(0xEF); SEN_PRINTLN("FL Selected"); }
	else if (sensorNumber == sC::MR) { sensGPIO.writeGPIOA(0xF7); SEN_PRINTLN("MR Selected"); }
	else if (sensorNumber == sC::ML) { sensGPIO.writeGPIOA(0xFB); SEN_PRINTLN("ML Selected"); }
	else if (sensorNumber == sC::BR) { sensGPIO.writeGPIOA(0xFD); SEN_PRINTLN("BR Selected"); }
	else if (sensorNumber == sC::BL) { sensGPIO.writeGPIOA(0xFE); SEN_PRINTLN("BL Selected"); }
	else {
		ERROR_PRINTLN("Sensor Select Error");
		ERROR_PRINTLN("All address lines returned to default");
		sensGPIO.writeGPIOA(0xFF);
	}
}


//Checks the Normalised sensor readings against their thresholds
void Sensor::toTileColour() {
	double WhiteThreshold = NORMALISED_MAX * (double)(1 - (double)((double)UPPER_THRESHOLD / 100)); //Default threshold values of 200 and 800, They can be cahnegd by editing the definitions in the header file
	double BlackThreshold = NORMALISED_MAX * (double)((double)LOWER_THRESHOLD / 100);
	if (Normalised >= WhiteThreshold) {
		tileWhite = true;
	}
	else if (Normalised <= BlackThreshold) {
		tileWhite = false;
	}
}


//Being called by PollSensors
//This handles printing of the boolean values of the sensor values (x6), to either the XBee or the USB
//Prints boolean values, presently there is no material difference between the conditions,
//so one has been commented out until such time as communication code is finalised.
#ifndef SENSOR_MEMORY_SAVE
void Sensor::printbw() {
#else
void Sensor::printbw() {
#endif
	//Print sensor outputs
#ifdef SENSOR_DEBUG
	Serial.print(F("FL:")); Serial.print(RVAL(sC::FL)); Serial.print(F("\t"));
	Serial.print(F("LTL:")); Serial.print(RVAL(sC::LTL)); Serial.print(F("\t"));
	Serial.print(F("LTR:")); Serial.print(RVAL(sC::LTR)); Serial.print(F("\t"));
	Serial.print(F("FR:")); Serial.print(RVAL(sC::FR)); Serial.println(F("\t"));
	Serial.print(F("ML:")); Serial.print(RVAL(sC::ML)); Serial.print(F("\t"));
	Serial.print(F("\t")); Serial.print(F("MR:")); Serial.print(RVAL(sC::MR)); Serial.println(F("\t"));
	Serial.print(F("BL:")); Serial.print(RVAL(sC::BL)); Serial.print(F("\t"));
	Serial.print(F("\t")); Serial.print(F("BR:")); Serial.print(RVAL(sC::BR)); Serial.println(F("\t"));
#endif
}

//Public function callable from navigation to print sensor values for debug
#ifndef SENSOR_MEMORY_SAVE
void Sensor::printCurrent() {
#else
void Sensor::printCurrent() {
#endif
	Serial.print(F("FL:")); Serial.print(RVAL(sC::FL)); Serial.print(F("\t"));
	Serial.print(F("LTL:")); Serial.print(RVAL(sC::LTL)); Serial.print(F("\t"));
	Serial.print(F("LTR:")); Serial.print(RVAL(sC::LTR)); Serial.print(F("\t"));
	Serial.print(F("FR:")); Serial.print(RVAL(sC::FR)); Serial.println(F("\t"));
	Serial.print(F("ML:")); Serial.print(RVAL(sC::ML)); Serial.print(F("\t"));
	Serial.print(F("\t")); Serial.print(F("MR:")); Serial.print(RVAL(sC::MR)); Serial.println(F("\t"));
	Serial.print(F("BL:")); Serial.print(RVAL(sC::BL)); Serial.print(F("\t"));
	Serial.print(F("\t")); Serial.print(F("BR:")); Serial.print(RVAL(sC::BR)); Serial.println(F("\t"));
}

//Polls all given sensors in the order specified.
void Sensor::PollSensors(Sensor *sens, const sC::sensorNumber *order, const byte OrderLength){

	if (_sensorInitComplete == false) {
		ERROR_PRINTLN("Please call initSensors() before attempting to poll");
		return;
	}

	sC::sensorNumber currentSensorIndex = sC::FR;
	for (byte n = 0; n < OrderLength; n++) {
		currentSensorIndex = order[n];
		SelectSensor(static_cast<byte>(currentSensorIndex));
		SLASTVAL(currentSensorIndex, RVAL(currentSensorIndex));
		sens[currentSensorIndex].GetReading();
		SVAL(currentSensorIndex, sens[currentSensorIndex].tileWhite);
	}
	
	printbw();


}

#ifdef SENSOR_MEMORY_SAVE
	void Sensor::setVal(sC::sensorNumber position, bool tileColour){
		bitWrite(values, position, tileColour);
	}
		
	bool Sensor::valIs(sC::sensorNumber position){
		return bitRead(values, position);
	}

	bool Sensor::lastValIs(sC::sensorNumber position){
		return bitRead(lastValues, position);
	}

	void Sensor::setLastVal(sC::sensorNumber position, bool lastValue){
		bitWrite(values, position, lastValue);
	}

	uint8_t Sensor::values = 0x00;
	static uint8_t lastValues = 0x00;
#else
	bool Sensor::values[8] = {};
	bool Sensor::lastValues[8] = {};
#endif

void Sensor::initSensors() {
	sensGPIO.begin(1);
	
	//Address pins
	sensGPIO.pinMode(FRPIN, OUTPUT);
	sensGPIO.pinMode(LTRPIN, OUTPUT);
	sensGPIO.pinMode(LTLPIN, OUTPUT);
	sensGPIO.pinMode(FLPIN, OUTPUT);
	sensGPIO.pinMode(MRPIN, OUTPUT);
	sensGPIO.pinMode(MLPIN, OUTPUT);
	sensGPIO.pinMode(BRPIN, OUTPUT);
	sensGPIO.pinMode(BLPIN, OUTPUT);

	//-------Set all sensor select pins to high---------------
	sensGPIO.writeGPIOA(0xFF);

	//Ultrasonic pins
	pinMode(A0, OUTPUT);
	pinMode(A1, INPUT);
	
	 for (byte i = sC::FR; i < sC::invalid; i++) {
		SelectSensor(i);
		tsl.begin(TSL2561_ADDR_0);
		tsl.setTiming(TSL2561_GAIN_16X, TSL2561_INTEGRATIONTIME_14MS);
		tsl.setPowerUp();
	}
	 
	delay(14);
	_sensorInitComplete = true;
}

