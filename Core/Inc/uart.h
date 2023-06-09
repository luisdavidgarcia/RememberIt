/*
 *      Author: Luis D. Garcia and Nilay Patel
 *      Description: UART Library Header File that permits
 *      			 interfacing with STM32L476RG.
*/

#ifndef SRC_UART_H_
#define SRC_UART_H_

#include "stdint.h"

#define BAUD_RATE 0x1C200  		// is just 115200
#define SYS_CLOCK 0x1E84800		// is just 32000000 Hz
#define UART_AF_MODE 0x7

void UART_init(void);
void UART_print(const uint8_t * str);
uint8_t UART_receive(void);

#endif /* SRC_UART_H_ */
