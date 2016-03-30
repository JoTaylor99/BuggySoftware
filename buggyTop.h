#ifndef _BUGGYTOP_H
#define _BUGGYTOP_H

#include "Config.h"
#include "navigation.h"
#include "Comms.h"

struct Frame {
	uint8_t cmd;
	uint32_t data;
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

	void Check();
	void parseData(struct Frame *theData);
	void AppendRoute(struct Frame *theData);
	void controlManually(struct Frame *theData);


	void sendError(Comms::ErrorCodes err);
	void sendAcknowledge(Comms::FunctionCodes cmd);
	
};

#endif