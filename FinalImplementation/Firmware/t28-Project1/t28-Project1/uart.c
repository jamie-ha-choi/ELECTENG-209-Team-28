/*
 * uart.c
 *
 * Created: 2026/8/25 12:57:47
 *  Author: Lingwu Cai, Bowen, Ziqi Meng, Jaime
 */ 

//give access to ATmega328P hardware register.
#include <avr/io.h>
//provide fixed width integer type.
#include <stdint.h>
//include our USART function created in uart.h file.
#include "uart.h"

//Initialization USART
void usart_init(uint16_t ubrr){
	//store upper bit of ubrr in UBRR0H and store lower bits of ubrr in UBRR0L.
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;
	
	//setting UCSR0A clear all bits in UCSR0A.
	UCSR0A = 0;
	
	//setting UCSR0B set TXEN0 to enable transmitter.
	UCSR0B = (1 << TXEN0);
	
	//setting UCSR0C set UCSZ01 and UCSZ00 to set character size to 8 bit.
	UCSR0C = (1 << UCSZ01)|(1 << UCSZ00);
}

void usart_transmit(uint8_t data){
	while (!(UCSR0A & (1 <<UDRE0))){
		// do nothing when UDRE0 is 0, jump out when UDRE0 is 1.
	}
	
	UDR0 = data;
}