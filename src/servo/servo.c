#include "servo.h"

void Servo_Init(void) 
{
    // Configure the servo pin as output.
    DDRC |= (1 << SERVO_PIN);
    // No further setup needed if you’re using manual “bit-banging”
    // with _delay_us() / _delay_ms().
}

void set_servo_angle(uint8_t angle) 
{
    // Constrain angle to 0–180 degrees.
    if (angle > 180) angle = 180;

    // Convert angle to a pulse width in microseconds.
    // This formula maps 0° to ~1000µs, 180° to ~2000µs.
    // Adjust the multiplier (11) or offset (1000) if needed.
    uint16_t pulse_width = ((angle * 1000UL) / 180) + 1000;


    // Start the pulse: set the pin HIGH.
    PORTC |= (1 << SERVO_PIN);

    // Maintain the pulse for 'pulse_width' microseconds.
    for (uint16_t i = 0; i < pulse_width; i++) {
        _delay_us(1);
    }

    // End the pulse: set the pin LOW.
    PORTC &= ~(1 << SERVO_PIN);

    // Wait the remainder of the ~20ms servo cycle.
    _delay_ms(20);
}
