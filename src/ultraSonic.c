#include "ultraSonic.h"

// void Ultrasonic_Init() {
//     DDRB |= (1 << TRIG_PIN);  // Set TRIG (PB4) as output
//     DDRB &= ~(1 << ECHO_PIN); // Set ECHO (PB5) as input

//     // Configure Timer1
//     TCCR1B |= (1 << CS11); // Set prescaler to 8 (1 tick = 0.5µs at 16MHz clock)
//     TCCR1A = 0;            // Normal mode
// }

// uint16_t Ultrasonic_ReadDistance() {
//     uint16_t duration = 0, distance;
//     uint16_t timeout = 0;

//     // Send 10µs trigger pulse
//     PORTB &= ~(1 << TRIG_PIN);
//     _delay_us(2);
//     PORTB |= (1 << TRIG_PIN);
//     _delay_us(10);
//     PORTB &= ~(1 << TRIG_PIN);

//     // Wait for echo start (with timeout)
//     TCNT1 = 0;
//     while (!(PINB & (1 << ECHO_PIN))) {
//         if (++timeout >= TIMEOUT) return 0; // No echo detected
//         _delay_us(1);
//     }

//     // Measure echo pulse width (with timeout)
//     TCNT1 = 0;
//     timeout = 0;
//     while (PINB & (1 << ECHO_PIN)) {
//         if (TCNT1 >= 60000) return 0; // Prevent timer overflow (max ~17ms)
//         if (++timeout >= TIMEOUT) return 0; // Echo too long
//         _delay_us(1);
//     }
//     duration = TCNT1;

//     // Calculate distance (cm)
//     // Timer ticks at 0.5µs (16MHz/8 prescaler)
//     // Speed of sound = 343m/s = 0.0343cm/µs
//     distance = (duration * 0.5 * 0.0343) / 2;
//     return distance;
// }

volatile uint16_t pulse_width = 0; // Stores the time taken to reach the receiver.
volatile uint8_t i = 0;

void ultrasonic_init()
{
    // Set Trigger pin as OUTPUT, and ECHO pin as INPUT.
    TRIGGER_DDR |= (1 << TRIGGER_PIN);
    ECHO_DDR &= ~(1 << ECHO_PIN);

    // Enable pin change interrupt for the echo pin.
    PCICR |= (1 << ECHO_PCICR_BIT);
    ECHO_PCMSK |= (1 << ECHO_PCINT);

    // Enable global interrupts.
    sei();

    // Configure Timer2 for CTC mode with OCR2A as the top value.
    OCR2A = 255;
    TIMSK2 |= (1 << OCIE2A); // Enable CTC interrupt
    TCCR2A |= (1 << WGM21);  // CTC mode
    TCCR2B |= TIMER2_PRESCALER_BITS; // Set prescaler
}

void trigger() 
{
    TRIGGER_PORT |= (1 << TRIGGER_PIN);
    _delay_ms(TRIGGER_FREQUENCY);
    TRIGGER_PORT &= ~(1 << TRIGGER_PIN);
}

ISR(ECHO_PCINT_vect) {
    if (!(ECHO_PORT & (1 << ECHO_PIN))) { // ECHO pin goes LOW
        pulse_width = TCNT2; // Read Timer2 value
        // Additional conditions or actions can be added here
    }
    TCNT2 = 0; // Reset Timer2
}
// Timer2 CTC interrupt handler to trigger the ultrasonic sensor.
ISR(TIMER2_COMPA_vect) {
    i = (i >= TIMER2_OVERFLOW) ? 0 : i + 1;

    if (i == 0) {
        TRIGGER_PORT |= (1 << TRIGGER_PIN); // Set Trigger HIGH
    }
    else if (i == TIMER2_OVERFLOW) {
        TRIGGER_PORT &= ~(1 << TRIGGER_PIN); // Set Trigger LOW
    }
}

uint16_t ultrasonic_read_distance() {
    trigger(); // Trigger the ultrasonic sensor
    _delay_ms(100); // Wait for the echo to return

    // Calculate distance in cm using pulse width
    uint16_t distance = (pulse_width * 0.0343) / 2; // Divide by 2 for round trip
    return distance;
}