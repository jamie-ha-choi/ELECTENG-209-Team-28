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
	
	//Character arrays storing the RMS voltage, peak current, power labels.
	char rms_text[] = "RMS Voltage is: ";
	char current_text[] = "Peak Current is: ";
	char power_text[] = "Power is: ";
	
	//Initial the USART with calculated UBRR value
	usart_init(UBRR_VALUE);
    while (1) 
    {
    }
}

