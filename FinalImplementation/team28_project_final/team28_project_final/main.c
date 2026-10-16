
/*
 * main.c
 *
 * ELECTENG 209 Energy Monitor
 * Team 28
 *
 * CPU = 2 MHz
 *
 * Display:
 * Voltage -> Current -> Power
 *
 * Each mode = 2 seconds
 */

#include "common.h"

#include "adc.h"
#include "uart.h"
#include "display.h"
#include "timer0.h"
#include "measurement.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>


/*
 * Print results to UART.
 */
static void uart_print_measurement(
    const measurement_t *m
)
{
    /*
     * RMS Voltage
     */
    uart_print("Voltage = ");

    uart_print_uint32(
        m->voltage_rms_mV / 1000
    );

    uart_transmit('.');

    uart_print_3digits(
        m->voltage_rms_mV % 1000
    );

    uart_print(" Vrms");


    /*
     * Peak Current
     */
    uart_print("   Current = ");

    uart_print_uint32(
        m->current_peak_mA
    );

    uart_print(" mApk");


    /*
     * Real Power
     */
    uart_print("   Power = ");

    int32_t p = m->power_mW;

    if (p < 0)
    {
        uart_transmit('-');
    }

    uint32_t abs_power;

    if (p < 0)
    {
        abs_power =
            (uint32_t)(-(int64_t)p);
    }
    else
    {
        abs_power = (uint32_t)p;
    }

    uart_print_uint32(
        abs_power / 1000
    );

    uart_transmit('.');

    uart_print_3digits(
        abs_power % 1000
    );

    uart_print(" W");


    /*
     * Energy
     */
    uart_print("   Energy = ");

    uart_print_uint32(
        m->energy_mJ / 1000
    );

    uart_transmit('.');

    uart_print_3digits(
        m->energy_mJ % 1000
    );

    uart_print(" J");


    /*
     * Offset diagnostic value
     */
    uart_print("   OffsetADC = ");

    uart_print_uint32(
        m->offset_adc
    );

    uart_print("\r\n");
}


/*
 * Select the display mode.
 *
 * 0 = Voltage
 * 1 = Current
 * 2 = Power
 */
static void update_display(
    uint8_t mode,
    const measurement_t *m
)
{
    if (mode == 0)
    {
        display_voltage(
            m->voltage_rms_mV
        );
    }
    else if (mode == 1)
    {
        display_current(
            m->current_peak_mA
        );
    }
    else
    {
        display_power(
            m->power_mW
        );
    }
}


int main(void)
{
    measurement_t measurement;

    uint16_t last_measure_time = 0;
    uint16_t last_display_time = 0;
    uint16_t last_uart_time = 0;
    uint16_t last_energy_time = 0;

    uint8_t display_mode = 0;


    /*
     * PD2 = Vzc input.
     *
     * Currently reserved for
     * future zero-cross synchronisation.
     */
    DDRD &= ~(1 << PD2);


    /*
     * Initialise modules.
     */
    adc_init();
    uart_init();
    init_display();
    timer0_init();

    measurement_init(&measurement);

    /*
     * Enable interrupts.
     */
    sei();


    uart_print(
        "\r\n"
        "ELECTENG209 Energy Monitor - Team28\r\n"
    );


    /*
     * Start with Voltage display.
     */
    display_voltage(0);


    while (1)
    {
        uint16_t now =
            timer0_get_ms();


        /*
         * MEASUREMENT
         *
         * Approximately every 100 ms.
         */
        if ((uint16_t)(
            now - last_measure_time
        ) >= MEASURE_INTERVAL_MS)
        {
            measurement_take(
                &measurement
            );

            uint16_t after =
                timer0_get_ms();

            uint16_t elapsed =
                (uint16_t)(
                    after - last_energy_time
                );

            measurement_add_energy(
                &measurement,
                elapsed
            );

            last_energy_time = after;
            last_measure_time = after;

            /*
             * Refresh current value
             * without changing mode.
             */
            update_display(
                display_mode,
                &measurement
            );
        }


        /*
         * DISPLAY MODE
         *
         * Change every 2 seconds.
         */
        now = timer0_get_ms();

        if ((uint16_t)(
            now - last_display_time
        ) >= DISPLAY_INTERVAL_MS)
        {
            last_display_time = now;

            display_mode++;

            if (display_mode >= 3)
            {
                display_mode = 0;
            }

            update_display(
                display_mode,
                &measurement
            );
        }


        /*
         * UART
         *
         * Print approximately
         * every 1 second.
         */
        now = timer0_get_ms();

        if ((uint16_t)(
            now - last_uart_time
        ) >= UART_INTERVAL_MS)
        {
            last_uart_time = now;

            uart_print_measurement(
                &measurement
            );
        }
    }
}
