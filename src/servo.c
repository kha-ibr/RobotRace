#include "servo.h"

void servo_init(void) {
    SERVO_DDR |= (1 << SERVO_PIN);  // Set pin 9 as OUTPUT

    // Timer 1 - Fast PWM mode with ICR1 as TOP, non-inverting mode
    TCCR1A = (1 << WGM11) | (1 << COM1A1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);  // Prescaler = 8

    ICR1 = TOP;  // Set TOP value for a 50Hz PWM signal
}

void set_angle(int angle) {
    SERVO_OCnx = SERVO_MIN + ((long)RANGE * angle);
}