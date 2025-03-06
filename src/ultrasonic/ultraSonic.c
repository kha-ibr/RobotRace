#include "ultraSonic.h"

void Ultrasonic_Init() {
    DDRB |= (1 << TRIG_PIN);  // Set TRIG (PB4) as output
    DDRB &= ~(1 << ECHO_PIN); // Set ECHO (PB5) as input

    // Configure Timer1
    TCCR1B |= (1 << CS11); // Set prescaler to 8 (1 tick = 0.5µs at 16MHz clock)
    TCCR1A = 0;            // Normal mode
}

uint16_t Ultrasonic_ReadDistance() {
    uint16_t duration, distance;

    // Send 10µs pulse to trigger
    PORTB &= ~(1 << TRIG_PIN);
    _delay_us(2);
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);

    // Wait for echo start
    while (!(PINB & (1 << ECHO_PIN))); // Wait until ECHO goes HIGH
    TCNT1 = 0;  // Reset Timer1

    // Measure echo pulse width
    while (PINB & (1 << ECHO_PIN)); // Wait until ECHO goes LOW
    duration = TCNT1;

    // Convert time to distance (Speed of sound = 343m/s)
    // (duration/2 gives the one-way travel time in timer ticks; adjust conversion as needed)
    distance = (duration / 2) * 0.0343;

    return distance;  // Return distance in cm
}
