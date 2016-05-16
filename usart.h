/**
 * USART Library
 * Christopher Bero <berocs@acedb.co>
 */

#ifndef AVR_USART_H
#define AVR_USART_H

/* Must be defined before setbaud.h is retrieved */
#ifndef F_CPU
# define F_CPU 16000000UL
#endif

/* Must be defined before setbaud.h is retrieved */
#ifndef BAUD
# define BAUD 38400
#endif

/* header files */
#include <util/setbaud.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

/* Bitmasks for circular buffer */
#define USART_BUF_SIZE (16)

/* Datatype declaration */
struct usart_buffer;

/* Variable declaration */
extern FILE usart_output;
extern FILE usart_input;

/* Function declaration */
void usart_init_async(void);
int usart_putchar_async(char c, FILE *stream);
int usart_getchar_sync(FILE *stream);

#endif // AVR_USART_H
