/*
 * Author: Luis D. Garcia
 * Filename: uart.h
 * Description: Header file that contains all pound defines for uart
 */

#ifndef SRC_UART_H_
#define SRC_UART_H_

#include <stdint.h>

#define BAUD_RATE 0x1C200  		// is just 115200
#define SYS_CLOCK 0x1E84800		// is just 32000000 Hz
#define UART_AF_MODE 0x7

void UART_init(void);
void UART_print(const uint8_t *str);
uint8_t UART_receive(void);
void printVal(uint16_t digitalVal);

#endif /* SRC_UART_H_ */
