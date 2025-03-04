#ifndef SERVO_H_
#define SERVO_H_

#include <avr/io.h>
#include <util/delay.h>

#define SERVO_PIN PC3

void Servo_Init();
void variable_delay_ms(uint16_t ms);
void variable_delay_us(uint16_t us);
void set_servo_angle(uint8_t angle);

#endif