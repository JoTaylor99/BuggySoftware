#ifndef _BUGGYTOP_H
#define _BUGGYTOP_H
#include <DFRobot_utility.h>
#include <utility/SerialProtocol.h>
#include "Config.h"
#include "navigation.h"
#include "Comms.h"

struct Frame {
	uint8_t cmd;
	uint32_t data;
};

union parseDouble{
	double Val;
	uint32_t Data;
};

union parseShort
{
	short Val;
	uint16_t Data;
};

class buggyTop {

public:

	buggyTop();

	~buggyTop();
	//
	struct Frame receiveCmd;
	Comms::Phase CurrentPhase;
	void init();

	void go();

	String str;
	volatile int NavigateLoop = 0;
	void Check();
	void parseData(struct Frame *theData);
	void AppendRoute(struct Frame *theData);
	void controlManually(struct Frame *theData);


	void sendError(Comms::ErrorCodes err);
	static void sendAcknowledge(Comms::FunctionCodes cmd);
	static void sendBoxValue(Comms::FunctionCodes cmd, double val);
	static void sendBoxValue(Comms::FunctionCodes cmd, short val);
	static void sendMovementComplete();
};

#endif