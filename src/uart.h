#ifndef UART_H
#define UART_H

#include <avr/io.h>

#define UART_RX0_BUFFER_SIZE 32
#define UART_TX0_BUFFER_SIZE 32

#define UART_FRAME_ERROR      0x0800              /**< Framing Error by UART       */
#define UART_OVERRUN_ERROR    0x0400              /**< Overrun condition by UART   */
#define UART_BUFFER_OVERFLOW  0x0200              /**< receive ringbuffer overflow */
#define UART_NO_DATA          0x0100              /**< no receive data available   */

#define UART_BAUD_SELECT(baudRate,xtalCpu) (((xtalCpu)+8UL*(baudRate))/(16UL*(baudRate))-1UL)

extern void uart_init(uint16_t baudrate);
extern uint16_t uart_getc(void);
extern void uart_putc(uint8_t data);
extern void uart_puts(const char *s);
extern uint16_t uart_available(void);
extern void uart_flush(void);

#endif //UART_H
