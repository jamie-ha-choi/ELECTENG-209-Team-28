/*
 * common.h
 *
 * Created: 2026/10/9 15:59:17
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 



#ifndef COMMON_H_
#define COMMON_H_

#define F_CPU 2000000UL

// ADC reference voltage (mV)
#define ADC_REF_MV 5000UL

// Number of samples
#define NUM_SAMPLES 96

/*
 * VOLTAGE CALIBRATION
 *
 * Reference = 13.7 Vrms
 * Previous display = 12.8 Vrms
 *
 * Previous scale = 22.3
 * New scale = 23.87
 */

#define VOLTAGE_SCALE_NUM 2387L
#define VOLTAGE_SCALE_DEN 100L

/*
 * CURRENT CALIBRATION
 *
 * Reference = 300 mA peak
 * Previous display = 275 mA peak
 *
 * Previous sensitivity = 1065 mV/A
 * New sensitivity = 976 mV/A
 */

#define CURRENT_MV_PER_AMP 747L

#endif
