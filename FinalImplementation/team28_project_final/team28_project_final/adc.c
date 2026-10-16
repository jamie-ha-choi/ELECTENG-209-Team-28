/*
 * adc.c
 *
 * Created: 2026/10/9 16:01:38
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 

#include "adc.h"
#include "common.h"

#include <avr/io.h>
#include <stdint.h>


void adc_init(void)
{
    /*
     * PC0 = Voltage
     * PC1 = Current
     * PC2 = Offset
     *
     * Configure all as inputs.
     */
    DDRC &= ~(
        (1 << PC0) |
        (1 << PC1) |
        (1 << PC2)
    );

    PORTC &= ~(
        (1 << PC0) |
        (1 << PC1) |
        (1 << PC2)
    );

    /*
     * REFS1:0 = 00
     * External AREF reference.
     *
     * Initial channel = ADC0.
     */
    ADMUX = 0;

    /*
     * ADC clock:
     * 2 MHz / 16 = 125 kHz
     */
    ADCSRA =
        (1 << ADEN) |
        (1 << ADPS2);

    ADCSRB = 0;

    /*
     * Disable digital input buffers.
     */
    DIDR0 =
        (1 << ADC0D) |
        (1 << ADC1D) |
        (1 << ADC2D);

    // Dummy conversion after enabling ADC
    (void)adc_read(0);
}


uint16_t adc_read(uint8_t channel)
{
    /*
     * Choose ADC0, ADC1 or ADC2.
     */
    ADMUX =
        (ADMUX & 0xF0) |
        (channel & 0x0F);

    /*
     * Begin conversion.
     */
    ADCSRA |= (1 << ADSC);

    /*
     * Wait until conversion completes.
     */
    while (ADCSRA & (1 << ADSC))
    {
    }

    return ADC;
}


uint16_t adc_read_average(
    uint8_t channel,
    uint8_t samples
)
{
    uint32_t total = 0;

    if (samples == 0)
    {
        return 0;
    }

    /*
     * Discard the first conversion
     * after switching channels.
     */
    (void)adc_read(channel);

    for (uint8_t i = 0; i < samples; i++)
    {
        total += adc_read(channel);
    }

    return (uint16_t)(
        total / samples
    );
}
