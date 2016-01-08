// Do not remove the include below
//####### COMPILATION TARGET #######
// TX - tx=1, board=2
// RX  tx=1, board=3
// Enable to compile transmitter code, default is RX (remove leading //)
//#define COMPILE_TX 0 // compile RX code
#define COMPILE_TX 1 // compile TX code

#define BOARD_TYPE 2 // 2 = Flytron OpenLRS M2/M3 Tx Board / OrangeRx UHF TX
//#define BOARD_TYPE 3 // 3 = Flytron OpenLRS Rx v2 Board / OrangeRx UHF RX / HawkEye UHF RX (RX and TX supported)

//####### Transparant serial #######
#define COM_BUF_MAXSIZE 32 // maximum downlink packet size. Max value is 64 , No use having higher than SERIAL_BUFSIZE, (also 1 byte per rf packet is dedicated for datalength and flags). Mavlink telemetry mode allows for serial downlink != 9

//####### MAVLink #######
#define MAVLINK_INJECT_INTERVAL 1000000






#include <FastSerial.h>
#define DefineSerialPort(_name, _num) FastSerialPort(_name, _num)
#include <Arduino.h>

#include "OSD_Vars.h"

#include  "MAVLinkOSD.h"

#include "test2.h"
#include <GCS_MAVLink.h>

#include "version.h"
#include "hardware.h"
#include "wd.h"
#include "binding.h"
#include "common.h"

#if (COMPILE_TX == 1)
#include "binary_com.h"
#include "rxc.h"
#ifdef CLI_ENABLED
#include "dialog.h"
#endif
#include "frskytx.h"
#include "TX.h"
#else
#include "mavlink.h"
#include "I2C.h"
#include "serialPPM.h"
#include "RX.h"
#endif

#define SERIAL_BAUD 57600

//FastSerialPort0(Serial);

//DefineSerialPort(Serial, 0);

/*
//DefineSerialPort(Serial, 0);

//#define DefineSerialPort(_name, _num) FastSerialPort(_name, _num)

//The setup function is called once at startup of the sketch
void setup()
{
	  uint16_t d = 030;
	  Serial.begin(9600);

// Add your initialization code here
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
	while(1){
		Serial.write("baba");
	}

}
*/
