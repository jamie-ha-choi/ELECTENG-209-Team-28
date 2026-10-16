/*
 * measurement.c
 *
 * Created: 2026/10/9 16:07:25
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 


/*
 * measurement.c
 * ELECTENG 209 - Team 28
 *
 * Optimised fixed-point version for 2 MHz AVR.
 *
 * True RMS Voltage
 * Peak Current
 * Real Power
 * Energy
 */

#include "measurement.h"
#include "adc.h"
#include "common.h"

#include <stdint.h>
#include <limits.h>

/*
 * Raw ADC samples.
 */
static uint16_t v_raw[NUM_SAMPLES];
static uint16_t i_raw[NUM_SAMPLES];

/*
 * Precomputed gain coefficients.
 *
 * Q4  = value multiplied by 16.
 * Q8  = value multiplied by 256.
 *
 * Compiler evaluates these constants.
 */

#define V_DEN \
    (1023ULL * VOLTAGE_SCALE_DEN)

#define VI_DEN \
    (1023ULL * VOLTAGE_SCALE_DEN \
     * CURRENT_GAIN_NUM)

#define V_GAIN_Q4 \
    ((int32_t)( \
        ((uint64_t)ADC_REF_MV \
         * VOLTAGE_SCALE_NUM * 16ULL \
         + V_DEN / 2ULL) / V_DEN))

#define I_SOURCE_GAIN_Q4 \
    ((int32_t)( \
        ((uint64_t)ADC_REF_MV \
         * VOLTAGE_SCALE_NUM \
         * CURRENT_GAIN_DEN * 16ULL \
         + VI_DEN / 2ULL) / VI_DEN))

#define I_LOAD_GAIN_Q4 \
    ((int32_t)( \
        ((uint64_t)ADC_REF_MV \
         * (VOLTAGE_SCALE_NUM - VOLTAGE_SCALE_DEN) \
         * CURRENT_GAIN_DEN * 16ULL \
         + VI_DEN / 2ULL) / VI_DEN))

#define CURRENT_DEN \
    (1023ULL * CURRENT_MV_PER_AMP)

#define CURRENT_MA_Q8 \
    ((int32_t)( \
        ((uint64_t)ADC_REF_MV \
         * 1000ULL * 256ULL \
         + CURRENT_DEN / 2ULL) \
        / CURRENT_DEN))


/*
 * Integer square root.
 */
static uint32_t integer_sqrt(uint64_t n)
{
    uint64_t result = 0;
    uint64_t bit = (uint64_t)1 << 62;

    while (bit > n)
    {
        bit >>= 2;
    }

    while (bit != 0)
    {
        if (n >= result + bit)
        {
            n -= result + bit;
            result = (result >> 1) + bit;
        }
        else
        {
            result >>= 1;
        }

        bit >>= 2;
    }

    return (uint32_t)result;
}


/*
 * Reconstruct source or load voltage.
 *
 * Inputs are centred ADC counts in Q4.
 *
 * 1 = Source Voltage
 * 0 = Load Voltage
 *
 * Output: millivolts.
 */
static int32_t voltage_from_adc_q4(
    int32_t v_counts_q4,
    int32_t i_counts_q4,
    uint8_t source
)
{
    int32_t i_gain;

    if (source)
    {
        i_gain = I_SOURCE_GAIN_Q4;
    }
    else
    {
        i_gain = I_LOAD_GAIN_Q4;
    }

    int32_t result =
        v_counts_q4 * V_GAIN_Q4
        + i_counts_q4 * i_gain;

    return result / 256L;
}


/*
 * Convert centred current ADC counts
 * into actual current in mA.
 */
static int32_t current_from_adc_q4(
    int32_t i_counts_q4
)
{
    return
        (i_counts_q4 * CURRENT_MA_Q8)
        / 4096L;
}


/*
 * Initialise measurements.
 */
void measurement_init(measurement_t *m)
{
    m->voltage_rms_mV = 0;
    m->current_peak_mA = 0;
    m->power_mW = 0;
    m->energy_mJ = 0;
    m->offset_adc = 0;
}


/*
 * Acquire and calculate measurements.
 */
void measurement_take(measurement_t *m)
{
    uint32_t sum_v = 0;
    uint32_t sum_i = 0;

    uint16_t min_i = 1023;
    uint16_t max_i = 0;

    /*
     * 1. Read TP7 Offset for diagnostics.
     */
    m->offset_adc =
        adc_read_average(2, 16);

    /*
     * 2. Prepare ADC0.
     */
    (void)adc_read(0);

    /*
     * 3. Capture voltage and current.
     */
    for (uint8_t n = 0; n < NUM_SAMPLES; n++)
    {
        v_raw[n] = adc_read(0);
        i_raw[n] = adc_read(1);

        sum_v += v_raw[n];
        sum_i += i_raw[n];

        if (i_raw[n] < min_i)
        {
            min_i = i_raw[n];
        }

        if (i_raw[n] > max_i)
        {
            max_i = i_raw[n];
        }
    }

    /*
     * 4. Separate DC centre for
     * voltage and current channels.
     *
     * Q4 = ADC count x 16.
     */
    int32_t v_mean_q4 =
        (int32_t)(
            (sum_v * 16UL) / NUM_SAMPLES
        );

    int32_t i_mean_q4 =
        (int32_t)(
            (sum_i * 16UL) / NUM_SAMPLES
        );

    /*
     * 5. TRUE RMS Voltage.
     *
     * Interpolate current to the
     * voltage sampling time.
     */
    uint64_t voltage_sum_sq = 0;

    for (uint8_t n = 1; n < NUM_SAMPLES; n++)
    {
        int32_t v_q4 =
            (int32_t)v_raw[n] * 16L
            - v_mean_q4;

        int32_t i_q4 =
            (
                (int32_t)i_raw[n - 1]
                + (int32_t)i_raw[n]
            ) * 8L
            - i_mean_q4;

        int32_t voltage_mV =
            voltage_from_adc_q4(
                v_q4,
                i_q4,
                DISPLAY_SOURCE_VOLTAGE
            );

        voltage_sum_sq +=
            (uint64_t)(
                (int64_t)voltage_mV
                * voltage_mV
            );
    }

    uint32_t voltage_rms =
        integer_sqrt(
            voltage_sum_sq
            / (NUM_SAMPLES - 1U)
        );

    if (voltage_rms > UINT16_MAX)
    {
        voltage_rms = UINT16_MAX;
    }

    m->voltage_rms_mV =
        (uint16_t)voltage_rms;

    /*
     * 6. PEAK CURRENT.
     *
     * Vpp = max - min.
     * Peak = Vpp / 2.
     *
     * CURRENT_MA_Q8 converts ADC
     * counts into mA.
     */
    uint32_t pp_counts =
        (uint32_t)max_i - min_i;

    uint32_t peak_current =
        (
            pp_counts * (uint32_t)CURRENT_MA_Q8
            + 256UL
        ) / 512UL;

    if (peak_current > UINT16_MAX)
    {
        peak_current = UINT16_MAX;
    }

    m->current_peak_mA =
        (uint16_t)peak_current;

    /*
     * 7. REAL POWER.
     *
     * Always use actual LOAD voltage,
     * not source voltage.
     *
     * Interpolate voltage to the
     * current sampling time.
     */
    int64_t power_sum = 0;

    for (uint8_t n = 0;
         n < NUM_SAMPLES - 1U;
         n++)
    {
        int32_t v_mid_q4 =
            (
                (int32_t)v_raw[n]
                + (int32_t)v_raw[n + 1]
            ) * 8L
            - v_mean_q4;

        int32_t i_q4 =
            (int32_t)i_raw[n] * 16L
            - i_mean_q4;

        int32_t load_voltage_mV =
            voltage_from_adc_q4(
                v_mid_q4,
                i_q4,
                0
            );

        int32_t current_mA =
            current_from_adc_q4(i_q4);

        /*
         * mV x mA / 1000 = mW.
         */
        int64_t instantaneous_power =
            (
                (int64_t)load_voltage_mV
                * current_mA
            ) / 1000LL;

        power_sum += instantaneous_power;
    }

    int64_t average_power =
        power_sum / (NUM_SAMPLES - 1U);

    if (average_power > INT32_MAX)
    {
        average_power = INT32_MAX;
    }

    if (average_power < INT32_MIN)
    {
        average_power = INT32_MIN;
    }

    m->power_mW = (int32_t)average_power;
}


/*
 * 8. ENERGY.
 *
 * E = Integral(P dt).
 */
void measurement_add_energy(
    measurement_t *m,
    uint16_t elapsed_ms
)
{
    if (m->power_mW <= 0)
    {
        return;
    }

    uint64_t added_energy =
        (
            (uint64_t)m->power_mW
            * elapsed_ms
        ) / 1000ULL;

    uint64_t new_energy =
        (uint64_t)m->energy_mJ
        + added_energy;

    if (new_energy > UINT32_MAX)
    {
        new_energy = UINT32_MAX;
    }

    m->energy_mJ = (uint32_t)new_energy;
}
