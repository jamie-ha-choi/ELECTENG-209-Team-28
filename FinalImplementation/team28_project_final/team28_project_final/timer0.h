/*
 * timer0.h
 *
 * Created: 2026/10/9 16:05:59
 *  Author: Admin
 */ 


#ifndef TIMER0_H_
#define TIMER0_H_

#include <stdint.h>


void timer0_init(void);

uint16_t timer0_get_ms(void);


#endif