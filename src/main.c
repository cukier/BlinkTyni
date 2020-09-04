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

#define I_SUB	D,2
#define I_DES	D,3
#define Q_PUL	D,4
#define Q_FDC	D,5
#define Q_HOM	D,6

#define TOP		20

int main(void) {
	uint8_t subir = 0, descer = 0, saida_ctrl = 0, home_ctr = 1, fdc_ctrl = 0;
	uint8_t pos = 0, pos_aux = 0;

	SET_INPUT(I_SUB);
	SET_INPUT(I_DES);
	SET_OUTPUT(Q_PUL);
	SET_OUTPUT(Q_FDC);
	SET_OUTPUT(Q_HOM);

	uart_init(UART_BAUD_SELECT(BAUD, F_CPU));
	_delay_ms(500);
	RESET(Q_PUL);
	sei();

	while (1) {
		subir = !IS_SET(I_SUB);
		descer = !IS_SET(I_DES);

		if (subir || descer) {
			_delay_ms(50);

			if (subir) {
				if (pos < TOP) {
					TOGGLE(Q_PUL);
					++pos;
				}
			}

			if (descer) {
				if (pos) {
					TOGGLE(Q_PUL);
					--pos;
				}
			}

			if (saida_ctrl) {
				saida_ctrl = 0;
//				uart_puts("Saida acionada\n\r");

//				if (subir)
//					uart_puts("Subir\n\r");
//				else if (descer)
//					uart_puts("Descer\n\r");
			}
		} else if (!saida_ctrl) {
			saida_ctrl = 1;
			RESET(Q_PUL);
//			uart_puts("Parado\n\r");
		}

		if (pos == 0) {
			if (home_ctr) {
				home_ctr = 0;
				SET(Q_HOM);
				RESET(Q_PUL);
//				uart_puts("HOME\n\r");
			}
		} else if (!home_ctr) {
			home_ctr = 1;
			RESET(Q_HOM);
		}

		if (pos == TOP) {
			if (fdc_ctrl) {
				fdc_ctrl = 0;
				SET(Q_FDC);
				RESET(Q_PUL);
//				uart_puts("TOP\n\r");
			}
		} else if (!fdc_ctrl) {
			fdc_ctrl = 1;
			RESET(Q_FDC);
		}

		if (pos != pos_aux) {
			pos_aux = pos;
			uart_puts("Pos ");
			uart_putc(pos + '0');
			uart_puts("\n\r");
		}
	}

	return 0;
}
