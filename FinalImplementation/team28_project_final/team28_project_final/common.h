/*
 * common.h
 *
 * Created: 2026/10/9 15:59:17
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 




#ifndef COMMON_H_
#define COMMON_H_

#define F_CPU 2000000UL

// External AREF, in millivolts
#define ADC_REF_MV 5000UL

// ADC sampling
#define NUM_SAMPLES 96U

/*
 * Voltage divider inverse gain
 *
 * R1 + R3 = 21.3k
 * R4 = 1k
 * R12 + R13 = 20k
 *
 * Inverse AC gain = 23.365
 */
#define VOLTAGE_SCALE_NUM 23365L
#define VOLTAGE_SCALE_DEN 1000L

/*
 * Current sensing
 *
 * Shunt: 1 ohm || 1 ohm = 0.5 ohm
 * U3C gain = 21.3k / 10k = 2.13
 *
 * Sensitivity = 1065 mV/A
 */
#define CURRENT_GAIN_NUM 213L
#define CURRENT_GAIN_DEN 100L

#define CURRENT_MV_PER_AMP 1065L

/*
 * Voltage selection:
 *
 * 1 = AC Source voltage at J1
 * 0 = AC Load voltage at P1
 *
 * Real Power always uses load voltage.
 */
#define DISPLAY_SOURCE_VOLTAGE 1

// Timing in milliseconds
#define MEASURE_INTERVAL_MS 100U
#define DISPLAY_INTERVAL_MS 2000U
#define UART_INTERVAL_MS 1000U

#endif
