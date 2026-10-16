/*
 * uart.h
 *
 * Created: 2026/10/9 16:02:29
 *  Author: Lingwu Cai, Ziqi Meng, Bowen Zhang, Jamie choi
 */ 


#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void uart_init(void);

void uart_transmit(uint8_t data);

void uart_print(const char *text);

void uart_print_uint32(uint32_t number);

void uart_print_int32(int32_t number);

void uart_print_3digits(uint16_t value);

#endif
