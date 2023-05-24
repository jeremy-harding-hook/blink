#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "usart.h"

/* Set constants for USART2 */
#define CONSOLE_UART USART2
#define USART_PORT GPIOA
#define USART_TX_BIT GPIO2
#define USART_RX_BIT GPIO3
#define USART_TX_RX_BITS (GPIO2 | GPIO3)

void usart_setup(void){
	gpio_mode_setup(USART_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART_TX_RX_BITS);
	gpio_set_af(USART_PORT, GPIO_AF7, USART_TX_RX_BITS);
	rcc_periph_clock_enable(RCC_USART2); 

	usart_set_baudrate(CONSOLE_UART, 115200);
	usart_set_databits(CONSOLE_UART, 8);
	usart_set_stopbits(CONSOLE_UART, USART_STOPBITS_1);
	usart_set_mode(CONSOLE_UART, USART_MODE_TX_RX);
	usart_set_parity(CONSOLE_UART, USART_PARITY_NONE);
	usart_set_flow_control(CONSOLE_UART, USART_FLOWCONTROL_NONE);
	usart_enable(CONSOLE_UART);
}

void console_putc(const char c)
{
	uint32_t	reg;
	do {
		reg = USART_SR(CONSOLE_UART);
	} while ((reg & USART_SR_TXE) == 0);
	USART_DR(CONSOLE_UART) = (uint16_t) c & 0xff;
}

char console_getc(const int wait, const int ignore_idle)
{
	uint32_t	reg;
	const unsigned int invalidity_bitmask = ignore_idle ?
		USART_SR_RXNE | USART_SR_IDLE :
		USART_SR_RXNE;
	char data;
	do {
		reg = USART_SR(CONSOLE_UART) ^ USART_SR_RXNE;
		data = (char)USART_DR(CONSOLE_UART); 
	} while (wait && (reg & invalidity_bitmask));
	return (reg & invalidity_bitmask) ? '\000' : data;
}

void console_puts(const char * s)
{
	while (*s != '\000') {
		console_putc(*s);
		/* Add in a carraige return, after sending line feed */
		if (*s == '\n') {
			console_putc('\r');
		}
		s++;
	}
}

int console_gets(char * const s, const int len)
{
	char *t = s;
	char c;

	*t = '\000';
	/* read until a <CR> is received */
	while ((c = console_getc(1, 1)) != '\r') {
		if ((c == '\010') || (c == '\127')) {
			if (t > s) {
				/* send ^H ^H to erase previous character */
				console_puts("\010 \010");
				t--;
			}
		} else {
			*t = c;
			console_putc(c);
			if ((t - s + 1) < len) {
				t++;
				/* TODO: Improve user feedback here? */
			}
		}
		/* update end of string with NUL */
		*t = '\000';
	}
	return t - s;
}
