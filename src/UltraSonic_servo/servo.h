#ifndef SERVO_H_
#define SERVO_H_

#include <avr/io.h>
#include <util/delay.h>

#define SERVO_PIN 3

void Servo_Init();
void Servo_SetAngle(uint8_t angle);

#endif