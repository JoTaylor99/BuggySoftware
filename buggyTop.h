#ifndef _BUGGYTOP_H
#define _BUGGYTOP_H

#include "Config.h"
#include "navigation.h"
#include "Comms.h"

class buggyTop {

public:

	buggyTop();

	~buggyTop();

	void init();

	void go();



};


//Handler Left
void leftStepCounter();

//Handler Right
void rightStepCounter();


#endif