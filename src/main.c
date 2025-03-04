#include <avr/io.h>
#include <util/delay.h>
#include <IRSensor/IRSensor.h>
#include <UltraSonic_servo/ultraSonic.h>
#include <UART/uart.h>
#include <UltraSonic_servo/servo.h>

void setup() {
    ADC_Init();
    Ultrasonic_Init();
    UART_Init(9600);
    Servo_Init();
}

void loop() {
    uint16_t leftSensor = Read_IR_Sensor(LEFTSENSORPIN);
    uint16_t rightSensor = Read_IR_Sensor(RIGHTSENSORPIN);

    uint16_t middleLeftSensor = Read_IR_Sensor(MIDDLELEFTSENSORPIN);
    uint16_t middleSensor = Read_IR_Sensor(MIDDLESENSORPIN);
    uint16_t middleRightSensor = Read_IR_Sensor(MIDDLERIGHTSENSORPIN);

    uint16_t UltraSonicSensor = Ultrasonic_ReadDistance();

    // Send values to Serial Monitor
    UART_SendString("Left: ");
    UART_SendNumber(leftSensor);
    UART_SendString(" | Right: ");
    UART_SendNumber(rightSensor);

    UART_SendString(" | Middle Left: ");
    UART_SendNumber(middleLeftSensor);
    UART_SendString(" | Middle: ");
    UART_SendNumber(middleSensor);
    UART_SendString(" | Middle Right: ");
    UART_SendNumber(middleRightSensor);
    
    UART_SendString(" | Ultra Sonic Sensor: ");
    UART_SendNumber(UltraSonicSensor);

    UART_SendString("\r\n"); // New Line

    _delay_ms(200);
}

int main(void) {
    setup();

    // Servo_SetAngle(0);    // Move to 0°
    // _delay_ms(1000);

    // Servo_SetAngle(90);   // Move to 90°
    // _delay_ms(1000);

    // Servo_SetAngle(180);  // Move to 180°
    // _delay_ms(1000);

    while (1) {
       loop();
    }

    return 0;
}
