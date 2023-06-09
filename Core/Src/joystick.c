#include "joystick.h"
#include <stdint.h>
#include "main.h"

DIR readJoystick(uint16_t x, uint16_t y) {
	DIR ret = NONE;

	if (y > 3500) {
		ret = DOWN;
	} else if (y < 500) {
		ret = UP;
	} else if (x < 500) {
		ret = LEFT;
	} else if (x > 3500) {
		ret = RIGHT;
	}
	return ret;
}

void startJoystickRead(void) {
	ADC1->CR |= ADC_CR_ADSTART;
	ADC2->CR |= ADC_CR_ADSTART;
}

int8_t checkJoystickComplete(uint16_t adc1_ready, uint16_t adc2_ready) {
	if (adc1_ready && adc2_ready) {
		adc1_ready = 0;
		adc2_ready = 0;
		return 1;
	}
	return -1;
}

void setupJoystickButton(void) {
	// Turn on the clock for GPIOC
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    // Set MODE to input
	GPIOC->MODER &= ~(GPIO_MODER_MODE0); 		
    // Set PUPD to Pull Up
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0); 		
	GPIOC->PUPDR |= (1 << GPIO_PUPDR_PUPD0_Pos);
}

int8_t detectButtonPress(void) {
	if ((GPIOC->IDR  & GPIO_IDR_ID0) == 0) {
		return 1;

	} else {
		return -1;
	}
}
