#ifndef _IR_SENSOR_H_
#define _IR_SENSOR_H_

#include <avr/io.h>

// Define middle sensor pins on PORTC
#define MIDDLESENSORPIN         PC0  // Middle sensor
#define MIDDLERIGHTSENSORPIN    PC1  // Middle right sensor
#define MIDDLELEFTSENSORPIN     PC2  // Middle left sensor

// Define left/right sensor pins on PORTC
#define LEFTSENSORPIN           PC4  // Left sensor
#define RIGHTSENSORPIN          PC5  // Right sensor

// Initialize the ADC for IR sensor readings.
void ADC_Init();

// Read the ADC value from the specified channel.
uint16_t Read_IR_Sensor(uint8_t channel);

#endif
