/*
 * main.c
 *
 *  Created on: 1 Sep 2020
 *      Author: mauricio
 */

//#include "sys.h"
//#define AT90_UART
//#include "uart.h"
#include <avr/io.h>
//#include <avr/interrupt.h>
//#include <util/delay.h>

int main(void) {
	DDRD |= (1 << 0);

//	uart0_init(UART_BAUD_SELECT(BAUD, F_CPU));
//	uart_init(UART_BAUD_SELECT(BAUD, F_CPU));
//	unsigned long int i;
//	_delay_ms(500);
//	sei();

	while (1) {
		PORTD ^= (1 << 0);
//		uart_printf("hello\n");
//		_delay_ms(1500);
	}

	return 0;
}
