#include "buggyTop.h" //Top level file for the Buggy Code.

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
	NavigateLoop = 0;
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
};

void buggyTop::go() {
	/* Code here is needed either here or in a not yet written communications class to separate an incomming command string from controlling PC into useful variables
	*  For the time being this is being done in the navigate function in the navigation class.
	*/
	delay(300);



	if (NavigateLoop == 1) {
		int length = str.length();
		for (int i = 0; i < length ; i++) {
			if (str[i] != '\0') {
				if (str[i] == 'T') {
					uint8_t BoxNum = str[i + 1] - '0';
					uint8_t Polarity = str[i + 2] = '0';
					buggy.navigate(F("G"));
					i += 2;
				}
				else {
					buggy.navigate(String(str[i]));
				}



				
				sendMovementComplete();
			}
			//Serial.println(str);
		}
		NavigateLoop = 0;

		
	}



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
	case Comms::FunctionCodes::ChangePhase:
		CurrentPhase = (Comms::Phase)((uint8_t)theData->data);
		sendAcknowledge(Comms::FunctionCodes::ChangePhase);
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
		if (CurrentPhase == Comms::Phase::Operational) {
			//sendAcknowledge(Comms::FunctionCodes::Go);
			NavigateLoop = 1;
		}
		break;
	case Comms::FunctionCodes::EmergencyStop:
		digitalWrite(13, LOW);

		//Reset_AVR();
		return;
		//Hello Darkness My Old Friend
		break;
	case Comms::FunctionCodes::Acknowledge:
		//Unreachable
		break;
	case Comms::FunctionCodes::ManualCtrl:
		controlManually(theData);

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
	if (theData->data >> 8 != 0xFF) {
		str += (char)theData->data;
	}
	if (theData->data >> 16 != 0xFF) {
		str += (char)theData->data;
	}
	if (theData->data >> 24 != 0xFF) {
		str += (char)theData->data;
	}
	NavigateLoop = 0;
}

void buggyTop::controlManually(Frame * theData)
{
	uint8_t a = theData->data;
	String temp = "";
	temp += (char)a;
	NavigateLoop = 1;
	sendAcknowledge((Comms::FunctionCodes)a);
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
	//Frame sendCmd;
	//sendCmd.cmd = Comms::FunctionCodes::Acknowledge;
	//sendCmd.data = (uint32_t)cmd;
	//sendData((void*)&sendCmd, sizeof(sendCmd));
}

void buggyTop::sendBoxValue(Comms::FunctionCodes cmd, double val) {
	Frame sendCmd;
	sendCmd.cmd = cmd;
	parseDouble D;
	D.Val = val;
	sendCmd.data = D.Data;
	sendData((void*)&sendCmd, sizeof(sendCmd));

}

void buggyTop::sendBoxValue(Comms::FunctionCodes cmd, short val) {
	Frame sendCmd;
	sendCmd.cmd = cmd;
	parseShort D;
	D.Val = val;
	sendCmd.data = D.Data;
	sendData((void*)&sendCmd, sizeof(sendCmd));

}
void buggyTop::sendMovementComplete() {

	Frame sendCmd;
	sendCmd.cmd = Comms::FunctionCodes::MovementComplete;
	sendCmd.data = (uint32_t)0;
	sendData((void*)&sendCmd, sizeof(sendCmd));
}