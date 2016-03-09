/****************************************************
 * OpenLRSng transmitter code
 ****************************************************/

#ifndef __TX_H__
#define __TX_H__

#include "LEDs.h"

//#include "Utils.h"
#include "MemoryFree.h"
//#include "PositionBuffer.h"
//#include "TinyGPSWrapper.h"
#include "TinyGPS.h"

static TinyGPS tinyGPS;
TinyGPS * giveTinyGPS() {
	return &tinyGPS;
}

static long ledGpsActivityTimer = 0.0;
static long ledMavlinkActivityTimer = 0.0;

enum DEVICE_MODE {
	NO_GPS,
	NO_GPS_TRY_MAVLINK,
	GPS_SERIAL,
	MAVLINK_SERIAL,
	GPS_I2C,
	BOTH_MAVLINKSERIAL_GPSI2C
};

static DEVICE_MODE device_mode = NO_GPS_TRY_MAVLINK; //NO_GPS;

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

static bool led1State = false;
static bool led2State = false;

static float lastLED = 0;
static bool stateLED = false;

float last_osd_lat = 0;                    // latidude
float last_osd_lon = 0;                    // longitude
int last_osd_satellites_visible = 0;

uint32_t mavlink_last_inject_time = 0;
uint16_t rxerrors = 0;

uint8_t RF_channel = 0;

uint8_t altPwrIndex = 0; // every nth packet at lower power
uint8_t altPwrCount = 0;


uint32_t lastTelemetry = 0;


uint16_t linkQuality = 0;
uint16_t linkQualityRX = 0;

volatile uint8_t ppmAge = 0; // age of PPM data

volatile uint8_t ppmCounter = 255; // ignore data until first sync pulse

uint8_t serialMode = 0; // 0 normal, 1 spektrum 1024 , 2 spektrum 2048, 3 SBUS, 4 SUMD


#ifdef DEBUG_DUMP_PPM
uint8_t ppmDump = 0;
uint32_t lastDump = 0;
#endif

/****************************************************
 * Interrupt Vector
 ****************************************************/

static inline void processPulse(uint16_t pulse) {
	if (serialMode) {
		return;
	}

#if (F_CPU == 16000000)
	if (!(tx_config.flags & MICROPPM)) {
		pulse >>= 1; // divide by 2 to get servo value on normal PPM
	}
#elif (F_CPU == 8000000)
	if (tx_config.flags & MICROPPM) {
		pulse<<= 1; //  multiply microppm value by 2
	}
#else
#error F_CPU invalid
#endif


}



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
#endif


	setupRfmInterrupt();

	sei();

	start = millis();
	while ((ppmAge == 255) && ((millis() - start) < 2000))
		;


	digitalWrite(BTN, HIGH);
	Red_LED_ON;

	Serial.flush();

	Serial.print("OpenLRSng TX starting ");
	printVersion(version);
	Serial.print(" on HW ");
	Serial.println(BOARD_TYPE);

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



	if (bind_data.serial_baudrate && (bind_data.serial_baudrate < 5)) {
		serialMode = bind_data.serial_baudrate;
		TelemetrySerial.begin((serialMode == 3) ? 100000 : 115200); // SBUS is 100000 rest 115200
	} else {
		// switch to userdefined baudrate here
		TelemetrySerial.begin(bind_data.serial_baudrate);
	}



	Red_LED_OFF;
	buzzerOff();



	altPwrIndex = 0;
	if (tx_config.flags & ALT_POWER) {
		if (bind_data.hopchannel[6] && bind_data.hopchannel[13]
				&& bind_data.hopchannel[20]) {
			altPwrIndex = 7;
		} else {
			altPwrIndex = 5;
		}
	}

	init_rfm(0);
	rfmSetChannel(RF_channel);
	rx_reset();




	watchdogConfig(WATCHDOG_2S);
}


uint8_t frameIndex = 0;
uint32_t srxLast = 0;
uint8_t srxFlags = 0;
uint8_t srxChannels = 0;


/////////////////////////////////////// LOOP ///////////////////////////////////////////////////
// ELT
#if 0
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

bool testIfNMEA3() {
	//Serial.setTimeout(15000UL);

	char inData[250]; // Allocate some space for the string
	char inChar; // Where to store the character read
	//Serial.write("ddd");
	int index33 = 0;
	inData[0] = 0;

	while (Serial.available() > 0) {
		//str = Serial.readString();
		//str = Serial.readStringUntil('');
		//return true;
		inChar = Serial.read(); // Read a character
		if (index33 < 203) { // One less than the size of the array

			inData[index33] = inChar; // Store it
			index33++; // Increment where to write next
			inData[index33] = 0; //'\0'; // Null terminate the string
		}
	}
	// GPRMC - rzadko
	if (index33 > 0)
		if (strstr(inData, "GNGSA")) {
			//Serial.print("Wynik: true");
			Serial.write('t');
			return true;
		}
	//Serial.write('f');
	//Serial.print("NMEA: false");
	return false;
}

bool readAndParse() {
//  readAndParseTest();
	while (Serial.available() > 0) {

		//    mavlink_active = 1;//test
		//    lastMAVBeat = millis();
		//uint8_t c = Serial.read();
		char c = Serial.read();
		Serial.write(c);
#if 1
		bool result = tinyGPS.encode(c);
		if (result)
			return true;
#endif
		//        Serial.write(c);
	}
	return false;
}

#endif

/////////////////////////////////////// LOOP ///////////////////////////////////////////////////
// ELT

void loop(void) {

#if 0
	serviceLEDs();
	//positionBuffer.tick();
#endif

#if 0
	if (millis() - detectionTimer > 3000) {

		detectionTimer = millis();
		//Serial.flush();
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

#if 0
	////////////////////////////////////// BUTTON ////////////////////////////////////////////////////////
	if (0 == digitalRead(BTN)) {
		//beep();
		//Serial.setTimeout(15000UL);
		//Serial.end();
		//Serial.begin(57600);
		//Serial.flush();
		Serial.write('m');
		Serial.print(freeMemory2());
		//Serial.write('b');
		//Serial.print("f");
		if (OFF == ledMavlinkMode)
		ledMavlinkMode = SINGLE_FLASH;// RED in Tx100mW
		else if (SINGLE_FLASH == ledMavlinkMode)
		ledMavlinkMode = DOUBLE_FLASH;
		else if (DOUBLE_FLASH == ledMavlinkMode)
		ledMavlinkMode = DOUBLE_FLASH_REV;
		else
		ledMavlinkMode = OFF;
	}
#endif
	uint32_t timeUs, timeMs;
	float mavLinkTimer = 0;

//        updateLBeep(false);
//        buzzerOff();
	read_mavlink();

//  watchdogReset();

//	 beacon_send_number(7, 2, 2, 2);
//              read_mavlink();
	if (millis() > mavLinkTimer + 100) {
		mavLinkTimer = millis();
///       OnMavlinkTimer();

//                read_mavlink();
//                Serial.flush();
	}

	if (millis() > lastLED + 1000) {
		lastLED = millis();
		//Serial.flush();
		/*
		 beacon_initialize_audio();
		 beacon_tone(840,5);
		 watchdogReset();
		 beacon_finish_audio();
		 */
		//Serial.write("okoeeeeeeeeeeeeeeeeeeee");
		Serial.write('Q');

		char buf[6];

		//http://forum.arduino.cc/index.php?topic=44262.0
		//http://stackoverflow.com/questions/27651012/arduino-sprintf-float-not-formatting
//    osd_roll = 2.35;
		dtostrf(osd_roll, 5, 1, buf);
		Serial.write(buf[0]);
		Serial.write(buf[1]);
		Serial.write(buf[2]);
		Serial.write(buf[3]);
		Serial.write(buf[4]);
		Serial.write(buf[5]);

		return;
//	 Serial.print("ee");
//        Serial.println(osd_roll);	 
		if (stateLED == true) {
			stateLED = false;
			Green_LED_OFF;
			Red_LED_ON;
		} else {
			stateLED = true;
			Green_LED_ON;
			Red_LED_OFF;
		}

		if (mavlink_active == 1) {
//  	if (mavbeat == 1) {
			Green_LED_OFF;
			Red_LED_ON;
			Serial.write('A');
//              Serial.flush();
			Serial.println("Mavlink active");

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
			Serial.println("NO Mavlink");
//                 beacon_send_number(12.345, 2, 3, 2);

			return;
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
	}
	return;




}

#endif
