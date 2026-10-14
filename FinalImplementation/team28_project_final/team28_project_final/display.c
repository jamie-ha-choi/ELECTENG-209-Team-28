/*
 * display.c
 *
 * Created: 2026/10/9 16:03:56
 *  Author: Admin
 */ 
#include "display.h"

#include <avr/io.h>


/*
 * 7-segment patterns
 *
 * bit7 = dp
 * bit6 = g
 * bit5 = f
 * bit4 = e
 * bit3 = d
 * bit2 = c
 * bit1 = b
 * bit0 = a
 */

static const uint8_t seg_pattern[10] =
{
    0x3F,     // 0
    0x06,     // 1
    0x5B,     // 2
    0x4F,     // 3
    0x66,     // 4
    0x6D,     // 5
    0x7D,     // 6
    0x07,     // 7
    0x7F,     // 8
    0x6F      // 9
};


/*
 * Extra characters.
 */

#define SEG_U       0x3E
#define SEG_A       0x77
#define SEG_P       0x73
#define SEG_MINUS   0x40
#define SEG_BLANK   0x00
#define SEG_DP      0x80


/*
 * IMPORTANT:
 *
 * Our array order is:
 *
 * [0] = right-most digit = Ds4
 * [1] = Ds3
 * [2] = Ds2
 * [3] = left-most digit = Ds1
 */

static volatile uint8_t disp_characters[4] =
{
    0, 0, 0, 0
};


static volatile uint8_t disp_position = 0;


void init_display(void)
{
    /*
     * PC3 = SH_CP
     * PC4 = SH_DS
     * PC5 = SH_ST
     */

    DDRC |= (1 << PC3)
          | (1 << PC4)
          | (1 << PC5);


    /*
     * PD4 = Ds1
     * PD5 = Ds2
     * PD6 = Ds3
     * PD7 = Ds4
     */

    DDRD |= (1 << PD4)
          | (1 << PD5)
          | (1 << PD6)
          | (1 << PD7);


    /*
     * Clock/latch LOW.
     */

    PORTC &= ~(1 << PC3);

    PORTC &= ~(1 << PC5);


    /*
     * Common cathode digit selection
     * is active-low.
     *
     * 1 = digit OFF.
     *
     * Turn all digits off initially.
     */

    PORTD |= (1 << PD4)
          | (1 << PD5)
          | (1 << PD6)
          | (1 << PD7);
}


/*
 * Helper function:
 *
 * Parameters are LEFT ? RIGHT.
 */

static void display_set_patterns(
    uint8_t d1,
    uint8_t d2,
    uint8_t d3,
    uint8_t d4)
{
    /*
     * Array is stored backwards because:
     *
     * position 0 = Ds4
     */

    disp_characters[3] = d1;
    disp_characters[2] = d2;
    disp_characters[1] = d3;
    disp_characters[0] = d4;
}


void send_next_character_to_display(void)
{
    uint8_t data;

    data = disp_characters[disp_position];


    /*
     * Start shift clock/latch LOW.
     */

    PORTC &= ~(1 << PC3);

    PORTC &= ~(1 << PC5);


    /*
     * Send 8 bits MSB first.
     */

    for (uint8_t i = 0; i < 8; i++)
    {
        /*
         * 0x80 = 10000000
         *
         * Check current MSB.
         */

        if (data & 0x80)
        {
            PORTC |= (1 << PC4);
        }
        else
        {
            PORTC &= ~(1 << PC4);
        }


        /*
         * SH_CP pulse:
         *
         * 0 ? 1 ? 0
         */

        PORTC |= (1 << PC3);

        PORTC &= ~(1 << PC3);


        /*
         * Move next bit into MSB position.
         */

        data <<= 1;
    }


    /*
     * Disable all digits first.
     */

    PORTD |= (1 << PD4)
          | (1 << PD5)
          | (1 << PD6)
          | (1 << PD7);


    /*
     * SH_ST latch pulse.
     */

    PORTC |= (1 << PC5);

    PORTC &= ~(1 << PC5);


    /*
     * Turn on ONE digit.
     *
     * Active-low.
     */

    if (disp_position == 0)
    {
        // Ds4
        PORTD &= ~(1 << PD7);
    }

    else if (disp_position == 1)
    {
        // Ds3
        PORTD &= ~(1 << PD6);
    }

    else if (disp_position == 2)
    {
        // Ds2
        PORTD &= ~(1 << PD5);
    }

    else
    {
        // Ds1
        PORTD &= ~(1 << PD4);
    }


    /*
     * Next digit.
     */

    disp_position++;


    if (disp_position >= 4)
    {
        disp_position = 0;
    }
}

/*
 * Display voltage
 *
 * Example:
 *
 * voltage_mV = 14020
 *
 * Display:
 *
 * 14.0U
 */
void display_voltage(uint16_t voltage_mV)
{
    /*
     * Convert mV to 0.1V
     *
     * 14020mV
     * -> 140
     * -> 14.0V
     */
    uint16_t value = (voltage_mV + 50) / 100;


    uint8_t tens =
        (value / 100) % 10;

    uint8_t ones =
        (value / 10) % 10;

    uint8_t tenths =
        value % 10;


    /*
     * Display:
     *
     * 1 4. 0 U
     */
    display_set_patterns(
        seg_pattern[tens],
        seg_pattern[ones] | SEG_DP,
        seg_pattern[tenths],
        SEG_U
    );
}


/*
 * Display current
 *
 * Current unit is mA peak.
 *
 * Example:
 *
 * 425mA
 *
 * Display:
 *
 * 425A
 */
void display_current(uint16_t current_mA)
{
    /*
     * We only have 3 numerical digits
     * because the last digit shows A.
     */
    if (current_mA > 999)
    {
        current_mA = 999;
    }


    uint8_t hundreds =
        (current_mA / 100) % 10;

    uint8_t tens =
        (current_mA / 10) % 10;

    uint8_t ones =
        current_mA % 10;


    display_set_patterns(
        seg_pattern[hundreds],
        seg_pattern[tens],
        seg_pattern[ones],
        SEG_A
    );
}


/*
 * Display power
 *
 * power_mW is power in milliwatts.
 *
 * Example:
 *
 * 6250mW
 *
 * -> 6.25W
 *
 * Display:
 *
 * 6.25P
 */
void display_power(int32_t power_mW)
{
    /*
     * If calculated power is negative,
     * show ---- for now.
     */
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


    /*
     * Convert mW into 0.01W.
     *
     * Example:
     *
     * 6250mW
     * -> 625
     * -> 6.25W
     */
    uint16_t value =
        (uint16_t)((power_mW + 5) / 10);


    if (value > 999)
    {
        value = 999;
    }


    uint8_t whole =
        (value / 100) % 10;

    uint8_t decimal1 =
        (value / 10) % 10;

    uint8_t decimal2 =
        value % 10;


    display_set_patterns(
        seg_pattern[whole] | SEG_DP,
        seg_pattern[decimal1],
        seg_pattern[decimal2],
        SEG_P
    );
}