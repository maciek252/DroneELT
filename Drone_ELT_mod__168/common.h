//####### COMMON FUNCTIONS #########

//#include "LEDs.h"
extern void mavlinkLedOn();
extern void mavlinkLedOff();
extern void gpsLedOn();
extern void gpsLedOff();



void rfmSetCarrierFrequency(uint32_t f);
void rfmSetPower(uint8_t p);
uint8_t rfmGetRSSI(void);
void RF22B_init_parameter(void);
uint8_t spiReadRegister(uint8_t address);
void spiWriteRegister(uint8_t address, uint8_t data);
void tx_packet(uint8_t* pkt, uint8_t size);
void to_rx_mode(void);

#define NOP() __asm__ __volatile__("nop")

#define RF22B_PWRSTATE_POWERDOWN    0x00
#define RF22B_PWRSTATE_READY        0x01
#define RF22B_PACKET_SENT_INTERRUPT 0x04
#define RF22B_PWRSTATE_RX           0x05
#define RF22B_PWRSTATE_TX           0x09

#define RF22B_Rx_packet_received_interrupt   0x02

uint8_t ItStatus1, ItStatus2;

void spiWriteBit(uint8_t b);

void spiSendCommand(uint8_t command);
void spiSendAddress(uint8_t i);
uint8_t spiReadData(void);
void spiWriteData(uint8_t i);

void to_sleep_mode(void);
void rx_reset(void);

// **** SPI bit banging functions

void spiWriteBit(uint8_t b) {
	if (b) {
		SCK_off;
		NOP();
		SDI_on;
		NOP();
		SCK_on;
		NOP();
	} else {
		SCK_off;
		NOP();
		SDI_off;
		NOP();
		SCK_on;
		NOP();
	}
}

uint8_t spiReadBit(void) {
	uint8_t r = 0;
	SCK_on;
	NOP();

	if (SDO_1) {
		r = 1;
	}

	SCK_off;
	NOP();
	return r;
}

void spiSendCommand(uint8_t command) {
	nSEL_on;
	SCK_off;
	nSEL_off;

	for (uint8_t n = 0; n < 8; n++) {
		spiWriteBit(command & 0x80);
		command = command << 1;
	}

	SCK_off;
}

void spiSendAddress(uint8_t i) {
	spiSendCommand(i & 0x7f);
}

void spiWriteData(uint8_t i) {
	for (uint8_t n = 0; n < 8; n++) {
		spiWriteBit(i & 0x80);
		i = i << 1;
	}

	SCK_off;
}

uint8_t spiReadData(void) {
	uint8_t Result = 0;
	SCK_off;

	for (uint8_t i = 0; i < 8; i++) {   //read fifo data byte
		Result = (Result << 1) + spiReadBit();
	}

	return (Result);
}

uint8_t spiReadRegister(uint8_t address) {
	uint8_t result;
	spiSendAddress(address);
	result = spiReadData();
	nSEL_on;
	return (result);
}

void spiWriteRegister(uint8_t address, uint8_t data) {
	address |= 0x80; //
	spiSendCommand(address);
	spiWriteData(data);
	nSEL_on;
}

// **** RFM22 access functions

void rfmSetChannel(uint8_t ch) {
	uint8_t magicLSB = (bind_data.rf_magic & 0xff) ^ ch;
	spiWriteRegister(0x79, bind_data.hopchannel[ch]);
	spiWriteRegister(0x3a + 3, magicLSB);
	spiWriteRegister(0x3f + 3, magicLSB);
}

uint8_t rfmGetRSSI(void) {
	return spiReadRegister(0x26);
}

uint16_t rfmGetAFCC(void) {
	return (((uint16_t) spiReadRegister(0x2B) << 2)
			| ((uint16_t) spiReadRegister(0x2C) >> 6));
}

void setModemRegs(struct rfm22_modem_regs* r) {

	spiWriteRegister(0x1c, r->r_1c);
	spiWriteRegister(0x1d, r->r_1d);
	spiWriteRegister(0x1e, r->r_1e);
	spiWriteRegister(0x20, r->r_20);
	spiWriteRegister(0x21, r->r_21);
	spiWriteRegister(0x22, r->r_22);
	spiWriteRegister(0x23, r->r_23);
	spiWriteRegister(0x24, r->r_24);
	spiWriteRegister(0x25, r->r_25);
	spiWriteRegister(0x2a, r->r_2a);
	spiWriteRegister(0x6e, r->r_6e);
	spiWriteRegister(0x6f, r->r_6f);
	spiWriteRegister(0x70, r->r_70);
	spiWriteRegister(0x71, r->r_71);
	spiWriteRegister(0x72, r->r_72);
}

void rfmSetCarrierFrequency(uint32_t f) {
	uint16_t fb, fc, hbsel;
	if (f < 480000000) {
		hbsel = 0;
		fb = f / 10000000 - 24;
		fc = (f - (fb + 24) * 10000000) * 4 / 625;
	} else {
		hbsel = 1;
		fb = f / 20000000 - 24;
		fc = (f - (fb + 24) * 20000000) * 2 / 625;
	}
	spiWriteRegister(0x75, 0x40 + (hbsel ? 0x20 : 0) + (fb & 0x1f));
	spiWriteRegister(0x76, (fc >> 8));
	spiWriteRegister(0x77, (fc & 0xff));
}

void rfmSetPower(uint8_t p) {
	spiWriteRegister(0x6d, p);
}

void init_rfm(uint8_t isbind) {
#ifdef SDN_pin
	digitalWrite(SDN_pin, 1);
	delay(50);
	digitalWrite(SDN_pin, 0);
	delay(50);
#endif

	ItStatus1 = spiReadRegister(0x03);   // read status, clear interrupt
	ItStatus2 = spiReadRegister(0x04);
	spiWriteRegister(0x06, 0x00);    // disable interrupts
	spiWriteRegister(0x07, RF22B_PWRSTATE_READY); // disable lbd, wakeup timer, use internal 32768,xton = 1; in ready mode
	spiWriteRegister(0x09, 0x7f);   // c = 12.5p
	spiWriteRegister(0x0a, 0x05);
#ifdef SWAP_GPIOS
	spiWriteRegister(0x0b, 0x15);    // gpio0 RX State
	spiWriteRegister(0x0c, 0x12);// gpio1 TX State
#else
	spiWriteRegister(0x0b, 0x12);    // gpio0 TX State
	spiWriteRegister(0x0c, 0x15);    // gpio1 RX State
#endif
	spiWriteRegister(0x0d, 0xfd);    // gpio 2 micro-controller clk output
	spiWriteRegister(0x0e, 0x00);    // gpio    0, 1,2 NO OTHER FUNCTION.

	if (isbind) {
		setModemRegs(&bind_params);
	} else {
		setModemRegs(&modem_params[bind_data.modem_params]);
	}

	// Packet settings
	spiWriteRegister(0x30, 0x8c); // enable packet handler, msb first, enable crc,
	spiWriteRegister(0x32, 0x0f);    // no broadcast, check header bytes 3,2,1,0
	spiWriteRegister(0x33, 0x42); // 4 byte header, 2 byte synch, variable pkt size
	spiWriteRegister(0x34, 0x0a);    // 10 nibbles (40 bit preamble)
	spiWriteRegister(0x35, 0x2a);    // preath = 5 (20bits), rssioff = 2
	spiWriteRegister(0x36, 0x2d);    // synchronize word 3
	spiWriteRegister(0x37, 0xd4);    // synchronize word 2
	spiWriteRegister(0x38, 0x00);    // synch word 1 (not used)
	spiWriteRegister(0x39, 0x00);    // synch word 0 (not used)

	uint32_t magic = isbind ? BIND_MAGIC : bind_data.rf_magic;
	for (uint8_t i = 0; i < 4; i++) {
		spiWriteRegister(0x3a + i, (magic >> 24) & 0xff);   // tx header
		spiWriteRegister(0x3f + i, (magic >> 24) & 0xff);   // rx header
		magic = magic << 8; // advance to next byte
	}

	spiWriteRegister(0x43, 0xff);    // all the bit to be checked
	spiWriteRegister(0x44, 0xff);    // all the bit to be checked
	spiWriteRegister(0x45, 0xff);    // all the bit to be checked
	spiWriteRegister(0x46, 0xff);    // all the bit to be checked

	if (isbind) {
		spiWriteRegister(0x6d, BINDING_POWER);
	} else {
		spiWriteRegister(0x6d, bind_data.rf_power);
	}

	spiWriteRegister(0x79, 0);

	spiWriteRegister(0x7a, bind_data.rf_channel_spacing);   // channel spacing

	spiWriteRegister(0x73, 0x00);
	spiWriteRegister(0x74, 0x00);    // no offset

	rfmSetCarrierFrequency(isbind ? BINDING_FREQUENCY : bind_data.rf_frequency);

}

void rx_reset(void) {
	spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
	spiWriteRegister(0x7e, 36); // threshold for rx almost full, interrupt when 1 byte received
	spiWriteRegister(0x08, 0x03);    //clear fifo disable multi packet
	spiWriteRegister(0x08, 0x00);    // clear fifo, disable multi packet
	spiWriteRegister(0x07, RF22B_PWRSTATE_RX);   // to rx mode
	spiWriteRegister(0x05, RF22B_Rx_packet_received_interrupt);
	ItStatus1 = spiReadRegister(0x03);   //read the Interrupt Status1 register
	ItStatus2 = spiReadRegister(0x04);
}

uint32_t tx_start = 0;


void beacon_tone(int16_t hz, int16_t len) //duration is now in 0.1 seconds.
		{
	int16_t d = 500000 / hz; // better resolution

	if (d < 1) {
		d = 1;
	}

	int16_t cycles = (len * 100000 / d);

	for (int16_t i = 0; i < cycles; i++) {
		SDI_on;
		delayMicroseconds(d);
		SDI_off;
		delayMicroseconds(d);
	}
}

void beacon_send_prelude(int8_t numOfSounds) {

	for (int8_t i = 0; i < numOfSounds; i++) {

		beacon_tone(1040, 2);
		watchdogReset();
		delayMicroseconds(500);
	}
}

void beacon_short_sound() {

	//mavlinkLedOn();
	//gpsLedOff();
	beacon_tone(540, 5);
	watchdogReset();
	delayMicroseconds(1000);
	//mavlinkLedOff();


}

void beacon_long_sound() {

	//mavlinkLedOn();
	//gpsLedOn();

	beacon_tone(350, 5);
	watchdogReset();
	delayMicroseconds(1000);

	//mavlinkLedOff();
	//gpsLedOff();
}

void beacon_send_digit(int8_t numm) {
// long - 0
// short - 1

	bool playBetween = false;

	int delayInsideDigit = 100;

	delay(1000);
	delay(delayInsideDigit);
	if (numm == 0) {
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();

	} else if (numm == 1) {
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_short_sound();

	} else if (numm == 2) {
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();

	} else if (numm == 3) {

		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_short_sound();

	} else if (numm == 4) {

		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();

	} else if (numm == 5) {
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_short_sound();

	} else if (numm == 6) {
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();

	} else if (numm == 7) {
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_short_sound();

	} else if (numm == 8) {
		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();

	} else if (numm == 9) {
		beacon_short_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);

		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_long_sound();
		if (playBetween)
			beacon_send_prelude(1);
		delay(delayInsideDigit);
		beacon_short_sound();

	};

}

void beacon_send_digit2(int8_t numm) {


	int num = (int) numm;
	for (int i = 4; i >= 0; i--) {
		int dzielnik = (2 ^ i);
		if (i == 0)
			dzielnik = 1;
		int reszta = num / dzielnik;

		if (reszta == 0)
			beacon_short_sound();
		else
			beacon_long_sound();

		beacon_send_prelude(1);

		num = num % dzielnik;
	}

}

/*

 1I - between digits
 2I - beginning, end of the whole number
 3I - dot

 */

void beacon_send_double_num(double num, int numOfDigitsLeftOfDot,
		int numOfDigitsRightOfDot) {

	delay(1000);
//  num = num + 0.000001;
	if (numOfDigitsLeftOfDot >= 3) {
		int s100 = ((int) num % 1000) / 100;
		beacon_send_digit(s100);
		beacon_send_prelude(1);
	}
	if (numOfDigitsLeftOfDot >= 2) {
		int s10 = ((int) num % 100) / 10;
		beacon_send_digit(s10);
		beacon_send_prelude(1);
	}
	if (numOfDigitsLeftOfDot >= 1) {
		int s1 = ((int) num % 10) / 1;
		beacon_send_digit(s1);
		beacon_send_prelude(3);
	}

	// after the dot

	num = num - (uint16_t) num;

	if (numOfDigitsRightOfDot > 0) {
		uint16_t u1 = ((uint16_t)(num * 10.0)) % 10;
		beacon_send_digit(u1);
	}
	if (numOfDigitsRightOfDot > 1) {
		uint16_t u1 = ((uint16_t)(num * 100.0)) % 10;
		beacon_send_prelude(1);
		beacon_send_digit(u1);
	}
	if (numOfDigitsRightOfDot > 2) {
		uint16_t u1 = ((uint16_t)(num * 1000.0)) % 10;
		beacon_send_prelude(1);
		beacon_send_digit(u1);

	}
	if (numOfDigitsRightOfDot > 3) {
		uint16_t u1 = ((uint16_t)(num * 10000.0)) % 10;
		beacon_send_prelude(1);
		beacon_send_digit(u1);
	}
	if (numOfDigitsRightOfDot > 4) {
		uint16_t u1 = ((uint16_t)(num * 100000.0)) % 10;
		beacon_send_prelude(1);
		beacon_send_digit(u1);
	}
	if (numOfDigitsRightOfDot > 5) {
		uint16_t u1 = ((uint16_t)(num * 1000000.0)) % 10;
		beacon_send_prelude(1);
		beacon_send_digit(u1);
	}
	if (numOfDigitsRightOfDot > 6) {
		uint16_t u1 = ((uint16_t)(num * 10000000.0)) % 10;
		beacon_send_prelude(1);
		beacon_send_digit(u1);
	}

	/*

	 beacon_send_prelude(2);
	 uint16_t u10 = ((uint16_t)(num*100.0)) %10;
	 beacon_send_digit(u10);
	 beacon_send_prelude(2);
	 uint16_t u100 = ((uint16_t)(num*1000.0)) %10;
	 beacon_send_digit(u100);
	 beacon_send_prelude(2);
	 uint16_t u1000 = ((uint16_t)(num*10000.0)) %10;
	 beacon_send_digit(u1000);
	 beacon_send_prelude(2);
	 uint16_t u10000 = ((uint16_t)(num*100000.0)) %10;
	 beacon_send_digit(u10000);
	 beacon_send_prelude(2);
	 uint16_t u100000 = ((uint16_t)(num*1000000.0)) %10;
	 beacon_send_digit(u100000);
	 */

	/*
	 for( int i = 1; i <= numOfDigitsRightOfDot; i++){
	 beacon_send_prelude(2);
	 uint16_t u = ((uint16_t)(num*double(10^i))) %10;
	 beacon_send_digit(u);
	 }*/

//  int ulamek = (int)fmod(num,1)*10^5;
//  int u1000 = ((int)ulamek % 10000) / 1000;
	/*
	 beacon_send_digit(u1000);
	 beacon_send_prelude(2);
	 int u100 = ((int)ulamek % 1000) / 100;
	 beacon_send_digit(u100);
	 beacon_send_prelude(2);
	 int u10 = ((int)ulamek % 100) / 10;
	 beacon_send_digit(u10);
	 beacon_send_prelude(2);
	 */

	/*
	 for( int i =0; int < numOfDigitsLeftOfDot; i++){
	 int s = num / (10^i);

	 }
	 */

}

void beacon_finish_audio() {
	spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
	Green_LED_OFF;
}

void beacon_initialize_audio() {

//  init_rfm(1); // dla testu

	Green_LED_ON;
	Red_LED_ON;

	ItStatus1 = spiReadRegister(0x03);   // read status, clear interrupt
	ItStatus2 = spiReadRegister(0x04);
	spiWriteRegister(0x06, 0x00);    // no wakeup up, lbd,
	spiWriteRegister(0x07, RF22B_PWRSTATE_READY); // disable lbd, wakeup timer, use internal 32768,xton = 1; in ready mode
	spiWriteRegister(0x09, 0x7f);  // (default) c = 12.5p
	spiWriteRegister(0x0a, 0x05);
	spiWriteRegister(0x0b, 0x12);    // gpio0 TX State
	spiWriteRegister(0x0c, 0x15);    // gpio1 RX State
	spiWriteRegister(0x0d, 0xfd);    // gpio 2 micro-controller clk output
	spiWriteRegister(0x0e, 0x00);    // gpio    0, 1,2 NO OTHER FUNCTION.

	spiWriteRegister(0x70, 0x2C);    // disable manchest

	spiWriteRegister(0x30, 0x00);    //disable packet handling

	spiWriteRegister(0x79, 0);    // start channel

	spiWriteRegister(0x7a, 0x05); // 50khz step size (10khz x value) // no hopping

	spiWriteRegister(0x71, 0x12); // trclk=[00] no clock, dtmod=[01] direct using SPI, fd8=0 eninv=0 modtyp=[10] FSK
	spiWriteRegister(0x72, 0x02); // fd (frequency deviation) 2*625Hz == 1.25kHz

	spiWriteRegister(0x73, 0x00);
	spiWriteRegister(0x74, 0x00);    // no offset

	//rfmSetCarrierFrequency(446068750);
	rfmSetCarrierFrequency(EU_PMR_CH(6));
	spiWriteRegister(0x6d, 0x07);   // 7 set max power 100mW

	delay(10);
	spiWriteRegister(0x07, RF22B_PWRSTATE_TX);    // to tx mode
	delay(10);

}

//---------------------------------------------------------------------------------------------------

void beacon_send_number(double num, int numOfDigitsLeftOfDot,
		int numOfDigitsRightOfDot, int numOfShortTonesBeforeAndAfter) {

	//beacon_initialize_audio();

	if (num < 0.0)
		num = -num;

	beacon_send_double_num(num, numOfDigitsLeftOfDot, numOfDigitsRightOfDot);

	//beacon_finish_audio();

}

void beacon_send(bool static_tone) {
	Green_LED_ON ItStatus1 = spiReadRegister(0x03); // read status, clear interrupt
	ItStatus2 = spiReadRegister(0x04);
	spiWriteRegister(0x06, 0x00);    // no wakeup up, lbd,
	spiWriteRegister(0x07, RF22B_PWRSTATE_READY); // disable lbd, wakeup timer, use internal 32768,xton = 1; in ready mode
	spiWriteRegister(0x09, 0x7f);  // (default) c = 12.5p
	spiWriteRegister(0x0a, 0x05);
	spiWriteRegister(0x0b, 0x12);    // gpio0 TX State
	spiWriteRegister(0x0c, 0x15);    // gpio1 RX State
	spiWriteRegister(0x0d, 0xfd);    // gpio 2 micro-controller clk output
	spiWriteRegister(0x0e, 0x00);    // gpio    0, 1,2 NO OTHER FUNCTION.

	spiWriteRegister(0x70, 0x2C);    // disable manchest

	spiWriteRegister(0x30, 0x00);    //disable packet handling

	spiWriteRegister(0x79, 0);    // start channel

	spiWriteRegister(0x7a, 0x05); // 50khz step size (10khz x value) // no hopping

	spiWriteRegister(0x71, 0x12); // trclk=[00] no clock, dtmod=[01] direct using SPI, fd8=0 eninv=0 modtyp=[10] FSK
	spiWriteRegister(0x72, 0x02); // fd (frequency deviation) 2*625Hz == 1.25kHz

	spiWriteRegister(0x73, 0x00);
	spiWriteRegister(0x74, 0x00);    // no offset

	//PMR6: 446068750

	//rfmSetCarrierFrequency(rx_config.beacon_frequency);
//  rfmSetCarrierFrequency(446068750);
	rfmSetCarrierFrequency(EU_PMR_CH(6));

	spiWriteRegister(0x6d, 0x07);   // 7 set max power 100mW

	delay(10);
	spiWriteRegister(0x07, RF22B_PWRSTATE_TX);    // to tx mode
	delay(10);

	if (false & static_tone) {
		uint8_t i = 0;
		while (i++ < 20) {
			beacon_tone(440, 1);
			watchdogReset();
		}
	} else {
		//close encounters tune
		//  G, A, F, F(lower octave), C
		//octave 3:  392  440  349  175   261

		beacon_tone(392, 1);
		watchdogReset();

		spiWriteRegister(0x6d, 0x05);   // 5 set mid power 25mW
		delay(10);
		beacon_tone(440, 1);
		watchdogReset();

		spiWriteRegister(0x6d, 0x04);   // 4 set mid power 13mW
		delay(10);
		beacon_tone(349, 1);
		watchdogReset();

		spiWriteRegister(0x6d, 0x02);   // 2 set min power 3mW
		delay(10);
		beacon_tone(175, 1);
		watchdogReset();

		spiWriteRegister(0x6d, 0x00);   // 0 set min power 1.3mW
		delay(10);
		beacon_tone(261, 2);
		watchdogReset();
	}
	spiWriteRegister(0x07, RF22B_PWRSTATE_READY);
	Green_LED_OFF
}

// Print version, either x.y or x.y.z (if z != 0)
void printVersion(uint16_t v) {
	Serial.print(v >> 8);
	Serial.print('.');
	Serial.print((v >> 4) & 0x0f);
	if (version & 0x0f) {
		Serial.print('.');
		Serial.print(v & 0x0f);
	}
}

