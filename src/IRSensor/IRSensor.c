#include <irsensor/IRSensor.h>

// Initialize the ADC: use AVcc as reference and set prescaler to 128.
void ADC_Init() {
    ADMUX = (1 << REFS0); // Set AVcc as reference.
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Enable ADC, set prescaler.
}

// Read and return the ADC value from the specified channel.
uint16_t Read_IR_Sensor(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel.
    ADCSRA |= (1 << ADSC);                     // Start conversion.
    while (ADCSRA & (1 << ADSC));              // Wait until conversion finishes.
    return ADC;                                
}
