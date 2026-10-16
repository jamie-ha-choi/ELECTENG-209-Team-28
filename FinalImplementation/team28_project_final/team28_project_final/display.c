
/*
 * display.c
 * ELECTENG 209 - Team 28
 */

#include "display.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

/*
 * Segment patterns 0 to 9.
 */
static const uint8_t seg_pattern[10] =
{
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

#define SEG_U       0x3E
#define SEG_A       0x77
#define SEG_P       0x73
#define SEG_MINUS   0x40
#define SEG_DP      0x80

/*
 * Position 0 = rightmost digit.
 * Position 3 = leftmost digit.
 */
static volatile uint8_t disp_characters[4] =
{
    0, 0, 0, 0
};

static volatile uint8_t disp_position = 0;


void init_display(void)
{
    /*
     * PC3 = Clock
     * PC4 = Data
     * PC5 = Latch
     */
    DDRC |=
        (1 << PC3) |
        (1 << PC4) |
        (1 << PC5);

    /*
     * PD4-PD7 = digit selection.
     */
    DDRD |=
        (1 << PD4) |
        (1 << PD5) |
        (1 << PD6) |
        (1 << PD7);

    PORTC &= ~(
        (1 << PC3) |
        (1 << PC5)
    );

    /*
     * Active-low digit selection.
     * HIGH = OFF.
     */
    PORTD |=
        (1 << PD4) |
        (1 << PD5) |
        (1 << PD6) |
        (1 << PD7);
}


/*
 * Update all four characters together.
 *
 * Temporarily disable interrupts
 * to prevent half-updated displays.
 */
static void display_set_patterns(
    uint8_t d1,
    uint8_t d2,
    uint8_t d3,
    uint8_t d4
)
{
    uint8_t old_SREG = SREG;

    cli();

    disp_characters[3] = d1;
    disp_characters[2] = d2;
    disp_characters[1] = d3;
    disp_characters[0] = d4;

    SREG = old_SREG;
}


/*
 * Refresh one display digit.
 *
 * Called once per Timer0 interrupt.
 */
void send_next_character_to_display(void)
{
    uint8_t data =
        disp_characters[disp_position];

    PORTC &= ~(1 << PC3);
    PORTC &= ~(1 << PC5);

    /*
     * Send 8 bits MSB first.
     */
    for (uint8_t i = 0; i < 8; i++)
    {
        if (data & 0x80)
        {
            PORTC |= (1 << PC4);
        }
        else
        {
            PORTC &= ~(1 << PC4);
        }

        PORTC |= (1 << PC3);
        PORTC &= ~(1 << PC3);

        data <<= 1;
    }

    /*
     * Turn all digits OFF.
     */
    PORTD |=
        (1 << PD4) |
        (1 << PD5) |
        (1 << PD6) |
        (1 << PD7);

    /*
     * Latch the new segment data.
     */
    PORTC |= (1 << PC5);
    PORTC &= ~(1 << PC5);

    /*
     * Select one digit.
     */
    switch (disp_position)
    {
        case 0:
            PORTD &= ~(1 << PD7);
            break;

        case 1:
            PORTD &= ~(1 << PD6);
            break;

        case 2:
            PORTD &= ~(1 << PD5);
            break;

        default:
            PORTD &= ~(1 << PD4);
            break;
    }

    disp_position++;

    if (disp_position >= 4)
    {
        disp_position = 0;
    }
}


/*
 * VOLTAGE
 *
 * 13700 mV -> 13.7U
 */
void display_voltage(uint16_t voltage_mV)
{
    uint16_t value =
        (uint16_t)(
            ((uint32_t)voltage_mV + 50UL)
            / 100UL
        );

    /*
     * Three digits allow up to 99.9 V.
     */
    if (value > 999)
    {
        display_set_patterns(
            SEG_MINUS,
            SEG_MINUS,
            SEG_MINUS,
            SEG_U
        );
        return;
    }

    display_set_patterns(
        seg_pattern[(value / 100) % 10],
        seg_pattern[(value / 10) % 10] | SEG_DP,
        seg_pattern[value % 10],
        SEG_U
    );
}


/*
 * CURRENT
 *
 * 376 mA peak -> 376A
 *
 * The display value is in mA,
 * not amperes.
 */
void display_current(uint16_t current_mA)
{
    if (current_mA > 999)
    {
        display_set_patterns(
            SEG_MINUS,
            SEG_MINUS,
            SEG_MINUS,
            SEG_A
        );
        return;
    }

    display_set_patterns(
        seg_pattern[(current_mA / 100) % 10],
        seg_pattern[(current_mA / 10) % 10],
        seg_pattern[current_mA % 10],
        SEG_A
    );
}


/*
 * REAL POWER
 *
 * 3.64 W  -> 3.64P
 * 12.36 W -> 12.4P
 * 125.7 W -> 126P
 */
void display_power(int32_t power_mW)
{
    if (power_mW < 0)
    {
        display_set_patterns(
            SEG_MINUS,
            SEG_MINUS,
            SEG_MINUS,
            SEG_MINUS
        );
        return;
    }

    uint32_t p = (uint32_t)power_mW;

    /*
     * Range 1: 0.00 to 9.99 W
     */
    if (p < 9995UL)
    {
        uint16_t value =
            (uint16_t)((p + 5UL) / 10UL);

        display_set_patterns(
            seg_pattern[value / 100] | SEG_DP,
            seg_pattern[(value / 10) % 10],
            seg_pattern[value % 10],
            SEG_P
        );
    }

    /*
     * Range 2: 10.0 to 99.9 W
     */
    else if (p < 99950UL)
    {
        uint16_t value =
            (uint16_t)((p + 50UL) / 100UL);

        display_set_patterns(
            seg_pattern[value / 100],
            seg_pattern[(value / 10) % 10] | SEG_DP,
            seg_pattern[value % 10],
            SEG_P
        );
    }

    /*
     * Range 3: 100 to 999 W
     */
    else if (p < 999500UL)
    {
        uint16_t value =
            (uint16_t)((p + 500UL) / 1000UL);

        display_set_patterns(
            seg_pattern[value / 100],
            seg_pattern[(value / 10) % 10],
            seg_pattern[value % 10],
            SEG_P
        );
    }

    /*
     * Out of display range.
     */
    else
    {
        display_set_patterns(
            SEG_MINUS,
            SEG_MINUS,
            SEG_MINUS,
            SEG_P
        );
    }
}
