/*
 * uart.c
 *
 * Created: 2026/10/9 16:02:56
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 

#include "uart.h"
#include "common.h"

#include <avr/io.h>
#include <stdint.h>


void uart_init(void)
{
    /*
     * CPU = 2 MHz
     * Baud = 9600
     * UBRR = 12
     */
    UBRR0H = 0;
    UBRR0L = 12;

    /*
     * Normal asynchronous mode.
     */
    UCSR0A = 0;

    /*
     * Enable transmitter.
     */
    UCSR0B = (1 << TXEN0);

    /*
     * 8 data bits, no parity,
     * 1 stop bit.
     */
    UCSR0C =
        (1 << UCSZ01) |
        (1 << UCSZ00);
}


void uart_transmit(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0)))
    {
    }

    UDR0 = data;
}


void uart_print(const char *text)
{
    while (*text != '\0')
    {
        uart_transmit((uint8_t)*text);
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
        buffer[i++] =
            (char)('0' + number % 10);

        number /= 10;
    }

    while (i > 0)
    {
        uart_transmit(buffer[--i]);
    }
}


void uart_print_int32(int32_t number)
{
    if (number < 0)
    {
        uart_transmit('-');

        uint32_t magnitude =
            (uint32_t)(-(int64_t)number);

        uart_print_uint32(magnitude);
    }
    else
    {
        uart_print_uint32(
            (uint32_t)number
        );
    }
}


void uart_print_3digits(uint16_t value)
{
    uart_transmit(
        '0' + (value / 100) % 10
    );

    uart_transmit(
        '0' + (value / 10) % 10
    );

    uart_transmit(
        '0' + value % 10
    );
}
