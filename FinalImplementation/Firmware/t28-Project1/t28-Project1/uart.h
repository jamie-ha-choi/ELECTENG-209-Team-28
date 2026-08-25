/*
 * uart.h
 *
 * Created: 2026/8/25 12:58:04
 *  Author: Lingwu Cai, Bowen, Ziqi Meng, Jamie
 */ 

// If UART_H_ has not been defined yet.
// Define UART_H_ to prevent repeated inclusion.
#ifndef UART_H_
#define UART_H_

// Provide fixed width integer type.
#include <stdint.h>

// Create function to initial USART.
void usart_init(uint16_t ubrr);

// Create function to transmit one byte.
void usart_transmit(uint8_t data);

// Create function to exract each digit of given number 
uint8_t extract_digit(uint16_t number, uint8_t position);

// Create function to convert digits to ASCII
uint8_t digit_to_ascii(uint8_t digit);

//end of include guard.
#endif /* UART_H_ */
