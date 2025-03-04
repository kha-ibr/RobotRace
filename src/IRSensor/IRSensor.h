#ifndef _IR_SENSOR_H_
#define _IR_SENSOR_H_

#include <avr/io.h>

// Middle Sensors
#define MIDDLESENSORPIN PC0
#define MIDDLERIGHTSENSORPIN PC1
#define MIDDLELEFTSENSORPIN PC2

// Left and Right Sensors
#define LEFTSENSORPIN PC4
#define RIGHTSENSORPIN PC5


void ADC_Init();
uint16_t Read_IR_Sensor(uint8_t channel);

#endif