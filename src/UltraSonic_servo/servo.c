#include <UltraSonic_servo/servo.h>

void Servo_Init() {
    DDRC |= (1 << SERVO_PIN);  // Set A3 (PC3) as output
}

void Servo_SetAngle(uint8_t angle) {
    if (angle > 180) angle = 180;  // Limit angle

    uint16_t pulse_width = (angle * 11) + 1000;  // Convert angle to 1000-2000µs pulse
    
    PORTC |= (1 << SERVO_PIN);  // Set pin HIGH
    _delay_us(pulse_width);  // Hold HIGH for pulse width
    PORTC &= ~(1 << SERVO_PIN);  // Set pin LOW
    _delay_ms(20);  // Wait for 20ms (servo update cycle)
}