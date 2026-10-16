/*
 * measurement.h
 *
 * Created: 2026/10/9 16:06:56
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 


#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_

#include <stdint.h>

typedef struct
{
	uint16_t voltage_rms_mV;

	uint16_t current_peak_mA;

	int32_t power_mW;

	uint32_t energy_mJ;

	uint16_t offset_adc;

} measurement_t;


void measurement_init(
measurement_t *m
);

void measurement_take(
measurement_t *m
);

void measurement_add_energy(
measurement_t *m,
uint16_t elapsed_ms
);

#endif
