/*
 * common.h
 *
 * Created: 2026/10/9 15:59:17
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 


#ifndef COMMON_H_
#define COMMON_H_

#define F_CPU 2000000UL

// ADC reference voltage on your PCB
#define ADC_REF_MV 5000UL

// Number of voltage/current samples per measurement block
#define NUM_SAMPLES 96

#define VOLTAGE_SCALE_NUM 223L
#define VOLTAGE_SCALE_DEN 10L

#define CURRENT_MV_PER_AMP 1065L

#endif