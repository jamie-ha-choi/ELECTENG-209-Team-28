/*
 * t28-Project1.c
 *
 * Created: 2026/8/25 12:31:37
 * Author : Lingwu Cai, Bowen, Ziqi Meng, Jamie
 */ 

/* Define frequency CPU to 2MHZ, using UL(unsigned long number) and
define baud rate + calculate UBRR value. */
#define F_CPU 2000000UL
#define BAUD 9600UL
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

//Give access to ATmega hardware register, enable delay, 
//provide fixed width integer, include functions created in uart.h.
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "uart.h"

// Pre-define RMSVoltage, PeakCurrent, Power
#define RMSVoltage     145
#define PeakCurrent    125
#define Power          160



int main(void)
{
	// 8 bit loop counter i for below loop.
	// Make a 16 bit variable to store number.
	uint8_t i;
	uint16_t value;
	
    usart_init(12);

	//Character arrays storing the RMS voltage, peak current, power labels.
	char rms_text[] = "RMS Voltage is: ";
	char current_text[] = "Peak Current is: ";
	char power_text[] = "Power is: ";
	
	//Initial the USART with calculated UBRR value
	usart_init(UBRR_VALUE);
    while (1) 
    {
		// RMS voltage label
		for (i = 0; rms_text[i] != '\0'; i++)
        {
            usart_transmit(rms_text[i]);
        }
		usart_transmit(digit_to_ascii(extract_digit(RMSVoltage, 0)));
        usart_transmit(digit_to_ascii(extract_digit(RMSVoltage, 1)));
        usart_transmit('.');
        usart_transmit(digit_to_ascii(extract_digit(RMSVoltage, 2)));
		usart_transmit('\r');
 		usart_transmit('\n');

		// Peak Current label
		for (i = 0; rms_text[i] != '\0'; i++)
        {
            usart_transmit(rms_text[i]);
        }
		usart_transmit(digit_to_ascii(extract_digit(PeakCurrent, 0)));
        usart_transmit(digit_to_ascii(extract_digit(PeakCurrent, 1)));
        usart_transmit(digit_to_ascii(extract_digit(PeakCurrent, 2)));
		usart_transmit('\r');
		usart_transmit('\n');
		// Power label
		for (i = 0; rms_text[i] != '\0'; i++)
        {
            usart_transmit(rms_text[i]);
        }
		usart_transmit(digit_to_ascii(extract_digit(Power, 0)));
		usart_transmit('.');
        usart_transmit(digit_to_ascii(extract_digit(Power, 1)));
        usart_transmit(digit_to_ascii(extract_digit(Power, 2)));
		usart_transmit('\r');
        usart_transmit('\n');
		_delay_ms(1000);
    }
}

