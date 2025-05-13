// #ifndef ULTRASONIC_H_
// #define ULTRASONIC_H_

// #include <avr/io.h>
// #include <util/delay.h>

// // Define Trigger and Echo Pins
// #define TRIG_PIN PB4
// #define ECHO_PIN PB5

// // Timeout value for waiting loops (approx 15ms)
// #define TIMEOUT 30000

// // Function Prototypes
// void Ultrasonic_Init();
// uint16_t Ultrasonic_ReadDistance();

// #endif


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define TRIGGER_DDR DDRB
#define TRIGGER_PORT PORTB
#define TRIGGER_PIN 4
#define TRIGGER_FREQUENCY 10 // Manual trigger delay between Trigger HIGH and LOW.

#define ECHO_DDR DDRB
#define ECHO_PORT PORTB
#define ECHO_PIN 5
#define ECHO_PCINT PCINT5
#define ECHO_PCICR_BIT PCIE0
#define ECHO_PCMSK PCMSK0
#define ECHO_PCINT_vect PCINT0_vect

#define TIMER2_PRESCALER_BITS ((1 << CS22) | (1 << CS21) | (1 << CS20)) // 1024 prescaler
#define TIMER2_OVERFLOW 10

void ultrasonic_init();
void trigger();
uint16_t ultrasonic_read_distance();