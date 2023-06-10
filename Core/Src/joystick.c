#include "joystick.h"
#include <stdint.h>
#include "main.h"

DIR readJoystick(uint16_t x, uint16_t y) {
	/*
	Takes joystick output and maps to directoin
	:param x: ADC reading from joystick on x-axis, range: (0 ~ 4000)
	:param y: ADC reading from joystick on y-axis, range: (0 ~ 4000)
	:return: enum type specifying direction of joystick
	*/
	DIR ret = NONE;

	//interpert ADC12 data to get direction
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
	/*
	Starts adc conversion to read joystick x and y values
	*/
	//start adc1 and adc2 conversion
	ADC1->CR |= ADC_CR_ADSTART;
	ADC2->CR |= ADC_CR_ADSTART;
}

int8_t checkJoystickComplete(uint16_t adc1_ready, uint16_t adc2_ready) {
	/*
	Checks user defined flags to see if ADC covnersions complete
	:param adc1_ready: flag for adc1 conversion
	:param adc2_ready: flag for adc2 conversion
	:return: 1 if ready, -1 if not
	*/
	//if both  conversoin complete return 1
	if (adc1_ready && adc2_ready) {
		adc1_ready = 0;
		adc2_ready = 0;
		return 1;
	}
	//return -1 otherwise
	return -1;
}

void setupJoystickButton(void) {
	/*
	Initialize joystick pushdown button
	*/
	// Turn on the clock for GPIOC
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // Set MODE to input
	GPIOC->MODER &= ~(GPIO_MODER_MODE0); 		

    // Pull up resistor so idles high and drops to 0 when button pressed
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0); 		
	GPIOC->PUPDR |= (1 << GPIO_PUPDR_PUPD0_Pos);
}

int8_t detectButtonPress(void) {
	/*
	Checks for button press
	:return: 1 if button press, otherwise -1
	*/
	//if button press return 1
	if ((GPIOC->IDR  & GPIO_IDR_ID0) == 0) {
		return 1;

	//return -1 if no button press
	} else {
		return -1;
	}
}
