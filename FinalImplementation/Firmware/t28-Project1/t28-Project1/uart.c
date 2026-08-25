/*
 * uart.c
 *
 * Created: 2026/8/25 12:57:47
 *  Author: Lingwu Cai, Bowen, Ziqi Meng, Jamie
 */ 

/* Give access to ATmega hardware register,
provide fixed width integer, include functions created in uart.h. */
#include <avr/io.h>
#include <stdint.h>
#include "uart.h"

// USART initialization
void usart_init(uint16_t ubrr){
	// Store upper bit of ubrr in UBRR0H and store lower bits of ubrr in UBRR0L.
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;
	
	UCSR0A = 0; // Setting UCSR0A clear all bits in UCSR0A.
	
	UCSR0B = (1 << TXEN0); // Setting UCSR0B to TXEN0 to enable transmitter.
	
	UCSR0C = (1 << UCSZ01)|(1 << UCSZ00); // Setting UCSR0C to UCSZ01 and UCSZ00 to set character size to 8 bit.
}

// Do nothing when UDRE0 is 0, jump out when UDRE0 is 1.
void usart_transmit(uint8_t data){
	while (!(UCSR0A & (1 <<UDRE0))){
	}
	
	UDR0 = data;
}

// Converts digits to ASCII, since 0 is 48
uint8_t digit_to_ascii(uint8_t digit)
{
    return digit + 48;
}

// Extract each digit of the current number
uint8_t extract_digit(uint16_t number, uint8_t position)
{
    if (position == 0)
    {
        return number / 100;
    }
    else if (position == 1)
    {
        return (number % 100) / 10;
    }
    else
    {
        return number % 10;
    }
}