/*
 * main.c
 *
 *  Created on: 1 Sep 2020
 *      Author: mauricio
 */

#include "sys.h"
#include "uart.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void) {
	uart_init(UART_BAUD_SELECT(BAUD, F_CPU));
	_delay_ms(500);
	sei();

	while (1) {
		uart_puts("hello\n\r");
		_delay_ms(1500);
	}

	return 0;
}
