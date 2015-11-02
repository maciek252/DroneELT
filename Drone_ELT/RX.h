/****************************************************
 * OpenLRSng receiver code
 ****************************************************/

uint32_t mavlink_last_inject_time = 0;
MavlinkFrameDetector mavlinkIncomingFrame; // Track the incoming mavlink frames (from Tx)

static float lastLED = 0;
static bool stateLED = false;

uint16_t rxerrors = 0;

#include <avr/eeprom.h>
uint8_t RF_channel = 0;

bool oko = false;

uint32_t lastPacketTimeUs = 0;
uint32_t lastRSSITimeUs = 0;
uint32_t linkLossTimeMs;

uint32_t lastBeaconTimeMs;

uint8_t RSSI_count = 0;
uint16_t RSSI_sum = 0;
uint8_t lastRSSIvalue = 0;
uint8_t smoothRSSI = 0;
uint8_t compositeRSSI = 0;
uint16_t lastAFCCvalue = 0;

uint16_t linkQuality = 0;
uint8_t linkQ;

uint8_t ppmCountter = 0;
uint16_t ppmSync = 40000;
uint8_t ppmChannels = 8;

volatile uint8_t disablePWM = 0;
volatile uint8_t disablePPM = 0;
uint8_t failsafeActive = 0;

uint16_t failsafePPM[PPM_CHANNELS];

uint8_t linkAcquired = 0;
uint8_t numberOfLostPackets = 0;

volatile uint8_t slaveState = 0; // 0 - no slave, 1 - slave initializing, 2 - slave running, 3- errored
uint32_t slaveFailedMs = 0;

bool willhop = 0, fs_saved = 0;

pinMask_t chToMask[PPM_CHANNELS];
pinMask_t clearMask;

void outputUp(uint8_t no) {
	PORTB |= chToMask[no].B;
	PORTC |= chToMask[no].C;
	PORTD |= chToMask[no].D;
}

void outputDownAll() {
	PORTB &= clearMask.B;
	PORTC &= clearMask.C;
	PORTD &= clearMask.D;
}

#if (F_CPU == 16000000)
#define PWM_MULTIPLIER 2
#define PPM_PULSELEN   600
#define PWM_DEJITTER   32
#define PPM_FRAMELEN   40000
#elif (F_CPU == 8000000)
#define PWM_MULTIPLIER 1
#define PPM_PULSELEN   300
#define PWM_DEJITTER   16
#define PPM_FRAMELEN   20000
#else
#error F_CPU not supported
#endif

volatile uint16_t nextICR1;

ISR(TIMER1_OVF_vect)
{
	if (ppmCountter < ppmChannels) {
		ICR1 = nextICR1;
		nextICR1 = servoBits2Us(PPM[ppmCountter]) * PWM_MULTIPLIER;
		ppmSync -= nextICR1;
		if (ppmSync < (rx_config.minsync * PWM_MULTIPLIER)) {
			ppmSync = rx_config.minsync * PWM_MULTIPLIER;
		}
		if ((disablePPM) || ((rx_config.flags & PPM_MAX_8CH) && (ppmCountter >= 8))) {
#ifdef USE_OCR1B
			OCR1B = 65535; //do not generate a pulse
#else
			OCR1A = 65535; //do not generate a pulse
#endif
		} else {
#ifdef USE_OCR1B
			OCR1B = nextICR1 - PPM_PULSELEN;
#else
			OCR1A = nextICR1 - PPM_PULSELEN;
#endif
		}

		while (TCNT1 < PWM_DEJITTER);
		outputDownAll();
		if ((!disablePWM) && (ppmCountter > 0)) {
			outputUp(ppmCountter - 1);
		}

		ppmCountter++;
	} else {
		ICR1 = nextICR1;
		nextICR1 = ppmSync;
		if (disablePPM) {
#ifdef USE_OCR1B
			OCR1B = 65535; //do not generate a pulse
#else
			OCR1A = 65535; //do not generate a pulse
#endif
		} else {
#ifdef USE_OCR1B
			OCR1B = nextICR1 - PPM_PULSELEN;
#else
			OCR1A = nextICR1 - PPM_PULSELEN;
#endif
		}
		ppmSync = PPM_FRAMELEN;

		while (TCNT1 < PWM_DEJITTER);
		outputDownAll();
		if (!disablePWM) {
			outputUp(ppmChannels - 1);
		}

		ppmCountter = 0;
	}
}

uint16_t RSSI2Bits(uint8_t rssi) {
	uint16_t ret = (uint16_t) rssi << 2;
	if (ret < 12) {
		ret = 12;
	} else if (ret > 1012) {
		ret = 1012;
	}
	return ret;
}

void set_PPM_rssi() {
	if (rx_config.RSSIpwm < 48) {
		uint8_t out;
		switch (rx_config.RSSIpwm & 0x30) {
		case 0x00:
			out = compositeRSSI;
			break;
		case 0x10:
			out = (linkQ << 4);
			break;
		default:
			out = smoothRSSI;
			break;
		}
		PPM[rx_config.RSSIpwm & 0x0f] = RSSI2Bits(out);
	} else if (rx_config.RSSIpwm < 63) {
		PPM[(rx_config.RSSIpwm & 0x0f)] = RSSI2Bits(linkQ << 4);
		PPM[(rx_config.RSSIpwm & 0x0f) + 1] = RSSI2Bits(smoothRSSI);
	}
}

void set_RSSI_output() {
	linkQ = countSetBits(linkQuality & 0x7fff);
	if (linkQ == 15) {
		// RSSI 0 - 255 mapped to 192 - ((255>>2)+192) == 192-255
		compositeRSSI = (smoothRSSI >> 1) + 128;
	} else {
		// linkquality gives 0 to 14*9 == 126
		compositeRSSI = linkQ * 9;
	}

	cli();
	set_PPM_rssi();
	sei();

	if (rx_config.pinMapping[RSSI_OUTPUT] == PINMAP_RSSI) {
		if ((compositeRSSI == 0) || (compositeRSSI == 255)) {
			TCCR2A &= ~(1 << COM2B1); // disable RSSI PWM output
			digitalWrite(OUTPUT_PIN[RSSI_OUTPUT],
					(compositeRSSI == 0) ? LOW : HIGH);
		} else {
			OCR2B = compositeRSSI;
			TCCR2A |= (1 << COM2B1); // enable RSSI PWM output
		}
	}
}

void failsafeApply() {
	if (failsafePPM[0] != 0xffff) {
		for (int16_t i = 0; i < PPM_CHANNELS; i++) {
			if (i == (rx_config.RSSIpwm & 0x0f)) {
				continue;
			}
			if ((i == (rx_config.RSSIpwm & 0x0f) + 1)
					&& (rx_config.RSSIpwm > 47)) {
				continue;
			}
			cli();
			PPM[i] = failsafePPM[i];
			sei();
		}
	}
}

void setupOutputs() {
	uint8_t i;

/*
	ppmChannels = getChannelCount(&bind_data);
	if ((rx_config.RSSIpwm & 0x0f) == ppmChannels) {
		ppmChannels += 1;
	}
	if ((rx_config.RSSIpwm > 47) && (rx_config.RSSIpwm < 63)
			&& ((rx_config.RSSIpwm & 0x0f) == ppmChannels - 1)) {
		ppmChannels += 1;
	}
	if (ppmChannels > 16) {
		ppmChannels = 16;
	}

	for (i = 0; i < OUTPUTS; i++) {
		chToMask[i].B = 0;
		chToMask[i].C = 0;
		chToMask[i].D = 0;
	}
	clearMask.B = 0xff;
	clearMask.C = 0xff;
	clearMask.D = 0xff;
	for (i = 0; i < OUTPUTS; i++) {
		if (rx_config.pinMapping[i] < PPM_CHANNELS) {
			chToMask[rx_config.pinMapping[i]].B |= OUTPUT_MASKS[i].B;
			chToMask[rx_config.pinMapping[i]].C |= OUTPUT_MASKS[i].C;
			chToMask[rx_config.pinMapping[i]].D |= OUTPUT_MASKS[i].D;
			clearMask.B &= ~OUTPUT_MASKS[i].B;
			clearMask.C &= ~OUTPUT_MASKS[i].C;
			clearMask.D &= ~OUTPUT_MASKS[i].D;
		}
	}
*/
	for (i = 0; i < OUTPUTS; i++) {
		switch (rx_config.pinMapping[i]) {
		case PINMAP_ANALOG:
			pinMode(OUTPUT_PIN[i], INPUT);
			break;
		case PINMAP_TXD:
		case PINMAP_RXD:
		case PINMAP_SDA:
		case PINMAP_SCL:
			break; //ignore serial/I2C for now
		default:
			if (i == RXD_OUTPUT) {
				UCSR0B &= 0xEF; //disable serial RXD
			}
			if (i == TXD_OUTPUT) {
				UCSR0B &= 0xF7; //disable serial TXD
			}
      pinMode(OUTPUT_PIN[i], OUTPUT); //PPM,PWM,RSSI,LBEEP // ELT cisza
			break;
		}
	}

	if (rx_config.pinMapping[PPM_OUTPUT] == PINMAP_PPM) {
		digitalWrite(OUTPUT_PIN[PPM_OUTPUT], HIGH);
#ifdef USE_OCR1B
		TCCR1A = (1 << WGM11) | (1 << COM1B1);
#else
		TCCR1A = (1 << WGM11) | (1 << COM1A1);
#endif
	} else {
		TCCR1A = (1 << WGM11);
	}

	disablePWM = 1;
	disablePPM = 1;

	if ((rx_config.pinMapping[RSSI_OUTPUT] == PINMAP_RSSI)
			|| (rx_config.pinMapping[RSSI_OUTPUT] == PINMAP_LBEEP)) {
    pinMode(OUTPUT_PIN[RSSI_OUTPUT], OUTPUT);// cisza
		  digitalWrite(OUTPUT_PIN[RSSI_OUTPUT], LOW); //cisza
		if (rx_config.pinMapping[RSSI_OUTPUT] == PINMAP_RSSI) {
			TCCR2A = (1 << WGM20);
			TCCR2B = (1 << CS20);
		} else { // LBEEP
			 // cisza
			 TCCR2A = (1 << WGM21); // mode=CTC
			 #if (F_CPU == 16000000)
			 TCCR2B = (1 << CS22) | (1 << CS20); // prescaler = 128
			 #elif (F_CPU == 8000000)
			 TCCR2B = (1 << CS22); // prescaler = 64
			 #else
			 #error F_CPU not supported
			 #endif
			 OCR2A = 62; // 1KHz
			 
		}
	}

	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
#ifdef USE_OCR1B
	OCR1B = 65535;  // no pulse =)
#else
	OCR1A = 65535;  // no pulse =)
#endif
	ICR1 = 2000; // just initial value, will be constantly updated
	ppmSync = PPM_FRAMELEN;
	nextICR1 = PPM_FRAMELEN;
	ppmCountter = 0;
	TIMSK1 |= (1 << TOIE1);

	if ((rx_config.flags & IMMEDIATE_OUTPUT) && failsafePPM[0] != 0xffff) {
		failsafeApply();
		disablePPM = 0;
		disablePWM = 0;
	}
}

void updateLBeep(bool packetLost) {
	// MS - by nie piszczalo!

	 #if defined(LLIND_OUTPUT)
	 if (rx_config.pinMapping[LLIND_OUTPUT] == PINMAP_LLIND) {
	 digitalWrite(OUTPUT_PIN[LLIND_OUTPUT],packetLost);
	 }
	 #endif
	 if (rx_config.pinMapping[RSSI_OUTPUT] == PINMAP_LBEEP) {
	 if (packetLost) {
	 TCCR2A |= (1 << COM2B0); // enable tone
	 } else {
	 TCCR2A &= ~(1 << COM2B0); // disable tone
	 }
	 }
	 
}

static inline void updateSwitches() {
	uint8_t i;
	for (i = 0; i < OUTPUTS; i++) {
		uint8_t map = rx_config.pinMapping[i];
		if ((map & 0xf0) == 0x10) { // 16-31
			digitalWrite(OUTPUT_PIN[i], (PPM[map & 0x0f] > 255) ? HIGH : LOW);
		}
	}
}

uint8_t bindReceive(uint32_t timeout) {
	uint32_t start = millis();
	uint8_t rxb;
	init_rfm(1);
	RF_Mode = Receive;
	to_rx_mode();
	Serial.println("Waiting bind\n");

	while ((!timeout) || ((millis() - start) < timeout)) {
		if (RF_Mode == Received) {
			Serial.println("Got pkt\n");
			spiSendAddress(0x7f);   // Send the package read command
			rxb = spiReadData();
			if (rxb == 'b') {
				for (uint8_t i = 0; i < sizeof(bind_data); i++) {
					*(((uint8_t*) &bind_data) + i) = spiReadData();
				}

				if (bind_data.version == BINDING_VERSION) {
					Serial.println("data good\n");
					rxb = 'B';
					tx_packet(&rxb, 1); // ACK that we got bound
					Green_LED_ON; //signal we got bound on LED:s
					return 1;
				}
			} else if ((rxb == 'p') || (rxb == 'i')) {
				uint8_t rxc_buf[sizeof(rx_config) + 1];
				if (rxb == 'p') {
					rxc_buf[0] = 'P';
					timeout = 0;
				} else {
					rxInitDefaults(1);
					rxc_buf[0] = 'I';
				}
				if (watchdogUsed) {
					rx_config.flags |= WATCHDOG_USED;
				} else {
					rx_config.flags &= ~WATCHDOG_USED;
				}
				memcpy(rxc_buf + 1, &rx_config, sizeof(rx_config));
				tx_packet(rxc_buf, sizeof(rx_config) + 1);
			} else if (rxb == 't') {
				uint8_t rxc_buf[sizeof(rxSpecialPins) + 5];
				timeout = 0;
				rxc_buf[0] = 'T';
				rxc_buf[1] = (version >> 8);
				rxc_buf[2] = (version & 0xff);
				rxc_buf[3] = OUTPUTS;
				rxc_buf[4] = sizeof(rxSpecialPins) / sizeof(rxSpecialPins[0]);
				memcpy(rxc_buf + 5, &rxSpecialPins, sizeof(rxSpecialPins));
				tx_packet(rxc_buf, sizeof(rxSpecialPins) + 5);
			} else if (rxb == 'u') {
				for (uint8_t i = 0; i < sizeof(rx_config); i++) {
					*(((uint8_t*) &rx_config) + i) = spiReadData();
				}
				accessEEPROM(0, true);
				rxb = 'U';
				tx_packet(&rxb, 1); // ACK that we updated settings
			} else if (rxb == 'f') {
				uint8_t rxc_buf[33];
				if (failsafePPM[0] != 0xffff) {
					rxc_buf[0] = 'F';
					for (uint8_t i = 0; i < 16; i++) {
						uint16_t us = servoBits2Us(failsafePPM[i]);
						rxc_buf[i * 2 + 1] = (us >> 8);
						rxc_buf[i * 2 + 2] = (us & 0xff);
					}
				} else {
					rxc_buf[0] = 'f';
				}
				tx_packet(rxc_buf, 33);
			} else if (rxb == 'g') {
				for (uint8_t i = 0; i < 16; i++) {
					uint16_t val;
					val = (uint16_t) spiReadData() << 8;
					val += spiReadData();
					failsafePPM[i] = servoUs2Bits(val);
				}
				rxb = 'G';
				failsafeSave();
				tx_packet(&rxb, 1);
			} else if (rxb == 'G') {
				failsafePPM[0] = 0xffff;
				failsafeSave();
				rxb = 'G';
				tx_packet(&rxb, 1);
			}
			RF_Mode = Receive;
			rx_reset();
		}
	}
	return 0;
}

int8_t checkIfConnected(uint8_t pin1, uint8_t pin2) {
	int8_t ret = 0;
	pinMode(pin1, OUTPUT);
	digitalWrite(pin1, 1);
	digitalWrite(pin2, 1);
	delayMicroseconds(10);

	if (digitalRead(pin2)) {
		digitalWrite(pin1, 0);
		delayMicroseconds(10);

		if (!digitalRead(pin2)) {
			ret = 1;
		}
	}

	pinMode(pin1, INPUT);
	digitalWrite(pin1, 0);
	digitalWrite(pin2, 0);
	return ret;
}

uint8_t rx_buf[21]; // RX buffer (uplink)
// First byte of RX buf is
// MSB..LSB [1bit uplink seqno.] [1bit downlink seqno] [6bits type)
// type 0x00 normal servo, 0x01 failsafe set
// type 0x38..0x3f uplinkked serial data

uint8_t tx_buf[COM_BUF_MAXSIZE]; // TX buffer (downlink)(1 byte reserved for transmit flags and data length)
// First byte is meta
// MSB..LSB [1 bit uplink seq] [1bit downlink seqno] [6b telemtype]
// 0x00 link info [RSSI] [AFCC]*2 etc...
// type 0x38-0x3f downlink serial data 1-COM_BUF_MAXSIZE bytes

#define SERIAL_BUF_RX_SIZE 256
#define SERIAL_BUF_TX_SIZE 64
uint8_t serial_rxbuffer[SERIAL_BUF_RX_SIZE];
uint8_t serial_txbuffer[SERIAL_BUF_TX_SIZE];

uint8_t hopcount;

uint8_t slaveAct = 0;
uint8_t slaveCnt = 0;

uint8_t slaveHandler(uint8_t *data, uint8_t flags) {
	if (flags & MYI2C_SLAVE_ISTX) {
		if (flags & MYI2C_SLAVE_ISFIRST) {
			*data = slaveState;
			slaveCnt = 0;
		} else {
			if (slaveCnt < getPacketSize(&bind_data)) {
				*data = rx_buf[slaveCnt++];
			} else {
				return 0;
			}
		}
	} else {
		if (flags & MYI2C_SLAVE_ISFIRST) {
			slaveAct = *data;
			slaveCnt = 0;
			if ((slaveAct & 0xe0) == 0x60) {
				if (slaveState >= 2) {
					RF_channel = (*data & 0x1f);
					slaveState = 3; // to RX mode
				}
				return 0;
			} else if (slaveAct == 0xfe) {
				// deinitialize
				slaveState = 0;
				return 0;
			}
		} else {
			if (slaveAct == 0xff) {
				// load bind_data
				if (slaveCnt < sizeof(bind_data)) {
					((uint8_t *) (&bind_data))[slaveCnt++] = *data;
					if (slaveCnt == sizeof(bind_data)) {
						slaveState = 1;
						return 0;
					}
				} else {
					return 0;
				}
			}
		}
	}
	return 1;
}

void slaveLoop() {
	myI2C_slaveSetup(32, 0, 0, slaveHandler);
	slaveState = 0;
	while (1) {
		if (slaveState == 1) {
			init_rfm(0); // Configure the RFM22B's registers for normal operation
			slaveState = 2; // BIND applied
			Red_LED_OFF;
		} else if (slaveState == 3) {
			Green_LED_OFF;
			rfmSetChannel(RF_channel);
			RF_Mode = Receive;
			rx_reset();
			slaveState = 4; // in RX mode
		} else if (slaveState == 4) {
			if (RF_Mode == Received) {
				spiSendAddress(0x7f);   // Send the package read command
				for (int16_t i = 0; i < getPacketSize(&bind_data); i++) {
					rx_buf[i] = spiReadData();
				}
				slaveState = 5;
				Green_LED_ON;
			}
		}
	}
}

void reinitSlave() {
	uint8_t ret, buf[sizeof(bind_data) + 1];
	buf[0] = 0xff;
	memcpy(buf + 1, &bind_data, sizeof(bind_data));
	ret = myI2C_writeTo(32, buf, sizeof(bind_data) + 1, MYI2C_WAIT);
	if (ret == 0) {
		ret = myI2C_readFrom(32, buf, 1, MYI2C_WAIT);
		if ((ret == 0)) {
			slaveState = 2;
		} else {
			slaveState = 255;
		}
	} else {
		slaveState = 255;
	}
	if (slaveState == 2) {
	} else {
		slaveFailedMs = millis();
	}
}

/////////////////////////////////////////////

void setup() {
/*
  buzzerInit();
        buzzerOff();

        updateLBeep(false);
  */
    Serial.begin(57600);
    Serial.println("oko");  
  	watchdogConfig (WATCHDOG_OFF);


	//LEDs
	pinMode(Green_LED, OUTPUT);
	pinMode(Red_LED, OUTPUT);

	setupSPI();

#ifdef SDN_pin
	pinMode(SDN_pin, OUTPUT);  //SDN
	digitalWrite(SDN_pin, 0);
#endif

	pinMode(0, INPUT);   // Serial Rx
	pinMode(1, OUTPUT);  // Serial Tx

// ELT
//  Serial.setBuffers(serial_rxbuffer, SERIAL_BUF_RX_SIZE, serial_txbuffer, SERIAL_BUF_TX_SIZE);
//	Serial.begin(115200);
//  Serial.begin(57600);
//	mavlink_comm_0_port = &Serial;
//	mavlink_comm_1_port = &Serial;

//	rxReadEeprom();

//  failsafeLoad(); // ELT

	Serial.print("DroneELT RX starting ");
//	printVersion (version);
	Serial.print(" on HW ");
	Serial.println(BOARD_TYPE);

//	rxReadEeprom();
	setupRfmInterrupt();

	sei();
	Red_LED_ON;
	Green_LED_ON;
   
	Serial.flush();

//      beacon_send_number(7, 2, 2, 2);

//  beacon_send_prelude(6);

           setupOutputs();   // lepiej nie bo piszczy!
	watchdogConfig (WATCHDOG_2S);
return;
	//beacon_initialize_audio();
	return;
	/*  
	 while(1){
	 // beacon_send_number(7, 2, 2, 2);

	 //     if(millis() > mavLinkTimer + 120){
	 //        mavLinkTimer = millis();
	 ///       OnMavlinkTimer();
	 //   }

	 read_mavlink();
	 if(mavlink_active==1)
	 beacon_send_prelude(1);
	 else
	 beacon_send_prelude(3);
	 
	 delay(500);  
	 }
	 */
//  beacon_send_number(7, 2, 2, 2);

        ////////////////////////// 2<->3 - scannerMode
	if (checkIfConnected(OUTPUT_PIN[2], OUTPUT_PIN[3])) { // ch1 - ch2 --> force scannerMode
		while (1) {
			Red_LED_OFF;
			Green_LED_OFF;
			scannerMode();
		}
	}

        ////////////////////////////// 0<->1
        /*
	if (checkIfConnected(OUTPUT_PIN[0], OUTPUT_PIN[1]) || (!bindReadEeprom())) {
		Serial.print(
				"EEPROM data not valid or bind jumpper set, forcing bind\n");

		if (bindReceive(0)) {
			bindWriteEeprom();
			Serial.println("Saved bind data to EEPROM\n");
			Green_LED_ON;
		}
		setupOutputs();
	} else {
		setupOutputs();

		if ((rx_config.pinMapping[SDA_OUTPUT] != PINMAP_SDA)
				|| (rx_config.pinMapping[SCL_OUTPUT] != PINMAP_SCL)) {
			rx_config.flags &= ~SLAVE_MODE;
		}

		if ((rx_config.flags & ALWAYS_BIND)
				&& (!(rx_config.flags & SLAVE_MODE))) {
			if (bindReceive(500)) {
				bindWriteEeprom();
				Serial.println("Saved bind data to EEPROM\n");
				setupOutputs(); // parameters may have changed
				Green_LED_ON;
   			}
		}
	}
        */
	if ((rx_config.pinMapping[SDA_OUTPUT] == PINMAP_SDA)
			&& (rx_config.pinMapping[SCL_OUTPUT] == PINMAP_SCL)) {
		myI2C_init(1);
		if (rx_config.flags & SLAVE_MODE) {
			Serial.println("I am slave");
			slaveLoop();
		} else {
			uint8_t ret, buf;
			delay(20);
			ret = myI2C_readFrom(32, &buf, 1, MYI2C_WAIT);
			if (ret == 0) {
				slaveState = 1;
			}
		}
	}

	Serial.print("Entering normal mode");

	watchdogConfig(WATCHDOG_2S);

	init_rfm(0);   // Configure the RFM22B's registers for normal operation
	RF_channel = 0;
	rfmSetChannel(RF_channel);

	// Count hopchannels as we need it later
	hopcount = 0;
	while ((hopcount < MAXHOPS) && (bind_data.hopchannel[hopcount] != 0)) {
		hopcount++;
	}

	//################### RX SYNC AT STARTUP #################
	RF_Mode = Receive;
	to_rx_mode();

	if (slaveState) {
		reinitSlave();
	}

	if ((rx_config.pinMapping[TXD_OUTPUT] == PINMAP_SPKTRM)
			|| (rx_config.pinMapping[TXD_OUTPUT] == PINMAP_SUMD)) {
		Serial.begin(115200);
	} else if (rx_config.pinMapping[TXD_OUTPUT] == PINMAP_SBUS) {
		Serial.begin(100000);
		UCSR0C |= 1 << UPM01; // set even parity
	} else if ((bind_data.flags & TELEMETRY_MASK) == TELEMETRY_FRSKY) {
		Serial.begin(9600);
	} else {
		if (bind_data.serial_baudrate < 10) {
			Serial.begin(9600);
		} else {
			Serial.begin(bind_data.serial_baudrate);
		}
	}

	Serial.flush();

	if (rx_config.pinMapping[RXD_OUTPUT] != PINMAP_RXD) {
		UCSR0B &= 0xEF; //disable serial RXD
	}
	if ((rx_config.pinMapping[TXD_OUTPUT] != PINMAP_TXD)
			&& (rx_config.pinMapping[TXD_OUTPUT] != PINMAP_SUMD)
			&& (rx_config.pinMapping[TXD_OUTPUT] != PINMAP_SBUS)
			&& (rx_config.pinMapping[TXD_OUTPUT] != PINMAP_SPKTRM)) {
		UCSR0B &= 0xF7; //disable serial TXD
	}

	linkAcquired = 0;
	lastPacketTimeUs = micros();

	mavlinkIncomingFrame.Reset();
}

/////////////////////////////////////////////////////////////////////////////////////////////

void slaveHop() {
	if (slaveState == 2) {
		uint8_t buf;
		buf = 0x60 + RF_channel;
		if (myI2C_writeTo(32, &buf, 1, MYI2C_WAIT)) {
			slaveState = 255;
			slaveFailedMs = millis();
		}
	}
}

// Return slave state or 255 in case of error
uint8_t readSlaveState() {
	uint8_t ret = 255, buf;
	if (slaveState == 2) {
		ret = myI2C_readFrom(32, &buf, 1, MYI2C_WAIT);
		if (ret) {
			slaveState = 255;
			slaveFailedMs = millis();
			ret = 255;
		} else {
			ret = buf;
		}
	}
	return ret;
}

//#define SLAVE_STATISTICS
#ifdef SLAVE_STATISTICS
uint16_t rxBoth = 0;
uint16_t rxSlave = 0;
uint16_t rxMaster = 0;
uint32_t rxStatsMs = 0;
#endif

//############ MAIN LOOP ##############
void loop() {
	uint32_t timeUs, timeMs;
        float mavLinkTimer = 0;

//        updateLBeep(false);
//        buzzerOff();
         read_mavlink();

//  watchdogReset();

//	 beacon_send_number(7, 2, 2, 2);
//              read_mavlink();
	     if(millis() > mavLinkTimer + 100){
              mavLinkTimer = millis();
///       OnMavlinkTimer();
                
//                read_mavlink();
//                Serial.flush();
	   }

	
	 if(millis() > lastLED + 1000){
	 lastLED = millis();
/*
	 beacon_initialize_audio();
	 beacon_tone(840,5);
	 watchdogReset();
	 beacon_finish_audio();
*/
//	 Serial.println("ee");
//        Serial.println(osd_roll);	 
	 if(stateLED == true){
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

	} else {
//           beacon
		//           beacon_tone(840,2);
		//          watchdogReset();
		//              delay(10);
                Serial.println("NO Mavlink");
               beacon_send_number(7, 2, 2, 2);
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
//          delay();  
//while(!Serial.available())
//{
//}

	//        read_mavlink();

//	delay(500);
//	return;

//  beacon_send(false);
//  beacon_send_number(7, 2, 2, 2);

////////////////////// REBOOT IF RC MODULE LOCKED //////////////////////////

	if (spiReadRegister(0x0C) == 0) {     // detect the locked module and reboot
		Serial.println("RX hang");
		init_rfm(0);
		to_rx_mode();
	}

	timeUs = micros();

///////////////////// I'M SLAVE ////////////////////////////////////////////

	uint8_t slaveReceived = 0;
	if (5 == readSlaveState()) {
		slaveReceived = 1;
	}

/////////////////// COS PRZYSZLO? KOPIUJ DO rx_buf /////////////////


	retry: if ((RF_Mode == Received) || (slaveReceived)) {
		uint32_t timeTemp = micros();

		if (RF_Mode == Received) {
			spiSendAddress(0x7f);   // Send the package read command

			for (int16_t i = 0; i < getPacketSize(&bind_data); i++) {
				rx_buf[i] = spiReadData();
			}

			lastAFCCvalue = rfmGetAFCC();
			Green_LED_ON;
		} else {
			uint8_t ret, slave_buf[22];
			ret = myI2C_readFrom(32, slave_buf, getPacketSize(&bind_data) + 1,
					MYI2C_WAIT);
			if (ret) {
				slaveState = 255;
				slaveFailedMs = millis();
				slaveReceived = 0;
				goto retry;
				//slave failed when reading packet...
			} else {
				memcpy(rx_buf, slave_buf + 1, getPacketSize(&bind_data));
			}
		}

/////////////////////////////////////////////////////////////////////////////

                lastPacketTimeUs = timeTemp; // used saved timestamp to avoid skew by I2C
		numberOfLostPackets = 0;
		linkQuality <<= 1;
		linkQuality |= 1;
		Red_LED_OFF;

		updateLBeep(false);

///////////////////////////TELEM MAVLINK /////////////////////////////////

		if ((bind_data.flags & TELEMETRY_MASK) == TELEMETRY_MAVLINK
				&& mavlinkIncomingFrame.IsIdle()
				&& timeUs - mavlink_last_inject_time
						> MAVLINK_INJECT_INTERVAL) {

			const uint8_t space = serial_space(Serial.available(),
			SERIAL_BUF_RX_SIZE);
			MAVLink_report(space, 0, smoothRSSI, rxerrors);
			mavlink_last_inject_time = timeUs;
		}

//////////////////////////////SLAVE_STATISTICS///////////////////////////////////////////////////////////////////

#ifdef SLAVE_STATISTICS
		if (5 == readSlaveState()) {
			if (RF_Mode == Received) {
				rxBoth++;
			} else {
				rxSlave++;
			}
		} else {
			rxMaster++;
		}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////// ALBO DANE DO SERW /////////////////////////

		if ((rx_buf[0] & 0x3e) == 0x00) {
			cli();
			unpackChannels(bind_data.flags & 7, PPM, rx_buf + 1);
#ifdef DEBUG_DUMP_PPM
			for (uint8_t i = 0; i < 8; i++) {
				Serial.print(PPM[i]);
				Serial.print(',');
			}
			Serial.println();
#endif
			set_PPM_rssi();
			sei();
			if (rx_buf[0] & 0x01) {
				if (!fs_saved) {
					for (int16_t i = 0; i < PPM_CHANNELS; i++) {
						failsafePPM[i] = PPM[i];
					}
					failsafeSave();
					fs_saved = 1;
				}
			} else if (fs_saved) {
				fs_saved = 0;
			}
		} else { ////////////////////// ALBO COŚ PRZYSZLO PO TELEMETRII////////////////////
			// something else than servo data...
			if ((rx_buf[0] & 0x38) == 0x38) {
				if ((rx_buf[0] ^ tx_buf[0]) & 0x80) {
					// We got new data... (not retransmission)
					uint8_t i;
					tx_buf[0] ^= 0x80; // signal that we got it

					if (rx_config.pinMapping[TXD_OUTPUT] == PINMAP_TXD) {

						if ((bind_data.flags & TELEMETRY_MASK)
								== TELEMETRY_MAVLINK) {

							for (i = 0; i <= (rx_buf[0] & 7);) {
								i++;
								const uint8_t ch = rx_buf[i];
								Serial.write(ch);
								mavlinkIncomingFrame.Parse(ch);
							}
						} else {
							for (i = 0; i <= (rx_buf[0] & 7);) {
								i++;
								Serial.write(rx_buf[i]);
							}
						}
					}
				}
			}
		}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (linkAcquired == 0) {
			linkAcquired = 1;
		}
		failsafeActive = 0;
		disablePWM = 0;
		disablePPM = 0;

		Serial.write('g');

//////////////////////////////// JESLI TELEMETRIA //////////////////////////////////////////////////////////////////////

		if (bind_data.flags & TELEMETRY_MASK) {
			if ((tx_buf[0] ^ rx_buf[0]) & 0x40) {
				// resend last message
			} else {
				tx_buf[0] &= 0xc0;
				tx_buf[0] ^= 0x40; // swap sequence as we have new data

				if (true) {

					/*
					 while(Serial.available() > 0) { 
					 
					 uint8_t c = Serial.read();
					 if( c == 'q' ){
					 oko = true;
					 }
					 }
					 */
					uint8_t bytes = 0;
					while ((bytes < 8)) {
						bytes++;
						//tx_buf[bytes] = bytes+'c'; // OK, dziala :)
						if (bytes == 1)
							tx_buf[bytes] = (osd_roll / 100) + '0';
						else if (bytes == 2 & oko)
							tx_buf[bytes] = 2 + '0';
						else if (bytes == 2)
							tx_buf[bytes] = 0 + '0';
						else if (bytes == 3)
							tx_buf[bytes] = (osd_roll % 10) + '0';
						else if (bytes == 4)
							tx_buf[bytes] = mavlink_active + '0';
						else if (bytes == 5)
							tx_buf[bytes] = mavbeat + '0';
						else
							tx_buf[bytes] = 'P';

					}
					tx_buf[0] |= (0x37 + bytes);

					//        mavlink_comm_0_port = &Serial;
					read_mavlink();
				}

////////////////////////////////////////////////////////////////////////////////////////////////////////////     

				/* // elt
				 if((bind_data.flags & TELEMETRY_MASK) == TELEMETRY_MAVLINK) {
				 uint8_t bytes = 0;
				 while ((bytes < bind_data.serial_downlink - 1) && Serial.available() > 0) {
				 bytes++;
				 const uint8_t ch = Serial.read();
				 tx_buf[bytes] = ch;
				 }
				 tx_buf[0] |= (0x3F & bytes);
				 } else {
				 if (Serial.available() > 0) {
				 uint8_t bytes = 0;
				 while ((bytes < bind_data.serial_downlink - 1) && Serial.available() > 0) {
				 bytes++;
				 tx_buf[bytes] = Serial.read();
				 }
				 tx_buf[0] |= (0x37 & bytes);
				 } else {
				 // tx_buf[0] lowest 6 bits left at 0
				 tx_buf[1] = lastRSSIvalue;

				 if (rx_config.pinMapping[ANALOG0_OUTPUT] == PINMAP_ANALOG) {
				 tx_buf[2] = analogRead(OUTPUT_PIN[ANALOG0_OUTPUT]) >> 2;
				 #ifdef ANALOG0_OUTPUT_ALT
				 } else if (rx_config.pinMapping[ANALOG0_OUTPUT_ALT] == PINMAP_ANALOG) {
				 tx_buf[2] = analogRead(OUTPUT_PIN[ANALOG0_OUTPUT_ALT]) >> 2;
				 #endif
				 } else {
				 tx_buf[2] = 0;
				 }

				 if (rx_config.pinMapping[ANALOG1_OUTPUT] == PINMAP_ANALOG) {
				 tx_buf[3] = analogRead(OUTPUT_PIN[ANALOG1_OUTPUT]) >> 2;
				 #ifdef ANALOG1_OUTPUT_ALT
				 } else if (rx_config.pinMapping[ANALOG1_OUTPUT_ALT] == PINMAP_ANALOG) {
				 tx_buf[3] = analogRead(OUTPUT_PIN[ANALOG1_OUTPUT_ALT]) >> 2;
				 #endif
				 } else {
				 tx_buf[3] = 0;
				 }
				 tx_buf[4] = (lastAFCCvalue >> 8);
				 tx_buf[5] = lastAFCCvalue & 0xff;
				 tx_buf[6] = countSetBits(linkQuality & 0x7fff);
				 } // else (no serial data available, send misc stuff).
				 }
				 */
			} // else then ((tx_buf[0] ^ rx_buf[0]) & 0x40) { 

///////////////////////////////////////////////////////////////////////////      

#ifdef TEST_NO_ACK_BY_CH1
			if (PPM[0]<900) {
				tx_packet_async(tx_buf, bind_data.serial_downlink);
				while(!tx_done()) {
				}
			}
#else
			tx_packet_async(tx_buf, bind_data.serial_downlink);

///////////////////////////////////////////////////////// CZEKAMY NA PRZESLANIE.../////////////////////////

			while (!tx_done()) {
			}
#endif

////////////////////////////////////////////////////// TEST - EXIT BY CH2 /////////////////////////////

#ifdef TEST_HALT_RX_BY_CH2
			if (PPM[1]>1013) {
				fatalBlink(3);
			}
#endif

			updateSwitches();

		} // if (bind_data.flags & TELEMETRY_MASK) {

  
  
		RF_Mode = Receive;
		rx_reset();

		willhop = 1;

		Green_LED_OFF;
	} // retry: if ((RF_Mode == Received) || (slaveReceived)) {


  
        timeUs = micros();
        timeMs = millis();

	// sample RSSI when packet is in the 'air'
	if ((numberOfLostPackets < 2) && (lastRSSITimeUs != lastPacketTimeUs)
			&& (timeUs - lastPacketTimeUs) > (getInterval(&bind_data) - 1500)) {
		lastRSSITimeUs = lastPacketTimeUs;
		lastRSSIvalue = rfmGetRSSI(); // Read the RSSI value
		RSSI_sum += lastRSSIvalue;    // tally up for average
		RSSI_count++;

		if (RSSI_count > 8) {
			RSSI_sum /= RSSI_count;
			smoothRSSI = (((uint16_t) smoothRSSI * 3 + (uint16_t) RSSI_sum * 1)
					/ 4);
			set_RSSI_output();
			RSSI_sum = 0;
			RSSI_count = 0;
		}

	} // if((numberOfLostPackets < 2) && (lastRSSITimeUs != lastPacketTimeUs)...


//        read_mavlink();

/////////////////////////////// MAMY LINK ////////////////////////////////////////////////////////////////////////////////////////////////

	if (linkAcquired) {
		if ((numberOfLostPackets < hopcount)
				&& ((timeUs - lastPacketTimeUs)
						> (getInterval(&bind_data) + 1000))) {
			// we lost packet, hop to next channel
			linkQuality <<= 1;
			willhop = 1;
			if (numberOfLostPackets == 0) {
				linkLossTimeMs = timeMs;
				lastBeaconTimeMs = 0;
				rxerrors++;
			}
			numberOfLostPackets++;
			lastPacketTimeUs += getInterval(&bind_data);
			willhop = 1;
			Red_LED_ON;
			updateLBeep(true);
			set_RSSI_output();
		} else if ((numberOfLostPackets == hopcount)
				&& ((timeUs - lastPacketTimeUs)
						> (getInterval(&bind_data) * hopcount))) {
			// hop slowly to allow resync with TX
			linkQuality = 0;
			willhop = 1;
			smoothRSSI = 0;
			set_RSSI_output();
			lastPacketTimeUs = timeUs;
		}

		if (numberOfLostPackets) {
			if (rx_config.failsafeDelay && (!failsafeActive)
					&& ((timeMs - linkLossTimeMs)
							> delayInMs(rx_config.failsafeDelay))) {
				failsafeActive = 1;
				failsafeApply();
				lastBeaconTimeMs = (timeMs
						+ delayInMsLong(rx_config.beacon_deadtime)) | 1; //beacon activating...
			}
			if (rx_config.pwmStopDelay && (!disablePWM)
					&& ((timeMs - linkLossTimeMs)
							> delayInMs(rx_config.pwmStopDelay))) {
				disablePWM = 1;
			}
			if (rx_config.ppmStopDelay && (!disablePPM)
					&& ((timeMs - linkLossTimeMs)
							> delayInMs(rx_config.ppmStopDelay))) {
				disablePPM = 1;
			}

			if ((rx_config.beacon_frequency) && (lastBeaconTimeMs)) {
				if (((timeMs - lastBeaconTimeMs) < 0x80000000) && // last beacon is future during deadtime
						(timeMs - lastBeaconTimeMs)
								> (1000UL * rx_config.beacon_interval)) {
					beacon_send((rx_config.flags & STATIC_BEACON));
					init_rfm(0);   // go back to normal RX
					rx_reset();
					lastBeaconTimeMs = millis() | 1; // avoid 0 in time
				}
			}
		}
	} else {
		// NIE MA JESZCZE LINKU///////////////////////////

		// Waiting for first packet, hop slowly
		if ((timeUs - lastPacketTimeUs)
				> (getInterval(&bind_data) * hopcount)) {
			lastPacketTimeUs = timeUs;
			willhop = 1;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////// POSLIJ PPM ///////////////////////////////////////
	if (!disablePPM) {
		if (rx_config.pinMapping[TXD_OUTPUT] == PINMAP_SPKTRM) {
			sendSpektrumFrame();
		} else if (rx_config.pinMapping[TXD_OUTPUT] == PINMAP_SBUS) {
			sendSBUSFrame(failsafeActive, numberOfLostPackets);
		} else if (rx_config.pinMapping[TXD_OUTPUT] == PINMAP_SUMD) {
			sendSUMDFrame(failsafeActive);
		}
	}

//////////////////////////// HOPPING HOP! /////////////////

	if (willhop == 1) {
		RF_channel++;

		if ((RF_channel == MAXHOPS)
				|| (bind_data.hopchannel[RF_channel] == 0)) {
			RF_channel = 0;
		}
		rfmSetChannel(RF_channel);
		slaveHop();
		willhop = 0;
	}

///////////////////////////////////////////////////////////////

//////////////////// REININT SLAVE?

	if ((slaveState == 255) && ((millis() - slaveFailedMs) > 1000)) {
		slaveFailedMs = millis();
		reinitSlave();
	}

///////////////////////////////////////////

#ifdef SLAVE_STATISTICS
	if ((millis() - rxStatsMs) > 5000) {
		rxStatsMs = millis();
		Serial.print(rxBoth);
		Serial.print(',');
		Serial.print(rxMaster);
		Serial.print(',');
		Serial.println(rxSlave);
		rxBoth = rxMaster = rxSlave = 0;
	}
#endif
}
