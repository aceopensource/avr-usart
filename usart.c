/**
 * USART Library
 * Christopher Bero <berocs@acedb.co>
 */

#include "usart.h"

struct usart_buffer {
    char buffer[USART_BUF_SIZE];
    volatile uint8_t first;
    volatile uint8_t last;
};

//FILE usart_output = FDEV_SETUP_STREAM(usart_putchar, NULL, _FDEV_SETUP_WRITE);
//FILE usart_input  = FDEV_SETUP_STREAM(NULL, usart_getchar, _FDEV_SETUP_READ);

#ifdef USART_ASYNC
static struct usart_buffer buf_rx, buf_tx;
#endif // USART_ASYNC

// Debugging tool, toggles "pin 13"
//	PINB |= (1 << PB5);

#ifdef USART_ASYNC
void usart_init(void)
{
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#	if USE_2X
		UCSR0A |= (1 << U2X0);
#	else
		UCSR0A &= ~(1 << U2X0);
#	endif

	/** UCSR0C 	- Datasheet p194
	 * UMSEL0X 	- USART Mode -> Async
	 * UPM0X 	- Parity Mode -> Disabled
	 * USBS0 	- Stop Bit -> 1 bit
	 * UCSZ0X 	- Character Size -> 8 bit
	 * UCPOL0 	- Clock Polarity -> Disabled for async
	 */
    UCSR0C = 	(0 << UMSEL01) 	| 	(0 << UMSEL00) 	| \
				(0 << UPM01) 	| 	(0 << UPM00) 	| \
				(0 << USBS0) 	| 	(1 << UCSZ01) 	| \
				(1 << UCSZ00) 	| 	(0 << UCPOL0);

    /** UCSR0B 	- Datasheet p192
	 * RX/TXCIE0- Interrupt enable -> disabled
	 * UDRIE0 	- Data Register Empty Interrupt -> disabled
	 * RX/TXEN0	- RX/TX enable -> enabled
	 * UCSZ02 	- Character Size -> 8 bit
	 * RX/TXB80	- Used for 9 bit characters
	 */
    UCSR0B = 	(0 << RXCIE0) 	| (0 << TXCIE0) | \
				(0 << UDRIE0) 	| (1 << RXEN0) 	| \
				(1 << TXEN0) 	| (0 << UCSZ02);

	buf_rx.first 	= 0;
	buf_rx.last 	= 0;
	buf_tx.first 	= 0;
	buf_tx.last 	= 0;

//	stdout = &usart_output;
//    stdin  = &usart_input;

	sei();
}
#else
void usart_init(void)
{
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#	if USE_2X
		UCSR0A |= (1 << U2X0);
#	else
		UCSR0A &= ~(1 << U2X0);
#	endif

    UCSR0C = 	(0 << UMSEL01) 	| 	(0 << UMSEL00) 	| \
				(0 << UPM01) 	| 	(0 << UPM00) 	| \
				(0 << USBS0) 	| 	(1 << UCSZ01) 	| \
				(1 << UCSZ00) 	| 	(0 << UCPOL0);

	UCSR0B = 	(0 << RXCIE0) 	| (0 << TXCIE0) | \
				(0 << UDRIE0) 	| (1 << RXEN0) 	| \
				(1 << TXEN0) 	| (0 << UCSZ02);

//	stdout = &usart_output;
//    stdin  = &usart_input;
}
#endif // USART_ASYNC

#ifdef USART_ASYNC
int usart_putchar(char data, FILE * stream)
{
    uint8_t next = (buf_tx.last + 1) % USART_BUF_SIZE;

    while (next == buf_tx.first)
	{
//		PINB |= (1 << PB5);
		_delay_ms(2);
	}

    if (next != buf_tx.first)
	{
        buf_tx.buffer[buf_tx.last] = data;
        buf_tx.last = next;
        // enable interrupt
		UCSR0B |= (1 << UDRIE0);
    }
    else
	{
		return 1;
	}

    if (data == '\n')
	{
        usart_putchar('\r', stream);
    }

    return 0;
}
#else
//int usart_putchar(char data, FILE * stream)
void usart_putchar(void* nothing, char data)
{
//	uint8_t timeout;

//	timeout = 0;
	OCR1A = 2;
    while (!(UCSR0A & (1 << UDRE0)))
	{
		//_delay_us(50);
//		timeout++;
//		if (timeout > 20)
//			return 1;
	}
	OCR1A = 0;

    UDR0 = data;

    // Confirm byte is sent, slower but
    // prevents serial garbage with LPM.
//    timeout = 0;
    OCR1A = 2;
    while (!(UCSR0A & (1 << TXC0)))
	{
		//_delay_us(50);
//		timeout++;
//		if (timeout > 20)
//			return 1;
	}
	OCR1A = 0;

    UCSR0A |= (1 << TXC0);

    if (data == '\n') {
//        usart_putchar('\r', stream);
		usart_putchar(NULL, '\r');
    }
//    return 0;
	return;
}
#endif // USART_ASYNC


//#ifdef USART_ASYNC
//int usart_getchar(FILE * stream)
//{
//    uint8_t read_pointer = (buf_rx.first + 1) % USART_BUF_SIZE;
//
//    buf_rx.first = read_pointer;
//    return buf_rx.buffer[read_pointer];
//}
//#else
//int usart_getchar(FILE * stream)
//{
////	uint8_t timeout;
//
////	timeout = 0;
//	OCR1A = 2;
//    while (!(UCSR0A & (1 << RXC0)))
//	{
//		//_delay_us(50);
////		timeout++;
////		if (timeout > 20)
////			return 1;
//	}
//	OCR1A = 0;
//
//    return UDR0;
//}
//#endif // USART_ASYNC


#ifdef USART_ASYNC
//ISR(USART_RX_vect)
//{
//    int write_pointer = (buf_rx.last + 1) % USART_BUF_SIZE;
//
//    /* Add next byte to buffer if it has space available. */
//    if (write_pointer != buf_rx.first)
//	{
//        buf_rx.buffer[buf_rx.last] = UDR0;
//        buf_rx.last = write_pointer;
//    }
//}

ISR(USART_UDRE_vect)
{
	if (buf_tx.first != buf_tx.last)
	{
		UDR0 = buf_tx.buffer[buf_tx.first];
		buf_tx.first = (buf_tx.first + 1) % USART_BUF_SIZE;
	}
	else
	{
		// disable interrupt
		UCSR0B &= ~(1 << UDRIE0);
	}
}
#endif // USART_ASYNC

