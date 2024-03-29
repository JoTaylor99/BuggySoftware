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
	



	if (NavigateLoop == 1) {
		//Serial.println(str);
		NavigateLoop = 0;
		//return;
		int length = str.length();
		for (int i = 0; i < length ; i++) {
			//Serial.println(str[i]);
			if (str[i] != '\0') {
				if (str[i] == 'T') {
					//Serial.println("Got into box case");
					
					sendMovementComplete();
					buggy.passedBoxNumber = str[i + 1] - '0';
					uint8_t Polarity = str[i + 2] - '0';
					if (Polarity == 'f') {
						buggy.passedBoxInversion = false;
					}
					else if (Polarity == 'r') {
						buggy.passedBoxInversion = true;
					}

					}
					buggy.navigate("G");
					i += 2;
					//sendMovementComplete();
				}
				else {
					buggy.navigate(String(str[i]));
					//if (str != "R" && str != "L") {
						sendMovementComplete();
					//}
				}




				
			}
			//Serial.println(str);
		}
		str = "";
		NavigateLoop = 0;
	}

void buggyTop::Check()
{
	//Serial.println("Check");
	int result = readData(&receiveCmd, sizeof(receiveCmd));
	if (result > 0) {
		parseData(&receiveCmd);
	}
}




void buggyTop::parseData(struct Frame *theData) {
	Frame sendCmd;
	//Serial.println(theData->cmd);
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
		//Serial.println(str);
		str = "";
		sendAcknowledge(Comms::FunctionCodes::RouteRst);
		break;
	case Comms::FunctionCodes::Go:
		if (CurrentPhase == Comms::Phase::Operational) {
			sendAcknowledge(Comms::FunctionCodes::Go);
			NavigateLoop = 1;
		}
		break;
	case Comms::FunctionCodes::EmergencyStop:
		//digitalWrite(13, LOW);

		RESET_AVR2();
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
	if (theData->data != 0x00) {
		
		str += (char)theData->data;
	}
	if (theData->data >> 8 != 0x00) {
		str += (char)(theData->data >> 8);
	}
	if (theData->data >> 16 != 0x00) {
		str += (char)(theData->data >> 16);
	}
	if (theData->data >> 24 != 0x00) {
		str += (char)(theData->data >> 24);
	}
	NavigateLoop = 0;
}

void buggyTop::controlManually(Frame * theData)
{
	uint8_t a = theData->data;
	//String temp = "";
	str = "";
	str += (char)a;
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
	Frame sendCmd;
	sendCmd.cmd = Comms::FunctionCodes::Acknowledge;
	sendCmd.data = (uint32_t)cmd;
	sendData((void*)&sendCmd, sizeof(sendCmd));
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

//Handler Left
void leftStepCounter() {
	buggy.stepDistanceLeft++;
	if (buggy.stepDistanceLeft >= buggy.stepTargetDistanceLeft) {
		PCICR &= ~bit(digitalPinToPCICRbit(LEFTMOTORCOUNT));
		digitalWrite(LEFTMOTOR, LOW);
		buggy.stepDistanceLeft = 0;
		buggy._leftWheelTask = true;
	}
}

//Handler Right
void rightStepCounter(){
	buggy.stepDistanceRight++;
	if (buggy.stepDistanceRight >= buggy.stepTargetDistanceRight) {
		PCICR &= ~bit(digitalPinToPCICRbit(RIGHTMOTORCOUNT));
		digitalWrite(RIGHTMOTOR, LOW);
		buggy.stepDistanceRight = 0;
		buggy._rightWheelTask = true;
	}
}

ISR(PCINT2_vect) // handle pin change interrupt for D0 to D7 here
{
	rightStepCounter();
}

ISR(PCINT0_vect) // handle pin change interrupt for D8 to D13 here
{
	leftStepCounter();
}