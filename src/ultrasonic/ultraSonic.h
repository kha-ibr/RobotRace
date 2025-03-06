#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include <avr/io.h>
#include <util/delay.h>

// Define Trigger and Echo Pins
#define TRIG_PIN PB4
#define ECHO_PIN PB5

// Function Prototypes
void Ultrasonic_Init();
uint16_t Ultrasonic_ReadDistance();

#endif
