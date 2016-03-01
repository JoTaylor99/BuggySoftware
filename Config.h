#ifndef _CONFIG_H
#define _CONFIG_H


#define FIVEVOLTOUTPUTPIN 1
#define ADCINPUTPIN	   2

#define pinP1BC        3
#define pinP15V        8
#define pinP1ADC       9
#define pinP2GND       4
#define pinP25V        2
#define pinP2Rd        1
#define pinP2Rk        0
#define pinP2ADC       13
#define pinGNDADC      10
#define pinGNDGND      11
#define pinGND5V       12
#define pinP1P2switch  7
#define pinBC          5    //both ADCswitch and BC to be put on 1 pin in next circuit rev
#define pinADCswitch   14

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