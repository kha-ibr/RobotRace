#include <avr/io.h>
#include <util/delay.h>
#include <irsensor/IRSensor.h>
#include <ultrasonic/ultraSonic.h>
#include <uart/uart.h>
#include <servo/servo.h>
#include <motormovement/motorMovement.h>

#define THRESHOLD             400 // Between white (~40-300) and black (~500-800)
#define OBSTACLE_THRESHOLD    15


void testSensor();

void setup() 
{
    ADC_Init();
    Ultrasonic_Init();
    UART_Init(9600);
    Servo_Init();
    Init_Motor_IO();
}

void loop() 
{
    uint16_t ultra_sonic_sensor = Ultrasonic_ReadDistance(); // stop if obsticle is at 15 cm. if clear then continue

    if (ultra_sonic_sensor <= OBSTACLE_THRESHOLD) {
        stop_motors();
    }
    else {
        /** will be used later **/
        // uint16_t leftSensor = Read_IR_Sensor(LEFTSENSORPIN);
        // uint16_t rightSensor = Read_IR_Sensor(RIGHTSENSORPIN);

        uint16_t middleLeftSensor = Read_IR_Sensor(MIDDLELEFTSENSORPIN);
        uint16_t middleSensor = Read_IR_Sensor(MIDDLESENSORPIN);
        uint16_t middleRightSensor = Read_IR_Sensor(MIDDLERIGHTSENSORPIN);

        if (middleSensor > THRESHOLD) {
            // Robot is centered on the black line → Go straight
            set_servo_angle(90);
            move_forward();
        } 
        else if (middleLeftSensor > THRESHOLD) {
            // Robot is drifting right → Turn left
            set_servo_angle(45);
            turn_left();
        } 
        else if (middleRightSensor > THRESHOLD) {
            // Robot is drifting left → Turn right
            set_servo_angle(135);
            turn_right();
        } 
        else {
            // Completely off the line → Stop and recalibrate
            stop_motors();
        }
    }

    _delay_ms(50); // Small delay for stability
}

int main(void) 
{
    setup();

    while (1) {
       loop();
    }

    return 0;
}

void testSensor()
{
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

}
