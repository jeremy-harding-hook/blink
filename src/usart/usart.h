#ifndef USART_H
#define USART_H

/**
 * Does the initialisation and setup of the USART. Call before attempting to use
 * the usart i/o functions. Only call once.
 */
void usart_setup(void);

/*
 * console_putc(char c)
 *
 * Send the character 'c' to the USART, wait for the USART
 * transmit buffer to be empty first.
 */
void console_putc(const char c);

/*
 * char = console_getc(int wait)
 *
 * Check the console for a character. If the wait flag is
 * non-zero. Continue checking until a character is received
 * otherwise return 0 if called and no character was available.
 * If ignore_idle is true an idle frame is not considered a character. Unless
 * you want to see a lot of FFs in your user input or you're just checking
 * connection it is recommended to set this parameter to nonzero.
 */
char console_getc(const int wait, const int ignore_idle);

/*
 * void console_puts(char *s)
 *
 * Send a string to the console, one character at a time, return
 * after the last character, as indicated by a NUL character, is
 * reached.
 */
void console_puts(const char *s);

/*
 * Wait for a string to be entered on the console, limited
 * support for editing characters (back space and delete)
 * end when a <CR> character is received.
 */
int console_gets(char * s, int len);

#endif
