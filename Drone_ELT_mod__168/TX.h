/****************************************************
 * OpenLRSng transmitter code
 ****************************************************/
#ifndef __TX_H__
#define __TX_H__

//#include <fifo.h>
#include <aircraft.h>
#include <mavlink.h>
//#include <asynch_tx.h>
//#include <frsky.h>

#include "LEDs.h"

#include "OneButton.h"

#include "Utils.h"
//#include "MemoryFree.h"
//#include "PositionBuffer.h"
//#include "TinyGPSWrapper.h"
//#include "TinyGPS.h"

//#include "LinkedList.h"

//#include "PositionBuffer.h"
#include "PositionBuffer2.h"

#include "TinyGPSplusplus.h"

//#include "mavlinkMS.h"
//#include "aircraft.h"
//#define TEST_GPS

#ifdef TEST_GPS
#if 0
const char *gpsStream =
"$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
"$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
"$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
"$GPGGA,045201.000,3014.3864,N,09748.9411,W,1,10,1.2,200.8,M,-22.5,M,,0000*6C\r\n"
"$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
"$GPGGA,045252.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n";
#endif

#if 0
const char *gpsStream =
"$GNGSA,A,3,79,78,68,85,70,86,,,,,,,1.39,0.76,1.16*1B\r\n"
"$GPGSV,3,1,12,02,33,294,22,03,16,134,33,04,13,029,25,05,07,307,22*7B\r\n"
"$GPGSV,3,2,12,06,38,238,31,07,44,192,38,09,87,011,32,16,25,070,26*7F\r\n"
"$GPGSV,3,3,12,23,53,079,30,26,18,041,27,29,05,350,26,30,16,204,21*70\r\n"
"$GLGSV,3,1,10,68,48,104,40,69,71,328,17,70,16,302,21,77,15,027,26*64\r\n"
"$GLGSV,3,2,10,78,60,072,26,79,49,160,42,80,01,190,,84,10,254,15*64\r\n"
"$GLGSV,3,3,10,85,22,302,36,86,13,354,30*60\r\n"
"$GNGLL,5212.86777,N,02110.62510,E,083531.40,A,A*70\r\n"
"$GNRMC,083531.60,A,5212.86778,N,02110.62511,E,0.047,,110316,,,A*62\r\n";
#endif

const char *gpsStream =
"$GNGGA,233804.20,5212.86861,N,02110.62773,E,1,12,0.76,102.0,M,34.1,M,,*49\r\n"
"$GNRMC,232617.80,A,5212.86936,N,02110.62812,E,0.039,,110316,,,A*60\r\n"
"$GNVTG,,T,,M,0.039,N,0.071,K,A*31\r\n"
"$GNGGA,232617.80,5212.86936,N,02110.62812,E,1,12,0.66,103.0,M,34.1,M,,*45\r\n";

#endif

OneButton button(11, true);

#if 0
class Poss {

	int e;
public:

	void resetPosition() {
		latitude = 0.0;
		longitude = 0.0;
		hdop = 0.0;
		numOfSats = 0;
		triggerTime = 0.0;
		fired = false;
		numOfSamples = 0;
	}

	/*
	 void update(Poss position) {
	 if(true){
	 //if (position.numOfSats >= 3) {
	 latitude += position.latitude;
	 longitude += position.longitude;
	 numOfSats += position.numOfSats;
	 hdop += position.hdop;
	 numOfSamples++;
	 }
	 }
	 */

	double latitude, longitude;
	int numOfSats, numOfSamples;
	double hdop;
	double triggerTime;
	bool fired;
};
#endif

//static Poss poss;
//static Position2 poss;

#if 0
static TinyGPS tinyGPS;
//TinyGPS * giveTinyGPS() {
//	return &tinyGPS;
//}
#endif

static bool audioInitialized = false;

static bool playingAlarm = false;

static TinyGPSPlus gpsPlus;
static TinyGPSCustom magneticVariation(gpsPlus, "GNRMC", 10);

//static PositionBuffer positionBuffer;
//static Position2 position;

static long ledGpsActivityTimer = 0.0;
static long ledMavlinkActivityTimer = 0.0;

static int nmeaDetectionCounter = 0;

enum DEVICE_MODE {
	NO_GPS,
	NO_GPS_TRY_MAVLINK,
	GPS_SERIAL,
	MAVLINK_SERIAL,
	GPS_I2C,
	BOTH_MAVLINKSERIAL_GPSI2C
};

static DEVICE_MODE device_mode = NO_GPS;
//NO_GPS_TRY_MAVLINK; //NO_GPS;

enum ELT_STATE {
	NO_POSITION_DATA_RCV, // leds off
	POSITION_DATA_NO_FIX, // 1 flash
	POSITION_DATA_FIX_NOT_STARTED, // 2 flashes
	DRONE_STARTED, // 1 flash inverted
	//DRONE_STARTED_NO_POSITION,
	ALARM
};

static ELT_STATE eltState = NO_POSITION_DATA_RCV;

static void changeEltState(ELT_STATE e) {
	eltState = e;
}

static LED_STATE ledGpsMode = OFF; // BLUE in Tx100mW
static LED_STATE ledMavlinkMode = OFF; // RED in Tx100mW

static long detectionTimer = 0.0;

static PositionBuffer positionBuffer;
//static Position position;

static bool led1State = false;
static bool led2State = false;

static float lastLED = 0;
static bool stateLED = false;

float last_osd_lat = 0;                    // latidude
float last_osd_lon = 0;                    // longitude
int last_osd_satellites_visible = 0;

uint8_t RF_channel = 0;

void sendCurrentPosition() {

	Serial.println(F("ALARM!"));
	//if (!audioInitialized) {
	if(true){
		beacon_initialize_audio();
		audioInitialized = true;
	}
	delay(1000);

	//beacon_send_number(92.453, 2, 3, 2);
	beacon_send_number(positionBuffer.lastValidPosition.latitude, 2, 6, 2);
	//delay(1000);			//beacon_finish_audio();
	delay(1000);
	beacon_send_number(positionBuffer.lastValidPosition.longitude, 2, 6, 2);
	delay(1000);
	beacon_finish_audio();
}

void doubleclick() {

	if (ledMavlinkMode == SINGLE_FLASH || ledMavlinkMode == QUICK_FLASH
			|| ledGpsMode == SINGLE_FLASH || ledGpsMode == QUICK_FLASH) {

		Serial.println(F("ALARM-test! Transmitting 12.345"));
		beacon_initialize_audio();
		//while (true)
		beacon_send_number(12.3, 2, 1, 2);
		beacon_finish_audio();
		Serial.println(F("ALARM-test done!"));

	} else if (ledMavlinkMode == DOUBLE_FLASH ||
			ledGpsMode == DOUBLE_FLASH
	) {
		Serial.println(F("ALARM-test! Transmitting current pos"));
		sendCurrentPosition();
		Serial.println(F("ALARM-test curr pos done!"));
		//beacon_finish_audio();
	}

}

void singleclick() {
	//positionBuffer.startCriterionMetFlag = true;

	if (ledMavlinkMode == DOUBLE_FLASH) {
		ledMavlinkMode = TRIPLE_FLASH;
	} else if (ledGpsMode == DOUBLE_FLASH) {
		ledGpsMode = TRIPLE_FLASH;
	}

}

void setup(void) {
	uint32_t start;

	//button.attachClick(singleclick);
	button.attachDoubleClick(doubleclick);
	//button.attachLongPressStart(singleclick);

	watchdogConfig (WATCHDOG_OFF);

	setupSPI();
#ifdef SDN_pin
	pinMode(SDN_pin, OUTPUT); //SDN
	digitalWrite(SDN_pin, 0);
#endif
//LED and other interfaces
	pinMode(Red_LED, OUTPUT); //RED LED
	pinMode(Green_LED, OUTPUT); //GREEN LED
#ifdef Red_LED2
			pinMode(Red_LED2, OUTPUT); //RED LED
			pinMode(Green_LED2, OUTPUT);//GREEN LED
#endif
	//pinMode(BTN, INPUT); //Buton
	pinMode(PPM_IN, INPUT); //PPM from TX
	digitalWrite(PPM_IN, HIGH); // enable pullup for TX:s with open collector output
#if defined (RF_OUT_INDICATOR)
			pinMode(RF_OUT_INDICATOR, OUTPUT);
			digitalWrite(RF_OUT_INDICATOR, LOW);
#endif
	buzzerInit();

//  Serial.setBuffers(serial_rxbuffer, SERIAL_BUF_RX_SIZE, serial_txbuffer, SERIAL_BUF_TX_SIZE);
//  Serial.begin(115200);
	Serial.begin(57600);
//Serial.setTimeout(15000UL);
	profileInit();
	txReadEeprom();

//setupPPMinput();

	setupRfmInterrupt();

	sei();

	start = millis();

	digitalWrite(BTN, HIGH);
	Red_LED_ON;

	printVersion(version);
//Serial.print(" on HW ");
//Serial.println(BOARD_TYPE);
//Serial.flush();

//  beacon_send_number(7, 2, 2, 2);
// przeniesione tu:
	init_rfm(0);
	rfmSetChannel(RF_channel);
//	rx_reset();
	watchdogConfig (WATCHDOG_2S);

//	beacon_initialize_audio();

	/*
	 if (0 == digitalRead(BTN)) {
	 Serial.println(F("ALARM-test! Transmitting 12.345"));
	 beacon_initialize_audio();
	 while(true)
	 beacon_send_number(12.345, 2, 3, 2);
	 Serial.println(F("ALARM-test done!"));
	 }
	 */

	return;
/////////////////////////////////////////////////
	delay(50);

}

void printStatus() {
	if (NO_GPS == device_mode)
		Serial.println(F("------------- NO GPS (try nmea)---------------"));
	else if (NO_GPS_TRY_MAVLINK == device_mode)
		Serial.println(F("------------- NO GPS TRY MAVLINK ---------------"));
	else if (GPS_SERIAL == device_mode)
		Serial.println(F("------------- GPS (nmea)---------------"));
	else if (MAVLINK_SERIAL == device_mode)
		Serial.println(F("------------- NO GPS (mavlink)---------------"));

}

#if 0
void displayInfo() {
	Serial.print(F("Location: "));
	if (gpsPlus.location.isValid()) {
		Serial.print(gpsPlus.location.lat(), 6);
		Serial.print(F(","));
		Serial.print(gpsPlus.location.lng(), 6);
	} else {
		Serial.print(F("INVALID"));
	}

	Serial.print(F("  Date/Time: "));
	if (gpsPlus.date.isValid()) {
		Serial.print(gpsPlus.date.month());
		Serial.print(F("/"));
		Serial.print(gpsPlus.date.day());
		Serial.print(F("/"));
		Serial.print(gpsPlus.date.year());
	} else {
		Serial.print(F("INVALID"));
	}

	Serial.print(F(" "));
	if (gpsPlus.time.isValid()) {
		if (gpsPlus.time.hour() < 10)
		Serial.print(F("0"));
		Serial.print(gpsPlus.time.hour());
		Serial.print(F(":"));
		if (gpsPlus.time.minute() < 10)
		Serial.print(F("0"));
		Serial.print(gpsPlus.time.minute());
		Serial.print(F(":"));
		if (gpsPlus.time.second() < 10)
		Serial.print(F("0"));
		Serial.print(gpsPlus.time.second());
		Serial.print(F("."));
		if (gpsPlus.time.centisecond() < 10)
		Serial.print(F("0"));
		Serial.print(gpsPlus.time.centisecond());
	} else {
		Serial.print(F("INVALID"));
	}

	Serial.println();
}
#endif

/////////////////////////////////////// LOOP ///////////////////////////////////////////////////
// ELT

static void serviceLEDs() {
	serviceLED(ledGpsMode, led1Timer, led1State, led1FlashCounter);
	serviceLED(ledMavlinkMode, led2Timer, led2State, led2FlashCounter);

	if (led1State) {
		Green_LED_ON;
	} else {
		Green_LED_OFF;
	}
	if (led2State) {
		Red_LED_ON;
	} else {
		Red_LED_OFF;
	}
}

bool testIfNMEA6(uint8_t inChar) {
//Serial.setTimeout(15000UL);

//String str = Serial.readString();
//str = Serial.readStringUntil('');
//return true;
//uint8_t
//byte inChar = Serial.read(); // Read a character
//continue;

//	 if (!( (inChar >= 'a' && inChar <= 'z') || (inChar >= 'A' && inChar <= 'Z') || (inChar >= '0' && inChar <= '9')))
//	 continue;

#if 0
	Serial.write('v');
	Serial.write(inChar);
#endif

//return true;

	if (nmeaDetectionCounter == 0 && inChar == 'G') {
		nmeaDetectionCounter = 1;
	} else if (nmeaDetectionCounter == 1 && inChar == 'N') {
		nmeaDetectionCounter = 2;
	} else if (nmeaDetectionCounter == 2 && inChar == 'G') {
		nmeaDetectionCounter = 3;
	} else if (nmeaDetectionCounter == 3 && inChar == 'S') {
		nmeaDetectionCounter = 4;
	} else if (nmeaDetectionCounter == 4 && inChar == 'A') {
		nmeaDetectionCounter = 0;
		Serial.write('T');
		return true;
	} else {
		nmeaDetectionCounter = 0;
	}

	return false;

}
////////////////////////////////////////////////// service mavlink /////////////////////////////////////
void serviceMavlink() {

	float mavLinkTimer = 0;

	if (millis() > lastLED + 1000) {
		lastLED = millis();
		//Serial.flush();
		Serial.write('Q');

#if 0

		printDouble(the_aircraft.attitude.roll, 3);
		if (the_aircraft.attitude.roll > 45
				&& the_aircraft.attitude.roll < 50) {
			if (!audioInitialized) {
				beacon_initialize_audio();
				audioInitialized = true;
			}
			delay(1000);
			beacon_send_number(92.453, 2, 3, 2);
			//delay(1000);			//beacon_finish_audio();
			delay(1000);
			//Serial.flush();
			Serial.end();
			Serial.begin(57600);

		}
#endif

#if 1

		Serial.write('\n');
		Serial.write('L');
		printDouble(the_aircraft.location.gps_lat / 10000000.0, 7);
		Serial.write('O');
		printDouble(the_aircraft.location.gps_lon / 10000000.0, 7);
		Serial.write('S');
		Serial.print(the_aircraft.gps.num_sats);
		Serial.write('H');
		Serial.print(the_aircraft.location.gps_hdop);
		Serial.write('\n');

		Position2 pos;
		pos.latitude = the_aircraft.location.gps_lat / 10000000.0;
		pos.longitude = the_aircraft.location.gps_lon / 10000000.0;
		pos.numOfSats = the_aircraft.gps.num_sats;
		pos.hdop = the_aircraft.location.gps_hdop;
		if (pos.numOfSats >= 3) {
			if (ledMavlinkMode == SINGLE_FLASH) {
				ledMavlinkMode = DOUBLE_FLASH;
				//ledMavlinkMode = TRIPLE_FLASH;
			}
			positionBuffer.addGPSPositionToOneSecondBuffers(pos);
		}

#endif

	}
}
//////////////////////////////////////////////////////////////////////////////////////

#if 1
bool readAndParse(uint8_t c) {

//bool result = tinyGPS.encode(c);
	bool result = gpsPlus.encode((char) c);
//bool result = gpsPlus.test(c);
	return result;
}
#endif

void loop(void) {

	if (playingAlarm) {
		sendCurrentPosition();
		return;
	}


	button.tick();


#if 1
	if (millis() - detectionTimer > 6000) {
		detectionTimer = millis();
		//Serial.flush();
		Serial.write('D');

		if (NO_GPS == device_mode) {
			//updateLBeep(true);
			device_mode = NO_GPS_TRY_MAVLINK;
			ledMavlinkMode = QUICK_FLASH;
			ledGpsMode = OFF;

		} else if (NO_GPS_TRY_MAVLINK == device_mode) {
			ledGpsMode = QUICK_FLASH;
			ledMavlinkMode = OFF;
			device_mode = NO_GPS;
		}
	}
#endif

	while (Serial.available() > 0) {

		uint8_t c = Serial.read();

		// NMEA //////////////////////////////////////
		if (NO_GPS == device_mode) {
			if (testIfNMEA6(c)) {

				device_mode = GPS_SERIAL;
				ledGpsMode = SINGLE_FLASH;
			}
		}
		if (GPS_SERIAL == device_mode) {
			if (gpsPlus.encode(c)) {
				Position2 pos;
				if (gpsPlus.satellites.isValid()
						&& gpsPlus.satellites.value() >= 3) {
					if (SINGLE_FLASH == ledGpsMode)
						ledGpsMode = DOUBLE_FLASH;
					pos.latitude = gpsPlus.location.lat();
					pos.longitude = gpsPlus.location.lng();
					pos.numOfSats = gpsPlus.satellites.value();
					pos.hdop = gpsPlus.hdop.value();
					if (pos.numOfSats >= 3) {
						positionBuffer.addGPSPositionToOneSecondBuffers(pos);
					}
				}
				//				displayInfo();
			}
		}

#if 1
		if (device_mode == NO_GPS_TRY_MAVLINK
				|| device_mode == MAVLINK_SERIAL) {
			// bylo OK ale testujemy nmea (duzo pamieci to zjada  - niekiedy)
			if (read_mavlink(c)) {
				device_mode = MAVLINK_SERIAL;
				if (QUICK_FLASH == ledMavlinkMode)
					ledMavlinkMode = SINGLE_FLASH;
			}
			serviceMavlink();
		}
#endif

	}

#ifdef TEST_GPS
	while (*gpsStream) {
		//delay(1000);
		if (gpsPlus.encode(*gpsStream++))
		displayInfo();
	}
	return;
#endif

//beacon_initialize_audio();
//beacon_send_number(2.4553, 2, 5, 3);

//poss.resetPosition();

#if 1
	serviceLEDs();
	positionBuffer.tick();
#endif

	if (positionBuffer.startCriterionMetFlag
//|| (0 == digitalRead(BTN))
	) {

		if (ledMavlinkMode == DOUBLE_FLASH) {
			ledMavlinkMode = TRIPLE_FLASH;
		} else if (ledGpsMode == DOUBLE_FLASH) {
			ledGpsMode = TRIPLE_FLASH;
		}
	}

#if 0
	if (ledMavlinkMode == TRIPLE_FLASH
			&& positionBuffer.alarmCriterionMetFlag) {
		ledMavlinkMode = SINGLE_FLASH_REV;
	}
	if (ledGpsMode == TRIPLE_FLASH && positionBuffer.alarmCriterionMetFlag) {
		ledGpsMode = SINGLE_FLASH_REV;
	}
#endif
	if ((ledGpsMode == TRIPLE_FLASH || ledMavlinkMode == TRIPLE_FLASH)
			&& positionBuffer.alarmCriterionMetFlag) {
		//ledGpsMode = SINGLE_FLASH_REV;
		playingAlarm = true;
		return;
	}

////////////////////////////////////// BUTTON ////////////////////////////////////////////////////////
#if 0
	if (0 == digitalRead(BTN)) {
		printStatus();
		positionBuffer.startCriterionMetFlag = true;
		Serial.write('m');
		//Serial.print(freeMemory2());
		//Serial.write('b');
		//Serial.print("f");
		/*
		 if (OFF == ledMavlinkMode)
		 ledMavlinkMode = SINGLE_FLASH;		// RED in Tx100mW
		 else if (SINGLE_FLASH == ledMavlinkMode)
		 ledMavlinkMode = DOUBLE_FLASH;
		 else if (DOUBLE_FLASH == ledMavlinkMode)
		 ledMavlinkMode = DOUBLE_FLASH_REV;
		 else
		 ledMavlinkMode = OFF;
		 */
	}
#endif

}

void mavlinkLedOn() {
	ledMavlinkMode = ON;
}

void mavlinkLedOff() {
	ledMavlinkMode = OFF;
}

void gpsLedOn() {
	ledGpsMode = ON;
}

void gpsLedOff() {
	ledGpsMode = OFF;
}

#if 0
Serial.write("S");
//return;
Position result = positionBuffer.testWawer5();
Serial.println(result.getLatitude());
Serial.println(result.getLongitude());
printDouble(3.123456789, 6);
printDouble(result.getLatitude(), 6);
printDouble(result.getLongitude(), 6);

double distanceWawerWodynska = positionBuffer.testDistanceWawerWodynska1();
Serial.println("distance Wawer-Wodynska");
printDouble(distanceWawerWodynska, 4);
return;
#endif

#endif
