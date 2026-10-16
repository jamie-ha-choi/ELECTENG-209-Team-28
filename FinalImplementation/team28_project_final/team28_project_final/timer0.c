/*
 * timer0.c
 *
 * Created: 2026/10/9 16:06:20
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 

#include "timer0.h"
#include "display.h"
#include "common.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

static volatile uint16_t system_ms = 0;


void timer0_init(void)
{
    /*
     * Timer0 CTC mode.
     *
     * CPU = 2 MHz
     * Prescaler = 8
     * OCR0A = 249
     *
     * 2,000,000 / 8 / 250
     * = 1000 interrupts per second.
     *
     * Exactly 1 ms nominal interval.
     */
    TCCR0A = (1 << WGM01);

    TCCR0B = (1 << CS01);

    TCNT0 = 0;
    OCR0A = 249;

    /*
     * Clear pending compare flag.
     */
    TIFR0 = (1 << OCF0A);

    /*
     * Enable compare A interrupt.
     */
    TIMSK0 |= (1 << OCIE0A);
}


/*
 * Interrupt every 1 ms.
 */
ISR(TIMER0_COMPA_vect)
{
    send_next_character_to_display();

    system_ms++;
}


/*
 * Read 16-bit counter atomically.
 */
uint16_t timer0_get_ms(void)
{
    uint8_t old_SREG = SREG;

    cli();

    uint16_t value = system_ms;

    SREG = old_SREG;

    return value;
}
