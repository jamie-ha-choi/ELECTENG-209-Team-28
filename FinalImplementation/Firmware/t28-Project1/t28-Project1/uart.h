/*
 * uart.h
 *
 * Created: 2026/8/25 12:58:04
 *  Author: Lingwu Cai, Bowen, Ziqi Meng, Jaime
 */ 

// If UART_H_ has not been defined yet.
// Define UART_H_ to prevent repeated inclusion.
#ifndef UART_H_
#define UART_H_
// provide fixed width integer type.
#include <stdint.h>

//create a function to initial USART.
void usart_init(uint16_t ubrr);

//create a function to transmit one byte.
void usart_transmit(uint8_t data);

//end of include guard.
#endif /* UART_H_ */