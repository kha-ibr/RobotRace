#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdlib.h>

// Function Prototypes
void UART_Init(uint16_t baudrate);
void UART_Transmit(char data);
void UART_SendString(const char *str);
void UART_SendNumber(uint16_t num);
char UART_Receive(void);

#endif
