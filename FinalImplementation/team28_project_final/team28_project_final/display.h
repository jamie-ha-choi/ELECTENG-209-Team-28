/*
 * display.h
 *
 * Created: 2026/10/9 16:03:29
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

void init_display(void);

void send_next_character_to_display(void);

void display_voltage(uint16_t voltage_mV);

void display_current(uint16_t current_mA);

void display_power(int32_t power_mW);

#endif
