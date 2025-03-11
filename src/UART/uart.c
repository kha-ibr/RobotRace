#include <uart/uart.h>

void UART_Init(uint16_t baudrate) 
{
    uint16_t ubrr_value = ((F_CPU / (baudrate * 16UL)) - 1);  // Calculate UBRR
    UBRR0H = (ubrr_value >> 8);  // Set baud rate high byte
    UBRR0L = ubrr_value;  // Set baud rate low byte
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);  // Enable Transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8-bit data, 1 stop bit
}

void UART_Transmit(char data) 
{
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for buffer to be empty
    UDR0 = data;  // Send data
}

void UART_SendString(const char *str) 
{
    while (*str) {
        UART_Transmit(*str++);
    }
}

// Used for the ultrasonic sensor
void UART_SendNumber(uint16_t num) 
{
    char buffer[10];
    itoa(num, buffer, 10);  // Convert number to string
    UART_SendString(buffer);
}

char UART_Receive(void)
{
    while (!(UCSR0A & (1 << RXC0)));  // Wait for data to be received
    return UDR0;  // Get and return received data from buffer
}

