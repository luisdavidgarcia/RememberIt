#include "adc.h"
#include "stm32l4xx_hal.h"

void ADC12_Init(void) {
	// Enable GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

	// Configure GPIO for channel 5 on analog input pin (PA0) ADC1
	// Clear PA0
	GPIOA->MODER &= ~(GPIO_MODER_MODE0_Msk);
	// Set pin as analog mode
	GPIOA->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE0_1);
	// Set analog switch
	GPIOA->ASCR |= GPIO_ASCR_ASC0;
	// Configure GPIO for channel 6 on analog input (PA1) ADC2
	// Clear PA1
	GPIOA->MODER &= ~(GPIO_MODER_MODE1_Msk);
	// set pin as analog mode
	GPIOA->MODER |= (GPIO_MODER_MODE1_0 | GPIO_MODER_MODE1_1);
	// set analog switch
	GPIOA->ASCR |= GPIO_ASCR_ASC1;

	// Enable clock for ADC
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	//ADC will run at the same speed as CPU (CLK/1), Prescaler = 1
	ADC123_COMMON->CCR = (1 << ADC_CCR_CKMODE_Pos);

	/*--------------- Set up ADC1 Here ------------*/
	//Power up the ADC and voltage regulator
	ADC1->CR &= ~(ADC_CR_DEEPPWD);
	ADC1->CR |= (ADC_CR_ADVREGEN);
	// delay for voltage regulation startup time ~ 20 us
	for (uint32_t i = 0; i < DELAY_TIME; i++)
		;
	// Calibrate the ADC1 and ensure ADC1 is disabled and single-ended mode
	ADC1->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF);
	// start the calibration
	ADC1->CR |= ADC_CR_ADCAL;
	while (ADC1->CR & ADC_CR_ADCAL)
		;
	// Configure for single-ended mode on channel 5 must be set before enabling the ADC1
	// Using ADC12_IN5 channel
	ADC1->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_5);
	// Enable ADC1
	// Clear ready bit with a 1
	ADC1->ISR |= (ADC_ISR_ADRDY);
	// Enable ADC
	ADC1->CR |= ADC_CR_ADEN;
	while (!(ADC1->ISR & ADC_ISR_ADRDY))
		;
	// Clear ready bit with a 1 (optional)
	ADC1->ISR |= (ADC_ISR_ADRDY);
	// Configure ADC1
	// set sequence for 1 conversion on channel 5
	ADC1->SQR1 |= (5 << ADC_SQR1_SQ1_Pos);
	// 12 bit resolution, software trigger, right align, single conversion
	ADC1->CFGR = 0;
	// Used to change sampling rate -> ADC1->SMPR1 = (SMP_640_5 << ADC_SMPR1_SMP5_Pos);

	/*--------------- Set up ADC2 Here --------------*/
	ADC2->CR &= ~(ADC_CR_DEEPPWD);
	ADC2->CR |= (ADC_CR_ADVREGEN);
	// delay for voltage regulation startup time ~ 20 us
	for (uint32_t i = 0; i < DELAY_TIME; i++)
		;
	// Calibrate ADC2 and ensure ADC2 is disabled and single-ended mode
	ADC2->CR &= ~(ADC_CR_ADEN | ADC_CR_ADCALDIF);
	// Configure for single-ended mode on channel 6 must be set befor ennable the ADC2
	// Using ADC12_IN6 channel
	ADC2->DIFSEL &= ~(ADC_DIFSEL_DIFSEL_6);
	// Enable ADC2
	// Clear ready bit with a 1
	ADC2->ISR |= (ADC_ISR_ADRDY);
	// Enable ADC
	ADC2->CR |= ADC_CR_ADEN;
	while (!(ADC2->ISR & ADC_ISR_ADRDY))
		;
	// Clear ready bit with a 1 (optional)
	ADC2->ISR |= (ADC_ISR_ADRDY);
	// Set sequence for 1 conversion on channel 6
	ADC2->SQR1 |= (6 << ADC_SQR1_SQ1_Pos);
	// 12 bit resolution, software trigger, right align, single conversion
	ADC2->CFGR = 0;

	// Start conversion
	ADC1->CR |= (ADC_CR_ADSTART);
	ADC2->CR |= (ADC_CR_ADSTART);
	// Enable interrupts for ADC
	// Interrupt on end of conversion
	ADC1->IER |= (ADC_IER_EOC);
	// Clear EOC flag with 1
	ADC1->ISR &= ~(ADC_ISR_EOC);
	// Interrupt on end of conversion
	ADC2->IER |= (ADC_IER_EOC);
	// Clear EOC flag with h2
	ADC2->ISR &= ~(ADC_ISR_EOC);
	// Enable interrupt in NVIC
	NVIC->ISER[0] = (1 << (ADC1_2_IRQn & 0x1F));
	// Enable interrupts globally
	__enable_irq();
}