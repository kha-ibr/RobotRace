#include <irsensor/IRSensor.h>

void ADC_Init() {
    ADMUX = (1 << REFS0);
    
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t Read_IR_Sensor(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC));

    return ADC;
}