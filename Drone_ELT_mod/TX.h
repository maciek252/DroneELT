/****************************************************
 * OpenLRSng transmitter code
 ****************************************************/
#ifndef __TX_H__
#define __TX_H__

#include "LEDs.h"

#include "Utils.h"
//#include "MemoryFree.h"
//#include "PositionBuffer.h"
//#include "TinyGPSWrapper.h"
//#include "TinyGPS.h"

//#include "LinkedList.h"

//#include "PositionBuffer.h"
#include "PositionBuffer2.h"

#include "TinyGPSplusplus.h"

#include "mavlinkMS.h"
#include "aircraft.h"
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

//static Poss poss;
static Position2 poss;

#if 0
static TinyGPS tinyGPS;
//TinyGPS * giveTinyGPS() {
//	return &tinyGPS;
//}
#endif

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

//static PositionBuffer positionBuffer;
//static Position position;

static bool led1State = false;
static bool led2State = false;

static float lastLED = 0;
static bool stateLED = false;

float last_osd_lat = 0;                    // latidude
float last_osd_lon = 0;                    // longitude
int last_osd_satellites_visible = 0;

uint8_t RF_channel = 0;

void setup(void) {
	uint32_t start;

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
	pinMode(BTN, INPUT); //Buton
	pinMode(PPM_IN, INPUT); //PPM from TX
	digitalWrite(PPM_IN, HIGH); // enable pullup for TX:s with open collector output
#if defined (RF_OUT_INDICATOR)
			pinMode(RF_OUT_INDICATOR, OUTPUT);
			digitalWrite(RF_OUT_INDICATOR, LOW);
#endif
	buzzerInit();

#ifdef __AVR_ATmega32U4__
	Serial.begin(0); // Suppress warning on overflow on Leonardo
	TelemetrySerial.setBuffers(serial_rxbuffer, SERIAL_BUF_RX_SIZE, serial_txbuffer, SERIAL_BUF_TX_SIZE);
#else
//  Serial.setBuffers(serial_rxbuffer, SERIAL_BUF_RX_SIZE, serial_txbuffer, SERIAL_BUF_TX_SIZE);
//  Serial.begin(115200);
	Serial.begin(57600);
	//Serial.setTimeout(15000UL);
#endif
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
	rx_reset();
	watchdogConfig (WATCHDOG_2S);

	beacon_initialize_audio();

	return;
/////////////////////////////////////////////////
	delay(50);

}

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

	uint32_t timeUs, timeMs;
	float mavLinkTimer = 0;

	//        updateLBeep(false);
	//        buzzerOff();

	//read_mavlink(c);
	//    return;
	//  watchdogReset();

#if 0
	if (millis() > mavLinkTimer + 100) {
		mavLinkTimer = millis();
		///       OnMavlinkTimer();

		//                read_mavlink();
		//                Serial.flush();
	}
#endif
	if (millis() > lastLED + 1000) {
		lastLED = millis();
		//Serial.flush();
		/*
		 beacon_initialize_audio();
		 beacon_tone(840,5);
		 watchdogReset();
		 beacon_finish_audio();
		 */
		//Serial.write("okoeee");
		Serial.write('Q');
		//return;

		//http://forum.arduino.cc/index.php?topic=44262.0
		//http://stackoverflow.com/questions/27651012/arduino-sprintf-float-not-formatting
		//    osd_roll = 2.35;
#if 0
		char buf[6];
		dtostrf(osd_roll, 5, 1, buf);
		Serial.write(buf[0]);
		Serial.write(buf[1]);
		Serial.write(buf[2]);
		Serial.write(buf[3]);
		Serial.write(buf[4]);
		Serial.write(buf[5]);
#endif

#if 1
		Serial.write('R');
		printDouble(the_aircraft.attitude.roll, 3);
		Serial.write('\n');
		//Serial.write('L');
		//printDouble(osd_lon, 6);
		Serial.write('\n');
#endif


#if 0
		Serial.write('R');
		printDouble(osd_roll, 3);
		Serial.write('\n');
		Serial.write('L');
		printDouble(osd_lon, 6);
		Serial.write('\n');
#endif

#if 0
		if(osd_roll > 45.0 && osd_roll < 53.0) {
			Serial.write('!');
			Serial.write('!');
			osd_roll = 0;
			//Serial.flush();
			Serial.end();
			beacon_send_number(02.953, 2, 3, 2);
			//Serial.flush();
			Serial.begin(57600);
		}
#endif
		//Serial.flush();

		//	 Serial.print("ee");
		//        Serial.println(osd_roll);

#if 0
		if (mavlink_active == 1) {

			device_mode = MAVLINK_SERIAL;
			ledMavlinkMode = DOUBLE_FLASH;

			//  	if (mavbeat == 1) {
			//Green_LED_OFF;
			//Red_LED_ON;
			Serial.write('A');
			//              Serial.flush();
			//Serial.println("Mavlink active");

			//        beacon_send_prelude(1);
			//               beacon_tone(440,1);
			//              watchdogReset();
			//                delay(10);
			//                beacon_finish_audio();

			//		 beacon_initialize_audio();
			//                beacon_send_prelude(2);
			//		 beacon_tone(240,10);
			//		 watchdogReset();
			//		 beacon_finish_audio();
			//		 delay(300);
			//	 beacon_send_number(7, 2, 2, 2);

			//static uint8_t      osd_satellites_visible = 0;     // number of satelites
			//static uint8_t      osd_fix_type = 0;               // GPS lock 0-1=no fix, 2=2D, 3=3D
			if (osd_fix_type == 3) {
				last_osd_lon = osd_lon;
				last_osd_lat = osd_lat;
				last_osd_satellites_visible = osd_satellites_visible;
			}

		} else {
			//           beacon
			//           beacon_tone(840,2);
			//          watchdogReset();
			//              delay(10);

			//Serial.println("NO Mavlink");
			return;
			beacon_send_number(12.345, 2, 3, 2);

			//        	 beacon_send_number(osd_roll, 2, 1, 2);
			//               	 beacon_send_number(osd_satellites_visible, 2, 0, 2);
			if (last_osd_lon != 0.0 && last_osd_lat != 0.0) {
				beacon_send_number(last_osd_satellites_visible, 2, 0, 2);
				beacon_send_number(last_osd_lon, 3, 5, 2);
				beacon_send_number(last_osd_lat, 3, 5, 2);
			}
			/*
			 beacon_initialize_audio();
			 beacon_tone(740,5);
			 watchdogReset();
			 beacon_finish_audio();


			 Green_LED_ON;
			 Red_LED_OFF;*/

		}
#endif
	}
}
//////////////////////////////////////////////////////////////////////////////////////

bool testIfNMEA5() {
	//Serial.setTimeout(15000UL);

	char inData[250]; // Allocate some space for the string
	uint8_t inChar; // Where to store the character read
	//Serial.write("ddd");
	int index33 = 0;
	inData[0] = 0;

	while (Serial.available() > 0) {
		//String str = Serial.readString();
		//str = Serial.readStringUntil('');
		//return true;
		inChar = Serial.read(); // Read a character
		//continue;

		if (!((inChar >= 'a' && inChar <= 'z')
				|| (inChar >= 'A' && inChar <= 'Z')
				|| (inChar >= '0' && inChar <= '9')))
			continue;

#if 1
		Serial.write('v');
		Serial.write(inChar);
#endif

		if (index33 < 203) { // One less than the size of the array

			inData[index33] = inChar; // Store it
			index33++; // Increment where to write next
			inData[index33] = 0; //'\0'; // Null terminate the string
		}
	}
	// GPRMC - rzadko
	if (index33 >= 5) {
		if (strstr(inData, "GNGSA") != NULL) {
			//Serial.print("Wynik: true");
			Serial.write('T');
			return true;
		}
	}
	//Serial.write('f');
	//Serial.print("NMEA: false");
	return false;
}

#if 1
bool readAndParse(uint8_t c) {

	//while (Serial.available() > 0)

	//Serial.write(c);
#if 1
	//bool result = tinyGPS.encode(c);
	bool result = gpsPlus.encode((char) c);
	//bool result = gpsPlus.test(c);
	//bool result = encode(c);
#endif

	return result;
}
#endif

void runGPSMAVLINK() {
	// to psuje parsowanie NMEA!!!
	while (Serial.available() > 0) {

		uint8_t c = Serial.read();
		//Serial.write(c)
		//if (gpsPlus.encode((byte)c))
			//displayInfo();
		//if(testIfNMEA6(c))
			//Serial.write('G');

		read_mavlink(c);

		//continue;
		//if (NO_GPS_TRY_MAVLINK == device_mode
			//	|| MAVLINK_SERIAL == device_mode) {

			uint32_t timeUs, timeMs;
			float mavLinkTimer = 0;

			//        updateLBeep(false);
			//        buzzerOff();

			//read_mavlink(c);
			serviceMavlink();
		//}
	}
}

void runGPSNMEA() {

	while (Serial.available() > 0) {

		uint8_t c = Serial.read();

		if (NO_GPS == device_mode) {
			if (testIfNMEA6(c)) {

				//ledGpsActivityTimer = millis();
				device_mode = GPS_SERIAL;
				ledGpsMode = SINGLE_FLASH;
			}
		}

		//delay(1000);
		if (gpsPlus.encode(c))
			displayInfo();
	}
}

void loop(void) {



#ifdef TEST_GPS
	while (*gpsStream) {
		//delay(1000);
		if (gpsPlus.encode(*gpsStream++))
		displayInfo();
	}
	return;
#endif

#if 0
	while (Serial.available() > 0) {

		uint8_t c = Serial.read();

		//delay(1000);
		if (gpsPlus.encode(c))
		displayInfo();
	}
	return;
#endif
	//beacon_initialize_audio();
	//beacon_send_number(2.4553, 2, 5, 3);

	//poss.resetPosition();

#if 1
	serviceLEDs();
	//positionBuffer.tick();
#endif

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

	////////////////////////////////////// BUTTON ////////////////////////////////////////////////////////
#if 1
	if (0 == digitalRead(BTN)) {
		//beep();
		//Serial.setTimeout(15000UL);
		//Serial.end();
		//Serial.begin(57600);
		//Serial.flush();
		Serial.write('m');
		//Serial.print(freeMemory2());
		//Serial.write('b');
		//Serial.print("f");
		if (OFF == ledMavlinkMode)
			ledMavlinkMode = SINGLE_FLASH;		// RED in Tx100mW
		else if (SINGLE_FLASH == ledMavlinkMode)
			ledMavlinkMode = DOUBLE_FLASH;
		else if (DOUBLE_FLASH == ledMavlinkMode)
			ledMavlinkMode = DOUBLE_FLASH_REV;
		else
			ledMavlinkMode = OFF;
	}
#endif
	//Serial.write('r');

	/////////////////////////////////////////////////// GPS SERIAL ////////////////////////////////////////
/*
	if (device_mode == NO_GPS || device_mode == GPS_SERIAL) {
		runGPSNMEA();
		return;
	}
*/
	runGPSMAVLINK();
	return;

	/////////////////////////////////////////////////// GPS SERIAL ////////////////////////////////////////

	while (Serial.available() > 0) {

		uint8_t c = Serial.read();
		//Serial.write(c);

#if 0
		//delay(1000);
		if (gpsPlus.encode(c))
		displayInfo();
		//continue; // z tym OK
#endif

#if 1
		if (NO_GPS == device_mode) {
			if (testIfNMEA6(c)) {

				//ledGpsActivityTimer = millis();
				device_mode = GPS_SERIAL;
				ledGpsMode = SINGLE_FLASH;
			}
		}
#endif
//		continue;
#if 1 // TU OK TU OK
		//delay(1000);
		if (gpsPlus.encode(c)) {
			displayInfo();

		}
		//continue;
#endif

		//continue;

#if 0 // to tez szkodzi NMEA
		if (GPS_SERIAL == device_mode) {
#if 0
			//delay(1000); // A TU NIE!!!!!!
			if (gpsPlus.encode(c))
			displayInfo();
			continue;
#endif

#if 0
			if (millis() - ledGpsActivityTimer > 6000) {
				ledGpsActivityTimer = millis();
				ledGpsMode = OFF;
				device_mode = NO_GPS;
				return;
			}
#endif
			//	ledGpsMode = SINGLE_FLASH;
			//Serial.write('S');
			//return;
#if 0
			if ((millis() - last_beep_time) > 3000) {
				last_beep_time = millis();
				/*
				 if (live_tick_sound) {
				 live_tick_sound = false;
				 beacon_send_number(0, 1, 0, 0);
				 } else {
				 live_tick_sound = true;
				 beacon_send_number(1, 1, 0, 0);
				 }
				 */
			}
#endif
			//led2mode = OFF;

			//if (readAndParse(c)) {
			if (gpsPlus.encode(c)) {
				displayInfo();
				ledGpsActivityTimer = millis();

				ledGpsMode = DOUBLE_FLASH;

#if 0
				Serial.print(F("Location: "));
				if (gpsPlus.location.isValid())
				{
					Serial.print(gpsPlus.location.lat(), 6);
					Serial.print(F(","));
					Serial.print(gpsPlus.location.lng(), 6);
				}
				else
				{
					Serial.print(F("INVALID"));
				}
#endif
				//Serial.write('P');
				// TinyGPS
				//      giveTinyGPS()->hdop();
#if 0
				if (TinyGPS::GPS_INVALID_SATELLITES == tinyGPS.satellites()) {
					//Serial.write("parseOK   NOFIX");
					Serial.write('P');
				} else {
					//Serial.write("parseOK  SATS: " + giveTinyGPS()->satellites());
					Serial.write('p');
					//beacon_initialize_audio();
					//beacon_send_number(2.4553, 2, 5, 3);
				}
#endif

				//int sats = gpsPlus.satellites.value();
				//Serial.print(sats);
#if 0
				int charProcessed = gpsPlus.charsProcessed();
				Serial.print(charProcessed);
				Serial.write(' ');
#endif

#if 1
				int sententesWithFix = gpsPlus.sentencesWithFix();
				Serial.print(sententesWithFix);
				Serial.write(' ');
				//double lat = gpsPlus.location.lat();
#endif

				if (magneticVariation.isUpdated()) {
					//Serial.print("Magnetic variation is ");
					//Serial.println(magneticVariation.value());
					Serial.write('M');
				}

				//double lat = tinyGPS._latitude / 1000000.0;
				//printDouble((double)lat, 5);
				//printDouble(123.456789, 5);
				//beacon_initialize_audio();
				//beacon_send_number(2.45	53, 2, 5, 3);
#if 0
				if (flat != 0.0 && flon != 0.0) {
					ledGpsMode = ON;
					beacon_initialize_audio();
					beacon_send_number(2.4553, 2, 5, 3);
					beacon_send_number(flat, 2, 5, 3);
					beacon_send_number(flon, 2, 5, 3);
				}
#endif

				return;

#if 0
				Serial.print(flat);
				Serial.print(" LON=");
				Serial.print(flon);
				Serial.print(" SAT=");

				Serial.print(
						giveTinyGPS()->satellites()
						== TinyGPS::GPS_INVALID_SATELLITES ?
						0 : giveTinyGPS()->satellites());
				Serial.print(" PREC=");
				Serial.print(
						giveTinyGPS()->hdop() == TinyGPS::GPS_INVALID_HDOP ?
						0 : giveTinyGPS()->hdop());
#endif
			}
		}
		////////////////////////////////////////////////////END GPS SERIAL ////////////////////////////////////
#endif

	}

}

#endif
