/**
 * USART Library
 * Christopher Bero <berocs@acedb.co>
 */

#include "uart.h"

struct usart_buffer {
    char buffer[USART_BUF_SIZE];
    uint8_t first;
    uint8_t last;
};

static struct usart_buffer buf_rx, buf_tx;

FILE usart_output = FDEV_SETUP_STREAM(usart_putchar_async, NULL, _FDEV_SETUP_WRITE);
FILE usart_input  = FDEV_SETUP_STREAM(NULL, usart_getchar_sync, _FDEV_SETUP_READ);

int send_flag;

// Debugging tool, toggles "pin 13"
//	PINB |= (1 << PB5);

void usart_init(void)
{
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#	if USE_2X
		UCSR0A |= (1 << U2X0);
#	else
		UCSR0A &= (0 << U2X0);
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

	sei();
}

//uint8_t usart_poll()
//{
//	if (send_flag == 1)
//	{
//		if (buf_tx.first != buf_tx.last)
//		{
//			UDR0 = buf_tx.buffer[buf_tx.first];
//			buf_tx.first = (buf_tx.first + 1) % USART_BUF_SIZE;
//		}
//		send_flag = 0;
//	}
//	return 0;
//}

int usart_putchar_async(char data, FILE * stream)
{
    uint8_t next = (buf_tx.last + 1) % USART_BUF_SIZE;

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
        usart_putchar_async('\r', stream);
    }

    return 0;
}

int usart_getchar_sync(FILE * stream)
{
    while ( !(UCSR0A & (1 << RXC0)) );

	return UDR0;
}

//int usart_getchar_async(FILE * stream)
//{
//    uint8_t read_pointer = (buf_rx.first + 1) % USART_BUF_SIZE;
//
//    buf_rx.first = read_pointer;
//    return buf_rx.buffer[read_pointer];
//}
//
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
