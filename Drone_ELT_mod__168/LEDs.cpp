/*
 * LEDs.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: maciek
 */
#include "LEDs.h"
#include <Arduino.h>

void serviceLED(LED_STATE ledMode, long &ledTimer, bool & ledState,
		int &ledFlashCounter) {

	switch (ledMode) {
	//enum LED_STATE {OFF, SINGLE_FLASH, DOUBLE_FLASH, TWO_OPPOSITE, ON};
	case SINGLE_FLASH:
		if (!ledState && millis() - ledTimer > led_interval_singleBreak) {
			ledState = true;
			ledTimer = millis();
		} else if (ledState && millis() - ledTimer > led_interval_singleFlash) {
			ledState = false;
			ledTimer = millis();
		}
		break;
	case SINGLE_FLASH_REV:
		if (ledState && millis() - ledTimer > led_interval_singleBreak) {
			ledState = false;
			ledTimer = millis();
		} else if (!ledState
				&& millis() - ledTimer > led_interval_singleFlash + 1000) {
			ledState = true;
			ledTimer = millis();
		}
		break;
	case QUICK_FLASH:
		if (!ledState && millis() - ledTimer > led_interval_quickFlash) {
			ledState = true;
			ledTimer = millis();
		} else if (ledState && millis() - ledTimer > led_interval_quickFlash) {
			ledState = false;
			ledTimer = millis();
		}
		break;
	case DOUBLE_FLASH:
		if (!ledState && millis() - ledTimer > led_interval_singleBreak
				&& ledFlashCounter == 0) {
			ledState = true;
			ledTimer = millis();
		} else if (!ledState && millis() - ledTimer > led_interval_singleFlash
				&& ledFlashCounter > 0) {
			ledState = true;
			ledTimer = millis();
		} else if (ledState && millis() - ledTimer > led_interval_singleFlash) {
			ledState = false;
			ledTimer = millis();
			ledFlashCounter++;
			if (ledFlashCounter == 2)
				ledFlashCounter = 0;
		}
		break;
	case TRIPLE_FLASH:
			if (!ledState && millis() - ledTimer > led_interval_singleBreak
					&& ledFlashCounter == 0) {
				ledState = true;
				ledTimer = millis();
			} else if (!ledState && millis() - ledTimer > led_interval_singleFlash
					&& ledFlashCounter > 0) {
				ledState = true;
				ledTimer = millis();
			} else if (ledState && millis() - ledTimer > led_interval_singleFlash) {
				ledState = false;
				ledTimer = millis();
				ledFlashCounter++;
				if (ledFlashCounter == 3)
					ledFlashCounter = 0;
			}
			break;
	case DOUBLE_FLASH_REV:
		if (ledState && millis() - ledTimer > led_interval_singleBreak
				&& ledFlashCounter == 0) {
			ledState = false;
			ledTimer = millis();
		} else if (ledState && millis() - ledTimer > led_interval_singleFlash
				&& ledFlashCounter > 0) {
			ledState = false;
			ledTimer = millis();
		} else if (!ledState
				&& millis() - ledTimer > led_interval_singleFlash) {
			ledState = true;
			ledTimer = millis();
			ledFlashCounter++;
			if (ledFlashCounter == 2)
				ledFlashCounter = 0;
		}
		break;
	case ON:
		ledState = true;
		break;
	case OFF:
		ledState = false;
		break;
	}
}
