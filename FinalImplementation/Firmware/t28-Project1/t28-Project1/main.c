/*
 * t28-Project1.c
 *
 * Created: 2026/8/25 12:31:37
 * Author : Lingwu Cai, Bowen, Ziqi Meng, Jaime
 */ 

//define frequency CPU to 2MHZ,using UL(unsigned long number).
#define F_CPU 2000000UL
//give access to ATmega hardware register.
#include <avr/io.h>
//enable delay.
#include <util/delay.h>
//provide fixed width integer.
#include<stdint.h>
//include functions crated in uart.h.
#include "uart.h"

// pre-define RMSVoltage,PeakCurrent,Power
#define RMSVoltage     14.5
#define PeakCurrent    125
#define Power          1.60


int main(void)
{
	//8 bit loop counter i for below loop.
	uint8_t i;
	//make a 16 bit variable to store number.
	uint16_t value;
	
	//Character array storing the RMS voltage label.
	char rms_text[] = "RMS Voltage is: ";
	//character array storing the Peak current label.
	char current_text[] = "Peak Current is: ";
	//character array storing the power label.
	char power_text[] = "Power is: ";
    while (1) 
    {
    }
}

