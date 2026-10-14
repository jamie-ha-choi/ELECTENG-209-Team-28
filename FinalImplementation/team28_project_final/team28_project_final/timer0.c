/*
 * timer0.c
 *
 * Created: 2026/10/9 16:06:20
 *  Author: Admin
 */ 
#include "timer0.h"
#include "display.h"

#include <avr/io.h>
#include <avr/interrupt.h>


static volatile uint16_t system_ms = 0;


void timer0_init(void)
{
    /*
     * Timer0 CTC mode.
     *
     * CPU = 2MHz
     * Prescaler = 64
     *
     * Timer clock:
     *
     * 2MHz / 64
     * = 31.25kHz
     *
     * One timer count:
     *
     * 32us
     *
     * OCR0A = 30
     *
     * 31 counts × 32us
     * = 992us
     *
     * ? 1ms
     */

    TCCR0A = (1 << WGM01);

    TCCR0B =
          (1 << CS01)
        | (1 << CS00);


    OCR0A = 30;


    /*
     * Enable compare match A interrupt.
     */

    TIMSK0 = (1 << OCIE0A);
}


ISR(TIMER0_COMPA_vect)
{
    /*
     * Keep ISR short.
     *
     * Refresh one display digit.
     */

    send_next_character_to_display();


    /*
     * Approximate 1ms system tick.
     */

    system_ms++;
}


uint16_t timer0_get_ms(void)
{
    uint16_t value;

    uint8_t old_SREG = SREG;


    /*
     * Temporarily disable interrupts
     * because system_ms is 16-bit
     * and AVR is an 8-bit CPU.
     */

    cli();

    value = system_ms;


    /*
     * Restore interrupt state.
     */

    SREG = old_SREG;


    return value;
}