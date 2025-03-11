#ifndef SERVO_H_
#define SERVO_H_

#include <avr/io.h>
#include <util/delay.h>

// Servo control on pin 9
#define SERVO_DDR DDRB
#define SERVO_PIN PB1  // Pin 9 is PB1 on ATmega328P (Arduino Uno)
#define SERVO_OCnx OCR1A  // Use OCR1A for PWM

#define TOP 20000
#define SERVO_MIN 950
#define SERVO_MAX 4700
#define RANGE (SERVO_MAX - SERVO_MIN) / 180

void servo_init(void);
void set_angle(int angle);

#endif
