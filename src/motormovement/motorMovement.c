#include <motormovement/motorMovement.h>
#include <uart/uart.h>

// Initialize motor I/O and configure Timer0 for PWM
void Init_Motor_IO(void)
{
    ML_Ctrl_DDR |= (1 << ML_Ctrl_PIN);  // Set left motor direction pin as output
    MR_Ctrl_DDR |= (1 << MR_Ctrl_PIN);  // Set right motor direction pin as output
    ML_PWM_DDR  |= (1 << ML_PWM_PIN);   // Set left motor PWM pin as output
    MR_PWM_DDR  |= (1 << MR_PWM_PIN);   // Set right motor PWM pin as output

    // Configure Timer0 for Fast PWM, non-inverting on OC0A and OC0B
    TCCR0A = (1 << WGM00) | (1 << WGM01) 
           | (1 << COM0A1) | (1 << COM0B1);
    // Set Timer0 prescaler to 64 (CPU clock/64)
    TCCR0B = (1 << CS01) | (1 << CS00);

    OCR0A = 0;  // Initialize right motor PWM duty cycle to 0 (motor off)
    OCR0B = 0;  // Initialize left motor PWM duty cycle to 0 (motor off)
}

// Move the robot forward
void move_forward(void)
{
    ML_Ctrl_PORT |= (1 << ML_Ctrl_PIN);  // Set left motor direction to forward
    MR_Ctrl_PORT |= (1 << MR_Ctrl_PIN);  // Set right motor direction to forward
    OCR0B = 160;  // Set left motor speed (PWM duty cycle)
    OCR0A = 160;  // Set right motor speed (PWM duty cycle)
}

// Turn the robot left
void turn_left(void)
{
    ML_Ctrl_PORT &= ~(1 << ML_Ctrl_PIN); // Left motor reverse
    MR_Ctrl_PORT |= (1 << MR_Ctrl_PIN);    // Right motor forward
    OCR0B = 130;  // Reduce speed on left wheel
    OCR0A = 160;  // Slightly increase right wheel speed
}

// Turn the robot right
void turn_right(void)
{
    ML_Ctrl_PORT |= (1 << ML_Ctrl_PIN);    // Left motor forward
    MR_Ctrl_PORT &= ~(1 << MR_Ctrl_PIN);     // Right motor reverse
    OCR0B = 160;  // Slightly increase left wheel speed
    OCR0A = 130;  // Reduce speed on right wheel
}

// Stop all motors
void stop_motors(void)
{
    ML_Ctrl_PORT &= ~(1 << ML_Ctrl_PIN); // Set left motor direction pin LOW
    MR_Ctrl_PORT &= ~(1 << MR_Ctrl_PIN); // Set right motor direction pin LOW
    OCR0B = 0;  // Set left motor PWM duty cycle to 0
    OCR0A = 0;  // Set right motor PWM duty cycle to 0
}
