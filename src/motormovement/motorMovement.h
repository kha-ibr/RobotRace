#ifndef _MOTOR_MOVEMENT_H_
#define _MOTOR_MOVEMENT_H_

#include <avr/io.h>
#include <util/delay.h>
// -------------------------
// Pin Definitions
// -------------------------
// Left Motor Control (ML_Ctrl) -> PD2
#define ML_Ctrl_DDR   DDRD
#define ML_Ctrl_PORT  PORTD
#define ML_Ctrl_PIN   2      // Bit 2 on PORTD controls left motor direction

// Left Motor PWM (ML_PWM) -> PD5 (OC0B)
#define ML_PWM_DDR   DDRD
#define ML_PWM_PORT  PORTD
#define ML_PWM_PIN   5      // Bit 5 on PORTD is used for left motor PWM (OC0B)

// Right Motor Control (MR_Ctrl) -> PD4
#define MR_Ctrl_DDR   DDRD
#define MR_Ctrl_PORT  PORTD
#define MR_Ctrl_PIN   4   // Bit 4 on PORTD controls right motor direction


// Right Motor PWM (MR_PWM) -> PD6 (OC0A)
#define MR_PWM_DDR   DDRD
#define MR_PWM_PORT  PORTD
#define MR_PWM_PIN   6   // Bit 6 on PORTD is used for right motor PWM (OC0A)

#define LEFT_SPEED	OCR0B
#define RIGHT_SPEED	OCR0A

// Function prototypes for motor control operations
void Init_Motor_IO(void);// Initialize motor I/O pins and Timer0 PWM settings
void move_forward(void);
void move_backward(void);
void turn_left(void);
void turn_right(void);
void stop_motors(void);

#endif