#ifndef _CONFIG_H
#define _CONFIG_H


#define FIVEVOLTOUTPUTPIN 1
#define ADCINPUTPIN	   2

#define P1BCPIN        3
#define P15VPIN        8
#define P1ADCPIN       9
#define P2GNDPIN       4
#define P25VPIN        2
#define P2RdPIN        1
#define P2RkPIN        0
#define P2ADCPIN       13
#define GNDADCPIN     10
#define GNDGNDPIN      11
#define GND5VPIN       12
#define P1P2SWITCHPIN  7
#define BCPIN          5    //both ADCswitch and BC to be put on 1 pin in next circuit rev
#define ADCSWITCHPIN   14

#define NUMADCREADINGS 50



#include <Arduino.h>


namespace boxConfig {

	enum boxSettings {
		box1	=	1,
		box2a	=	2,
		box2b	=	3,
		box3a	=	4,
		box3b	=	5,
		box4a	=	6,
		box4b	=	7,
		box5a	=	8,
		box5b	=	9,
		box6a	=	10,
		box6b	=	11,
		box7a	=	12,
		box7b	=	13
	};


	namespace otherThings {};
}
#endif