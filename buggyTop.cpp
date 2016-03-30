#include "buggyTop.h" //Top level file for the Buggy Code.
#include <DFRobot_utility.h>
#include <utility/SerialProtocol.h>
buggyTop::buggyTop() {};

buggyTop::~buggyTop() {};

navigation buggy;





void buggyTop::init() {
	/*Code for testing box analysis, actual structure requires team b and c discussion*/
	COMPILE_DATE;
	COMPILE_TIME;
	buggy.initNavigation();
	str = "";
	CurrentPhase = Comms::Phase::Idle;
};

void buggyTop::go() {
	/* Code here is needed either here or in a not yet written communications class to separate an incomming command string from controlling PC into useful variables
	*  For the time being this is being done in the navigate function in the navigation class.
	*/

	

	Check();
	
	//if (Serial.available() > 0) { // If a command has been sent
	//	str = Serial.readString();
	//	buggy.navigate(str);
	//}

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




void buggyTop::Check()
{
	int result = readData(&receiveCmd, sizeof(receiveCmd));
	if (result > 0) {
		parseData(&receiveCmd);
	}
}



void buggyTop::parseData(struct Frame *theData) {
	Frame sendCmd;
	switch (theData->cmd) {
	case Comms::FunctionCodes::ConnectionRequest:
		sendCmd.cmd = Comms::ConnectionAccept;
		sendCmd.data = (uint32_t)0;
		sendData((void*)&sendCmd, sizeof(sendCmd));
		break;
	case Comms::FunctionCodes::ConnectionAccept:
		//This case shouldn't be reached
		break;
	case Comms::FunctionCodes::CurrentStatus:
		sendCmd.cmd = theData->cmd;
		sendCmd.data = CurrentPhase;
		sendData((void*)&sendCmd, sizeof(sendCmd));
		break;
	case Comms::FunctionCodes::ChangePhase:
		CurrentPhase = (Comms::Phase)((uint8_t)theData->data);
		sendAcknowledge(Comms::FunctionCodes::ChangePhase);
		break;
	case Comms::FunctionCodes::ParameterRequest:
		//Coming soon to a buggy near you
		break;
	case Comms::FunctionCodes::ParameterSet:
		//Coming soon to a buggy near you
		break;
	case Comms::FunctionCodes::RouteCmd:
		if (CurrentPhase == Comms::Phase::RouteDownload) {
			AppendRoute(theData);
			sendAcknowledge(Comms::FunctionCodes::RouteCmd);
		}
		else {
			sendError(Comms::ErrorCodes::IncorrectPhase);
		}
		break;
	case Comms::FunctionCodes::RouteRst:
		str = "";
		sendAcknowledge(Comms::FunctionCodes::RouteRst);
		break;
	case Comms::FunctionCodes::Go:
		sendAcknowledge(Comms::FunctionCodes::Go);
		buggy.navigate(str);
		break;
	case Comms::FunctionCodes::EmergencyStop:
		//Coming soon to a buggy near you
		break;
	case Comms::FunctionCodes::Acknowledge:
		//Unreachable
		break;

	default:
		sendError(Comms::ErrorCodes::UnknownCmd);
	}
	//Serial.println("Switch case complete");
}

void buggyTop::AppendRoute(struct Frame *theData) {
	char a;
	if (theData->data != 0xFF) {
		//Serial.println((char)theData->data);
		str += (char)theData->data;
		 
	}
	if (theData->data>>8 != 0xFF) {
		str += (char)theData->data;
	}
	if (theData->data>>16 != 0xFF) {
		str += (char)theData->data;
	}
	if (theData->data>>24 != 0xFF) {
		str += (char)theData->data;
	}
}

void buggyTop::sendError(Comms::ErrorCodes err)
{
	Frame sendCmd;
	sendCmd.cmd = Comms::FunctionCodes::Error;
	sendCmd.data = (uint32_t)err;
	sendData((void*)&sendCmd, sizeof(sendCmd));

}

void buggyTop::sendAcknowledge(Comms::FunctionCodes cmd)
{
	Frame sendCmd;
	sendCmd.cmd = Comms::FunctionCodes::Acknowledge;
	sendCmd.data = (uint32_t)cmd;
	sendData((void*)&sendCmd, sizeof(sendCmd));
}


