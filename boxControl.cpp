#include "boxControl.h"

boxControl::boxControl(){};

boxControl::~boxControl(){};

boxControl::switcher::switcher(){
	static unsigned short _currentRelayState = 0;
	static bool initComplete = 0;
};
boxControl::switcher::~switcher(){};

void boxControl::switcher::initSwitcher(){};

void boxControl::initControl(){
	

};

bool boxControl::switchControl(boxConfig::boxSettings boxSetting){};

void boxControl::setDirection(bool inverted){};

bool boxControl::checkboxcontrolInit() {
	bool switcherResult = false;
	switcher GPIO;
	switcherResult = GPIO.checkswitchercontrolInit();
	bool result = false;
	result = ((_boxcontrolinitComplete && switcherResult) ? true : false);
	return result;
}

bool boxControl::switcher::checkswitchercontrolInit(){
	bool result = false;
	result = (_switcherinitComplete ? true : false);
	return result;
}

bool boxControl::_boxcontrolinitComplete = false;

bool boxControl::switcher::_switcherinitComplete = false;

void boxControl::setInput(inputStatus position){};
void boxControl::setInput(inputStatus position, byte boxNumber){};