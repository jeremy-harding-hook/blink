#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "usart.h"

#define CONSOLE_USART USART2
#define USART_PORT GPIOA
#define USART_TX_BIT GPIO2
#define USART_RX_BIT GPIO3

#ifdef FLOW_CONTROL_ON
#define USART_CTS_BIT GPIO0
#define USART_RTS_BIT GPIO1
#define USART_BITS (USART_TX_BIT | USART_RX_BIT | USART_RTS_BIT | USART_CTS_BIT)

#else
#define USART_BITS (USART_TX_BIT | USART_RX_BIT)
#endif

void usart_setup(void){
	rcc_periph_clock_enable(RCC_USART2); 
	gpio_mode_setup(USART_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, USART_BITS);
	gpio_set_af(USART_PORT, GPIO_AF7, USART_BITS);

	usart_enable(CONSOLE_USART);
	usart_set_baudrate(CONSOLE_USART, 9600);
	usart_set_databits(CONSOLE_USART, 8);
	usart_set_stopbits(CONSOLE_USART, USART_STOPBITS_1);
	usart_set_mode(CONSOLE_USART, USART_MODE_TX_RX);
	usart_set_parity(CONSOLE_USART, USART_PARITY_NONE);
	usart_set_flow_control(CONSOLE_USART, USART_FLOWCONTROL_NONE);
}

void console_putc(const char c)
{
	switch (c)
	{
	case '\n':
	case '\r':
		usart_send_blocking(CONSOLE_USART, '\n');
		usart_send_blocking(CONSOLE_USART, '\r');
		break;
	case '\b':
	case '\x7f':
		usart_send_blocking(CONSOLE_USART, '\b');
		usart_send_blocking(CONSOLE_USART, ' ');
		usart_send_blocking(CONSOLE_USART, '\b');
		break;
	default:
		usart_send_blocking(CONSOLE_USART, c);
	}
}

#define ECHO
char console_getc(const int wait, const int ignore_idle)
{
	uint32_t	reg;
	const unsigned int invalidity_bitmask = ignore_idle ?
		USART_SR_RXNE | USART_SR_IDLE :
		USART_SR_RXNE;
	char data;
	do {
		reg = USART_SR(CONSOLE_USART) ^ USART_SR_RXNE;
		data = (char)USART_DR(CONSOLE_USART) & USART_DR_MASK; 
		if((reg & invalidity_bitmask) == 0){
			console_puts("Character recieved!\n");
		}
		if(reg & USART_SR_ORE){
			console_puts("Overrun error!\n");
		}
		if(reg & USART_SR_FE){
			console_puts("Framing error!\n");
		}
		if(reg & USART_SR_NE){
			console_puts("Noise error!\n");
		}
		if(reg & USART_SR_PE){
			console_puts("Parity error!\n");
		}
	} while (wait && (reg & invalidity_bitmask));
	return (reg & invalidity_bitmask) ? '\000' : data;
}

void console_puts(const char * s)
{
	while (*s != '\000') {
		console_putc(*s++);
	}
}

int console_gets(char * const s, const int len){
	char *current_char = s;
	char recieved_char;

	console_puts("> ");
	while(1){
		if(!(USART_SR(CONSOLE_USART) & USART_SR_RXNE))
			continue;
		recieved_char = (char)USART_DR(CONSOLE_USART) & USART_DR_MASK; 

		switch(recieved_char){
			case '\b':
			case '\x7F':
				if(current_char - s)
				{
					current_char--;
					console_putc(recieved_char);
				}
				continue;
			case '\r':
			case '\n':
				console_putc(recieved_char);
				*current_char = '\0';
				return current_char - s + 1;
			default:
				console_putc(recieved_char);
		}

		if(current_char - s == len - 1)
			console_putc('\b');
		else
			*current_char++ = recieved_char;
	}
}

int console_gets_old(char * const s, const int len)
{
	char *t = s;
	char c;

	if(USART_CR1(CONSOLE_USART) & USART_CR1_OVER8){
		console_puts("Over8 is set to 1\n");
	}
	else{
		console_puts("Over8 is set to 0\n");
	}
	*t = '\000';
	/* read until a <CR> is received */
	while ((c = console_getc(1, 0)) != '\r') {
		if ((c == '\010') || (c == '\x7F')) {
			if (t > s) {
				/* send ^H ^H to erase previous character */
#ifdef ECHO
				console_puts("\010 \010");
#endif
				t--;
			}
		} else {
			*t = c;
			if ((t - s + 1) < len) {
#ifdef ECHO
				console_putc(c);
#endif
				t++;
				/* TODO: Improve user feedback here? */
			}
		}
		/* update end of string with NUL */
		*t = '\000';
	}
	return t - s;
}

#if 0
__attribute__((interrupt(USART_CR1_RXNEIE_VECTOR))) void handle_usart_rx(void){
	;
}
#endif
