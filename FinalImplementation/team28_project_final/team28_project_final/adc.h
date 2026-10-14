/*
 * adc.h
 *
 * Created: 2026/10/9 16:01:08
 *  Author: Admin
 */ 

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

void adc_init(void);

uint16_t adc_read(uint8_t channel);

uint16_t adc_read_average(uint8_t channel,
uint8_t samples);

#endif