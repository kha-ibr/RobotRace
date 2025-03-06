#ifndef SERVO_H_
#define SERVO_H_

#include <avr/io.h>
#include <util/delay.h>

// Choose whichever pin you want for the servo signal.
// In your setup, it's PC3 (A3 on some boards).
#define SERVO_PIN PC3

// Initialize the servo pin and any needed resources.
void Servo_Init(void);

// Set the servo angle (0–180).
void set_servo_angle(uint8_t angle);

#endif
