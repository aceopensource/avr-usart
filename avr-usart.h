/**
 * USART Library
 * Christopher Bero <berocs@acedb.co>
 */

#ifndef AVR_USART_H
#define AVR_USART_H 1

#ifdef __cplusplus
extern "C" {
#endif

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
//#include <stdio.h>

#ifndef NULL
#define NULL ((void *) 0)
#endif

/* Bitmasks for circular buffer */
#define USART_BUF_SIZE (32)

/* Datatype declaration */
struct usart_buffer;

/* Variable declaration */
//extern FILE usart_output;
//extern FILE usart_input;

/* Function declaration */
void usart_init(void);
void usart_close();
void usart_putchar(void * nothing, char c);
//int usart_getchar(FILE *stream);
int usart_getchar(char * data);

#ifdef __cplusplus
}
#endif

#endif // AVR_USART_H
