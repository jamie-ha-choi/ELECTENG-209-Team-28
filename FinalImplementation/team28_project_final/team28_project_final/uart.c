/*
 * uart.c
 *
 * Created: 2026/10/9 16:02:56
 *  Author: Admin
 */ 
#include "uart.h"
#include "common.h"

#include <avr/io.h>


void uart_init(void)
{
    /*
     * F_CPU = 2MHz
     * Baud = 9600
     *
     * UBRR = 12
     */

    UBRR0H = 0;
    UBRR0L = 12;


    /*
     * Normal asynchronous mode.
     */

    UCSR0A = 0;


    /*
     * Enable transmitter only.
     */

    UCSR0B = (1 << TXEN0);


    /*
     * 8 data bits
     * no parity
     * 1 stop bit
     */

    UCSR0C = (1 << UCSZ01)
           | (1 << UCSZ00);
}


void uart_transmit(uint8_t data)
{
    /*
     * Wait until transmit buffer is empty.
     */

    while (!(UCSR0A & (1 << UDRE0)))
    {
    }


    /*
     * Put byte into UART register.
     */

    UDR0 = data;
}


void uart_print(const char *text)
{
    while (*text != '\0')
    {
        uart_transmit(*text);

        text++;
    }
}


void uart_print_uint32(uint32_t number)
{
    char buffer[10];

    uint8_t i = 0;


    if (number == 0)
    {
        uart_transmit('0');

        return;
    }


    while (number > 0)
    {
        buffer[i] = (number % 10) + '0';

        number /= 10;

        i++;
    }


    while (i > 0)
    {
        i--;

        uart_transmit(buffer[i]);
    }
}


void uart_print_int32(int32_t number)
{
    if (number < 0)
    {
        uart_transmit('-');

        number = -number;
    }

    uart_print_uint32((uint32_t)number);
}


/*
 * Always print exactly 3 digits.
 *
 * Example:
 *
 * 7   -> 007
 * 45  -> 045
 * 625 -> 625
 */

void uart_print_3digits(uint16_t value)
{
    uart_transmit(
        '0' + ((value / 100) % 10)
    );

    uart_transmit(
        '0' + ((value / 10) % 10)
    );

    uart_transmit(
        '0' + (value % 10)
    );
}