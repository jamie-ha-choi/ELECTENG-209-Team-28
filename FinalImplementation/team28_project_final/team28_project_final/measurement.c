/*
 * measurement.c
 *
 * Created: 2026/10/9 16:07:25
 *  Author: Admin
 */ 
#include "measurement.h"

#include "adc.h"
#include "common.h"

#include <stdint.h>


static int16_t voltage_samples[NUM_SAMPLES];

static int16_t current_samples[NUM_SAMPLES];


static uint16_t absolute_int16(int16_t value)
{
    if (value < 0)
    {
        return (uint16_t)(-value);
    }

    return (uint16_t)value;
}


/*
 * Convert signed ADC counts
 * into signed ADC-side mV.
 */

static int32_t counts_to_adc_mv(
    int16_t counts)
{
    return ((int32_t)counts
            * ADC_REF_MV)
            / 1023L;
}


/*
 * Convert ADC-side voltage signal
 * into actual source voltage.
 */

static int32_t voltage_adc_to_actual_mv(
    int32_t adc_mv)
{
    return (adc_mv
            * VOLTAGE_SCALE_NUM)
            / VOLTAGE_SCALE_DEN;
}


/*
 * Convert ADC current-conditioning
 * voltage into actual current in mA.
 */

static int32_t current_adc_to_mA(
    int32_t adc_mv)
{
    return (adc_mv * 1000L)
           / CURRENT_MV_PER_AMP;
}


void measurement_init(
    measurement_t *m)
{
    m->voltage_rms_mV = 0;

    m->current_peak_mA = 0;

    m->power_mW = 0;

    m->energy_mJ = 0;

    m->offset_adc = 0;
}


void measurement_take(
    measurement_t *m)
{
    uint16_t voltage_peak_counts = 0;

    uint16_t current_peak_counts = 0;


    /*
     * Read Offset from ADC2.
     *
     * Average several samples
     * to reduce noise.
     */

    m->offset_adc =
        adc_read_average(2, 16);


    /*
     * Throw away first ADC0 sample
     * after switching away from ADC2.
     */

    adc_read(0);


    /*
     * Sample:
     *
     * ADC0 ? Voltage
     * ADC1 ? Current
     *
     * Repeat 96 times.
     *
     * Each conversion ?104us.
     *
     * Total conversion rate ?9.6kHz.
     */

    for (uint8_t i = 0;
         i < NUM_SAMPLES;
         i++)
    {
        uint16_t raw_voltage =
            adc_read(0);

        uint16_t raw_current =
            adc_read(1);


        /*
         * Remove DC offset.
         *
         * This converts:
         *
         * approximately 2.1V-centred waveform
         *
         * back into a signed AC waveform
         * centred around 0.
         */

        voltage_samples[i] =
            (int16_t)raw_voltage
            - (int16_t)m->offset_adc;


        current_samples[i] =
            (int16_t)raw_current
            - (int16_t)m->offset_adc;


        /*
         * Find peaks.
         */

        uint16_t v_abs =
            absolute_int16(
                voltage_samples[i]
            );


        uint16_t i_abs =
            absolute_int16(
                current_samples[i]
            );


        if (v_abs > voltage_peak_counts)
        {
            voltage_peak_counts = v_abs;
        }


        if (i_abs > current_peak_counts)
        {
            current_peak_counts = i_abs;
        }
    }


    /*
     * ---------------------------
     * VOLTAGE
     * ---------------------------
     *
     * Convert ADC peak into actual
     * source peak voltage.
     */

    int32_t voltage_peak_adc_mv =
        counts_to_adc_mv(
            voltage_peak_counts
        );


    int32_t voltage_peak_actual_mv =
        voltage_adc_to_actual_mv(
            voltage_peak_adc_mv
        );


    /*
     * Source is sinusoidal.
     *
     * Vrms = Vpk / sqrt(2)
     *
     * 1/sqrt(2) ? 0.7071
     */

    m->voltage_rms_mV =
        (uint16_t)(
            (voltage_peak_actual_mv
             * 7071L)
             / 10000L
        );


    /*
     * ---------------------------
     * CURRENT
     * ---------------------------
     *
     * Specification asks for mA peak.
     */

    int32_t current_peak_adc_mv =
        counts_to_adc_mv(
            current_peak_counts
        );


    m->current_peak_mA =
        (uint16_t)(
            current_adc_to_mA(
                current_peak_adc_mv
            )
        );


    /*
     * ---------------------------
     * REAL POWER
     * ---------------------------
     *
     * Voltage and current are sampled
     * one after another.
     *
     * Therefore they are not measured
     * at exactly the same time.
     *
     * Sequence:
     *
     * V0 ? I0 ? V1 ? I1 ? ...
     *
     * I0 occurs approximately halfway
     * between V0 and V1.
     *
     * Therefore estimate voltage at I0:
     *
     * Vmid = (V0 + V1) / 2
     */

    int32_t power_sum_mW = 0;


    for (uint8_t i = 0;
         i < NUM_SAMPLES - 1;
         i++)
    {
        int16_t voltage_mid_counts =
            (
                voltage_samples[i]
                +
                voltage_samples[i + 1]
            )
            / 2;


        int32_t voltage_adc_mv =
            counts_to_adc_mv(
                voltage_mid_counts
            );


        int32_t current_adc_mv =
            counts_to_adc_mv(
                current_samples[i]
            );


        int32_t actual_voltage_mv =
            voltage_adc_to_actual_mv(
                voltage_adc_mv
            );


        int32_t actual_current_mA =
            current_adc_to_mA(
                current_adc_mv
            );


        /*
         * mV × mA / 1000
         * gives mW.
         */

        int32_t instantaneous_power_mW =
            (actual_voltage_mv
             * actual_current_mA)
             / 1000L;


        power_sum_mW +=
            instantaneous_power_mW;
    }


    m->power_mW =
        power_sum_mW
        / (NUM_SAMPLES - 1);
}


void measurement_add_energy(
    measurement_t *m,
    uint16_t elapsed_ms)
{
    /*
     * Only accumulate positive
     * consumed power.
     */

    if (m->power_mW > 0)
    {
        /*
         * mW × ms / 1000
         *
         * = mJ
         */

        m->energy_mJ +=
            (
                (uint32_t)m->power_mW
                * elapsed_ms
            )
            / 1000UL;
    }
}