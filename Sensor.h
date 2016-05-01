// Sensor.h

#ifndef _SENSOR_H
#define _SENSOR_H


#include <SparkFunTSL2561.h>
#include "Config.h"
#include <Adafruit_MCP23017.h>

/// <summary>
/// Conditional inclusion of QTR Sensors library
/// </summary>
#ifdef QTRSINUSE
#include <QTRSensors.h>
#endif


/// <summary>
/// Sensor is a class included from navigation which contains the code to control and access output from the optical sensors
/// </summary>
class Sensor {

public:
	/// <summary>
	/// Sensor Constructor
	/// </summary>
	/// <param name="Pin">Pin of address select GPIO expander sensor was connected to</param>
	Sensor(uint8_t Pin);

	/// <summary>
	/// Sensor constructor
	/// </summary>
	/// <param name="Pin">where S == TSL, pin of address select GPIO expander sensor was connected to, where S==QTR, pin of Arduino MCU sensor was connected to</param>
	/// <param name="S">Sensor type: TSL or QTR</param>
	Sensor(uint8_t Pin, sC::SensorType S);

	
	/// <summary>
	/// Destructor
	/// </summary>
	~Sensor();
		
	/// <summary>
	/// Arrays containing different combinations of sensors
	/// For use if only a subset of sensors need polling, ususally only done for marginal speed increase
	/// </summary>
	static const sC::sensorNumber DefaultOrder[8];
	static const sC::sensorNumber Front[4];
	static const sC::sensorNumber Back[2];
	static const sC::sensorNumber Middle[2];
	static const sC::sensorNumber FrontNMiddle[6];
	
	/// <summary>
	/// Sets up address select GPIO expander, initialises and powers one each sensor in turn with gain and integration time settings, 
	/// and delays one integration period to ensure readings are immediately available.
	/// </summary>
	static void initSensors();
	
	/// <summary>
	/// Polls the sensors in the order specified
	/// </summary>
	/// <param name="sens">A pointer to the array of sensor objects to poll</param>
	/// <param name="order">A pointer to the constant array containing the poll order, has default value of all sensors indexed front right to back left</param>
	/// <param name="OrderLength">The number of sensors to poll, has default value of all sensors</param>
	static void PollSensors(Sensor *sens, const sC::sensorNumber *order = Sensor::DefaultOrder, const byte OrderLength = NUM_SENSORS);

	/// <summary>
	/// Prints to serial the last recorded boolean values of each sensor
	/// Note to get fully up to date readings a call to PollSensors should immediately proceed calling this function.
	/// </summary>
	static void printCurrent();

	/// <summary>
	/// Static sensor value storage variables and access functions, see config.h for explanation of sensor_memory_save
	/// Note these variables store the values of all the sensors
	/// </summary>
#ifdef SENSOR_MEMORY_SAVE
	private:
		static void setVal(sC::sensorNumber position, bool tileColour);
		static void setLastVal(sC::sensorNumber position, bool lastValue);
		static uint8_t lastValues;

	public:
		static bool valIs(sC::sensorNumber position);
		static bool lastValIs(sC::sensorNumber position);
		static uint8_t values;
#else
	public:
		static bool values[8];
		static bool lastValues[8];
#endif


private:
	
	/// <summary>
	/// stores the boolean sensor value for specific sensor object
	/// </summary>
	bool tileWhite;

	/// <summary>
	/// Sensor pin number
	/// </summary>
	uint8_t _pin;

	/// <summary>
	/// Max observed raw value of specific sensor object
	/// </summary>
	uint16_t Max;

	/// <summary>
	/// Min observed raw value of specific sensor object
	/// </summary>
	uint16_t Min;

	/// <summary>
	/// Last observed raw value of specific sensor object
	/// </summary>
	uint16_t Raw;

	/// <summary>
	/// Last normalised value calculated of specific sensor object
	/// </summary>
	double Normalised;

	/// <summary>
	/// Sensor object type
	/// </summary>
	sC::SensorType _s;
	
	/// <summary>
	/// Value retrieval function
	/// </summary>
	/// <returns>boolean interpretation of normalised value retrieved</returns>
	bool GetReading();

	/// <summary>
	/// Static Sensor select function, toggles pins on the GPIO expander to switch the specified sensor to the correct I2C address and all other sensors to an alternate I2C address
	/// </summary>
	/// <param name="sensorNumber">Sensor to select</param>
	static void SelectSensor(byte sensorNumber);

	/// <summary>
	/// Reads the raw value from whichever sensor is enabled
	/// </summary>
	void ReadRaw();

	/// <summary>
	/// Updates the sensors range of values by amending min and max to allow for correct normalisation
	/// </summary>
	void UpdateRange();

	/// <summary>
	/// Normalise the sensors most recent reading on a scale between the previous observed maximum and minimum value
	/// </summary>
	void Normalise();

	/// <summary>
	/// Checks the normalised sensor reading against the static thresholds defined in Config.h
	/// </summary>
	void toTileColour();
	
	/// <summary>
	/// Prints the sensor values in the same manner as printCurrent, however this function is private and will not print anything unless the
	/// SENSOR_DEBUG preprocessor switch is enabled.
	/// </summary>
	static void printbw();

	/// <summary>
	/// static boolean function to ensure sensors are initialised before value retrieval is attempted.
	/// </summary>
	static bool _sensorInitComplete;  

};

#endif

