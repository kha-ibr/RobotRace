#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // Set pin 13 (PB5) as an output
    DDRB |= (1 << DDB5);

    while (1) {
        // Turn the LED on (set PB5 high)
        PORTB |= (1 << PORTB5);
        _delay_ms(1000); // Wait for 1 second

        // Turn the LED off (set PB5 low)
        PORTB &= ~(1 << PORTB5);
        _delay_ms(1000); // Wait for 1 second
    }

    return 0;
}
