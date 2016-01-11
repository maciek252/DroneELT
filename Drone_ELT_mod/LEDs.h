/*
 * LEDs.h
 *
 *  Created on: Jan 11, 2016
 *      Author: maciek
 */

#ifndef DRONE_ELT_MOD_LEDS_H_
#define DRONE_ELT_MOD_LEDS_H_



static long led_interval_singleBreak = 1000;
static long led_interval_singleFlash = 200;
static long led_interval_doubleBreak = 200;



enum LED_STATE {
	OFF,
	SINGLE_FLASH,
	SINGLE_FLASH_REV,
	DOUBLE_FLASH,
	DOUBLE_FLASH_REV,
	TRIPLE_FLASH,
	TWO_OPPOSITE,
	ON
};



static void serviceLED(LED_STATE ledMode, long &ledTimer, bool & ledState,
		int &ledFlashCounter);

#endif /* DRONE_ELT_MOD_LEDS_H_ */
