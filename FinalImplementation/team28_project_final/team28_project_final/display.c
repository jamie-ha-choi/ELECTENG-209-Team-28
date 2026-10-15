
/*
 * display.c
 *
 * ELECTENG 209 Energy Monitor
 * Team 28
 *
 * Four-digit 7-segment display
 *
 * Display modes:
 *
 * Voltage: 14.0U
 * Current: 300A  (300 mA peak)
 * Power:   2.91P / 12.4P / 126P
 */

#include "display.h"

#include <avr/io.h>
#include <stdint.h>


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
 * Extra characters
 */

#define SEG_U       0x3E
#define SEG_A       0x77
#define SEG_P       0x73
#define SEG_MINUS   0x40
#define SEG_BLANK   0x00
#define SEG_DP      0x80


/*
 * Display array order:
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


/*
 * Initialise display pins
 */

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
     * Clock and latch LOW
     */

    PORTC &= ~(1 << PC3);

    PORTC &= ~(1 << PC5);


    /*
     * Common cathode digit selection
     * is active-low.
     *
     * 1 = digit OFF
     */

    PORTD |= (1 << PD4)
          | (1 << PD5)
          | (1 << PD6)
          | (1 << PD7);
}


/*
 * Set four display patterns
 *
 * Parameters:
 * d1 = left-most
 * d4 = right-most
 */

static void display_set_patterns(
    uint8_t d1,
    uint8_t d2,
    uint8_t d3,
    uint8_t d4)
{
    disp_characters[3] = d1;
    disp_characters[2] = d2;
    disp_characters[1] = d3;
    disp_characters[0] = d4;
}


/*
 * Refresh one digit
 *
 * Called from Timer0 interrupt
 */

void send_next_character_to_display(void)
{
    uint8_t data;

    data = disp_characters[disp_position];


    /*
     * Clock and latch LOW
     */

    PORTC &= ~(1 << PC3);

    PORTC &= ~(1 << PC5);


    /*
     * Shift 8 bits MSB first
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


        /*
         * Shift clock pulse
         */

        PORTC |= (1 << PC3);

        PORTC &= ~(1 << PC3);


        /*
         * Shift to next bit
         */

        data <<= 1;
    }


    /*
     * Turn off all digits
     */

    PORTD |= (1 << PD4)
          | (1 << PD5)
          | (1 << PD6)
          | (1 << PD7);


    /*
     * Latch new data
     */

    PORTC |= (1 << PC5);

    PORTC &= ~(1 << PC5);


    /*
     * Enable one digit
     *
     * Active-low
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
     * Move to next digit
     */

    disp_position++;


    if (disp_position >= 4)
    {
        disp_position = 0;
    }
}


/*
 * =========================================
 * DISPLAY VOLTAGE
 * =========================================
 *
 * Input: millivolts
 *
 * Example:
 *
 * 13700 mV -> 13.7U
 * 14020 mV -> 14.0U
 */

void display_voltage(uint16_t voltage_mV)
{
    /*
     * Convert mV to 0.1 V
     */

    uint16_t value =
        (voltage_mV + 50UL) / 100UL;


    uint8_t tens =
        (value / 100) % 10;

    uint8_t ones =
        (value / 10) % 10;

    uint8_t tenths =
        value % 10;


    /*
     * Display format:
     *
     * 1 3. 7 U
     */

    display_set_patterns(
        seg_pattern[tens],
        seg_pattern[ones] | SEG_DP,
        seg_pattern[tenths],
        SEG_U
    );
}


/*
 * =========================================
 * DISPLAY CURRENT
 * =========================================
 *
 * Input: mA peak
 *
 * Example:
 *
 * 300 mA -> 300A
 * 275 mA -> 275A
 *
 * Last A represents current.
 */

void display_current(uint16_t current_mA)
{
    /*
     * Maximum display = 999 mA
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
 * =========================================
 * DISPLAY REAL POWER
 * =========================================
 *
 * Input: milliwatts
 *
 * Automatic decimal point:
 *
 * 0.00 - 9.99 W:
 *   2.91P
 *
 * 10.0 - 99.9 W:
 *   12.4P
 *
 * 100 - 999 W:
 *   126P
 *
 * Negative power:
 *   ----
 *
 * Out of range:
 *   ---P
 */

void display_power(int32_t power_mW)
{
    /*
     * Negative power
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


    uint32_t p = (uint32_t)power_mW;


    /*
     * RANGE 1:
     *
     * 0.00 to 9.99 W
     *
     * Resolution = 0.01 W
     *
     * Example:
     * 2910 mW -> 2.91P
     */

    if (p < 9995UL)
    {
        uint16_t value =
            (uint16_t)((p + 5UL) / 10UL);


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


    /*
     * RANGE 2:
     *
     * 10.0 to 99.9 W
     *
     * Resolution = 0.1 W
     *
     * Example:
     * 12360 mW -> 12.4P
     */

    else if (p < 99950UL)
    {
        uint16_t value =
            (uint16_t)((p + 50UL) / 100UL);


        uint8_t tens =
            (value / 100) % 10;

        uint8_t ones =
            (value / 10) % 10;

        uint8_t decimal =
            value % 10;


        display_set_patterns(
            seg_pattern[tens],
            seg_pattern[ones] | SEG_DP,
            seg_pattern[decimal],
            SEG_P
        );
    }


    /*
     * RANGE 3:
     *
     * 100 to 999 W
     *
     * Resolution = 1 W
     *
     * Example:
     * 125700 mW -> 126P
     */

    else if (p < 999500UL)
    {
        uint16_t value =
            (uint16_t)((p + 500UL) / 1000UL);


        uint8_t hundreds =
            (value / 100) % 10;

        uint8_t tens =
            (value / 10) % 10;

        uint8_t ones =
            value % 10;


        display_set_patterns(
            seg_pattern[hundreds],
            seg_pattern[tens],
            seg_pattern[ones],
            SEG_P
        );
    }


    /*
     * Above display range
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
