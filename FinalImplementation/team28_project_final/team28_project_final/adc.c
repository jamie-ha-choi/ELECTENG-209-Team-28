/*
 * adc.c
 *
 * Created: 2026/10/9 16:01:38
 *  Author: Admin
 */ 
#include "adc.h"
#include "common.h"

#include <avr/io.h>


void adc_init(void)
{
    /*
     * PC0 = ADC0 = Vmeas
     * PC1 = ADC1 = Imeas
     * PC2 = ADC2 = Offset
     */

    DDRC &= ~((1 << PC0)
            | (1 << PC1)
            | (1 << PC2));


    /*
     * IMPORTANT:
     *
     *PCB connects AREF directly to 5V.
     *
     * Therefore:
     *
     * REFS1 = 0
     * REFS0 = 0
     *
     * External voltage on AREF is used.
     *
     * ADLAR = 0
     * Result is right-adjusted.
     *
     * Start with ADC0.
     */

    ADMUX = 0;


    /*
     * Enable ADC.
     *
     * CPU clock = 2MHz
     *
     * Prescaler = 16
     *
     * ADC clock:
     *
     * 2MHz / 16 = 125kHz
     */

    ADCSRA = (1 << ADEN)
           | (1 << ADPS2);


    ADCSRB = 0;


    /*
     * Disable digital input buffers
     * on ADC0, ADC1 and ADC2.
     *
     * This helps reduce noise.
     */

    DIDR0 = (1 << ADC0D)
          | (1 << ADC1D)
          | (1 << ADC2D);


    /*
     * First ADC conversion after enabling
     * can take longer.
     *
     * Perform one dummy conversion.
     */

    adc_read(0);
}


uint16_t adc_read(uint8_t channel)
{
    /*
     * Clear old channel bits:
     *
     * xxxx1111
     *
     * Then insert new channel.
     */

    ADMUX = (ADMUX & 0xF0)
          | (channel & 0x0F);


    /*
     * Start conversion.
     */

    ADCSRA |= (1 << ADSC);


    /*
     * ADSC stays 1 while conversion
     * is happening.
     *
     * Wait until it becomes 0.
     */

    while (ADCSRA & (1 << ADSC))
    {
    }


    /*
     * ADC is the combined
     * ADCL + ADCH 10-bit result.
     */

    return ADC;
}


uint16_t adc_read_average(uint8_t channel,
                          uint8_t samples)
{
    uint32_t sum = 0;

    for (uint8_t i = 0; i < samples; i++)
    {
        sum += adc_read(channel);
    }

    return (uint16_t)(sum / samples);
}