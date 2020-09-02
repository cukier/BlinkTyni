#include "uart.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#include <stdio.h>
#include <stdarg.h>

#define UART_RX0_BUFFER_MASK (UART_RX0_BUFFER_SIZE - 1)
#define UART_TX0_BUFFER_MASK (UART_TX0_BUFFER_SIZE - 1)

#define ATMEGA_USART
#define UART0_RECEIVE_INTERRUPT   	USART_RX_vect
#define UART0_TRANSMIT_INTERRUPT  	USART_UDRE_vect
#define UART0_STATUS   				UCSRA
#define UART0_CONTROL 				UCSRB
#define UART0_DATA     				UDR
#define UART0_UDRIE    				UDRIE

static volatile uint8_t UART_TxBuf[UART_TX0_BUFFER_SIZE];
static volatile uint8_t UART_RxBuf[UART_RX0_BUFFER_SIZE];

static volatile uint8_t UART_TxHead;
static volatile uint8_t UART_TxTail;
static volatile uint8_t UART_RxHead;
static volatile uint8_t UART_RxTail;
static volatile uint8_t UART_LastRxError;

ISR(UART0_RECEIVE_INTERRUPT) {
	uint16_t tmphead;
	uint8_t data;
	uint8_t usr;
	uint8_t lastRxError;

	usr = UART0_STATUS;
	data = UART0_DATA;
	lastRxError = (usr & (_BV(FE) | _BV(DOR)));
	tmphead = (UART_RxHead + 1) & UART_RX0_BUFFER_MASK;

	if (tmphead == UART_RxTail) {
		lastRxError = UART_BUFFER_OVERFLOW >> 8;
	} else {
		UART_RxHead = tmphead;
		UART_RxBuf[tmphead] = data;
	}

	UART_LastRxError = lastRxError;
}

ISR(UART0_TRANSMIT_INTERRUPT) {
	uint16_t tmptail;

	if (UART_TxHead != UART_TxTail) {
		tmptail = (UART_TxTail + 1) & UART_TX0_BUFFER_MASK;
		UART_TxTail = tmptail;
		UART0_DATA = UART_TxBuf[tmptail];
	} else {
		UART0_CONTROL &= ~_BV(UART0_UDRIE);
	}
}

void uart_init(uint16_t baudrate) {
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		UART_TxHead = 0;
		UART_TxTail = 0;
		UART_RxHead = 0;
		UART_RxTail = 0;
	}

	if (baudrate & 0x8000) {
		UART0_STATUS = (1 << U2X);
		baudrate &= ~0x8000;
	}

	UBRRH = (uint8_t) (baudrate >> 8);
	UBRRL = (uint8_t) baudrate;

	UART0_CONTROL = _BV(RXCIE) | (1 << RXEN) | (1 << TXEN);
	UCSRC = (3 << UCSZ0);
}

uint16_t uart_getc(void) {
	uint16_t tmptail;
	uint8_t data;

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		if (UART_RxHead == UART_RxTail) {
			return UART_NO_DATA;
		}
	}

	tmptail = (UART_RxTail + 1) & UART_RX0_BUFFER_MASK;

	UART_RxTail = tmptail;

	data = UART_RxBuf[tmptail];

	return (UART_LastRxError << 8) + data;
}

void uart_putc(uint8_t data) {
	uint16_t tmphead;

	tmphead = (UART_TxHead + 1) & UART_TX0_BUFFER_MASK;

	while (tmphead == UART_TxTail)
		;

	UART_TxBuf[tmphead] = data;
	UART_TxHead = tmphead;
	UART0_CONTROL |= _BV(UART0_UDRIE);
}

void uart_puts(const char *s) {
	while (*s) {
		uart_putc(*s++);
	}

}

uint16_t uart_available(void) {
	uint16_t ret;

	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		ret = (UART_RX0_BUFFER_SIZE + UART_RxHead - UART_RxTail)
				& UART_RX0_BUFFER_MASK;
	}
	return ret;
}

void uart_flush(void) {
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		UART_RxHead = UART_RxTail;
	}
}
