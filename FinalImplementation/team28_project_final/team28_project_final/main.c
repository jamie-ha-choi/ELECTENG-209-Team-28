
/*
 * team28_project_final.c
 *
 * ELECTENG 209 Energy Monitor
 *
 * Display:
 * Voltage -> Current -> Power
 * Each mode lasts approximately 2 seconds.
 *
 * CPU Clock: 2 MHz
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
 * Print measurement results to UART.
 */

static void uart_print_measurement(
    measurement_t *m)
{
    /*
     * Voltage RMS
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
     * Current Peak
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

    if (m->power_mW < 0)
    {
        uart_transmit('-');
    }

    uint32_t power_abs;

    if (m->power_mW < 0)
    {
        power_abs =
            (uint32_t)(-m->power_mW);
    }
    else
    {
        power_abs =
            (uint32_t)m->power_mW;
    }

    uart_print_uint32(
        power_abs / 1000
    );

    uart_transmit('.');

    uart_print_3digits(
        power_abs % 1000
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

    uart_print("\r\n");
}


/*
 * Update display according to mode.
 *
 * Mode 0 = Voltage
 * Mode 1 = Current
 * Mode 2 = Power
 */

static void update_display(
    uint8_t display_mode,
    measurement_t *m)
{
    if (display_mode == 0)
    {
        display_voltage(
            m->voltage_rms_mV
        );
    }

    else if (display_mode == 1)
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


/*
 * MAIN
 */

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
     * Not used in Version 1 yet.
     */

    DDRD &= ~(1 << PD2);


    /*
     * Initialise hardware.
     */

    adc_init();

    uart_init();

    init_display();

    timer0_init();

    measurement_init(
        &measurement
    );


    /*
     * Enable global interrupts.
     */

    sei();


    uart_print(
        "\r\n"
        "ELECTENG209 Energy Monitor\r\n"
    );


    /*
     * Initial display = Voltage.
     */

    display_voltage(0);


    /*
     * Main loop.
     */

    while (1)
    {
        uint16_t now =
            timer0_get_ms();


        /*
         * ---------------------------------
         * MEASUREMENT
         *
         * Take new measurement
         * approximately every 100 ms.
         * ---------------------------------
         */

        if ((uint16_t)
            (now - last_measure_time)
            >= 100)
        {
            measurement_take(
                &measurement
            );


            uint16_t after_measurement =
                timer0_get_ms();


            uint16_t energy_elapsed =
                (uint16_t)(
                    after_measurement
                    - last_energy_time
                );


            measurement_add_energy(
                &measurement,
                energy_elapsed
            );


            last_energy_time =
                after_measurement;


            last_measure_time =
                after_measurement;


            /*
             * Update displayed value
             * without changing mode.
             */

            update_display(
                display_mode,
                &measurement
            );
        }


        /*
         * ---------------------------------
         * DISPLAY MODE
         *
         * Change mode every ~2 seconds.
         *
         * 0-2s = Voltage
         * 2-4s = Current
         * 4-6s = Power
         *
         * Repeat.
         * ---------------------------------
         */

        now = timer0_get_ms();


        if ((uint16_t)
            (now - last_display_time)
            >= 2000)
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
         * ---------------------------------
         * UART
         *
         * Print measurements
         * approximately every 1 second.
         * ---------------------------------
         */

        if ((uint16_t)
            (now - last_uart_time)
            >= 1000)
        {
            last_uart_time = now;


            uart_print_measurement(
                &measurement
            );
        }
    }
}
