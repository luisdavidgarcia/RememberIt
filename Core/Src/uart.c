/*
 * Author: Luis D. Garcia
 * Filename: uart.c
 * Description: Implementation file that permits communication over uart
 */

#include "uart.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

void UART_init(void) {
	// Enable GPIOA and USART2 clocks
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

	// Configure USART2 pins for alternate function
	GPIOA->MODER &= ~(GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk);
	GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
	GPIOA->AFR[0] |= (UART_AF_MODE << GPIO_AFRL_AFSEL2_Pos)
			| (UART_AF_MODE << GPIO_AFRL_AFSEL3_Pos);

	// Configure USART2 for 115200 baud, 8-bit data, no parity, 1 stop bit
	USART2->BRR = (SYS_CLOCK / BAUD_RATE);

	// Enable everything for USART
	USART2->CR1 |=
	USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_UE;

	// Enable interrupts in NVIC
	NVIC->ISER[1] = (1 << (USART2_IRQn & 0x1F)); // Enable interrupt in NVIC for UART2
}

void UART_print(const uint8_t *str) {
	while (*str) {
		while (!(USART2->ISR & USART_ISR_TXE_Msk))
			;
		USART2->TDR = (*str++);
	}
}

uint8_t UART_receive(void) {
	while (!(USART2->ISR & USART_ISR_RXNE_Msk))
		;
	return USART2->RDR;
}

void printVal(uint16_t digitalVal) {
	char buffer[10];
	sprintf(buffer, "%d", digitalVal);
	while (!(USART2->ISR & USART_ISR_TXE_Msk))
		;
	UART_print(buffer);
}
