
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <irsensor/IRSensor.h>
#include <ultraSonic/ultraSonic.h>
#include <uart/uart.h>
#include <servo/servo.h>
#include <motormovement/motorMovement.h>

// Define safe-zone thresholds for the middle sensors (line following)
#define MIDDLE_LOW_THRESHOLD  550   // Lower bound (avoid completely white)
#define MIDDLE_HIGH_THRESHOLD 720   // Upper bound (avoid completely black)

// Right sensor thresholds (for guidance when line-following fails)
#define RIGHT_LOW_THRESHOLD   500   // Too light
#define RIGHT_HIGH_THRESHOLD  670   // Too dark

// Obstacle distance threshold in centimeters
#define OBSTACLE_THRESHOLD    15

// Optional: Function prototype for debugging sensor values.
void testSensor(void);

// Obstacle scanning routine that sweeps the servo from 0° to 180°
// and chooses the best angle based on ultrasonic distance.
void obstacle_avoidance_scan(void)
{
    // Stop the robot.
    stop_motors();
    _delay_ms(100);

    uint8_t bestAngle = 90;       // Default to center
    uint16_t bestDistance = 0;    // Maximum distance found

    // Sweep from 0° to 180° in 15° increments.
    for (uint8_t angle = 0; angle <= 180; angle += 15)
    {
        set_servo_angle(angle);
        _delay_ms(300);  // Allow time for the servo to reach the position.
        uint16_t d = Ultrasonic_ReadDistance();

        // Debug output for each angle.
        UART_SendString("Angle: ");
        UART_SendNumber(angle);
        UART_SendString("  Distance: ");
        UART_SendNumber(d);
        UART_SendString("\r\n");

        // Record the best angle.
        if (d > bestDistance)
        {
            bestDistance = d;
            bestAngle = angle;
        }
    }

    // Return servo to center.
    set_servo_angle(90);
    _delay_ms(300);

    // Debug output for chosen best angle.
    UART_SendString("Best Angle: ");
    UART_SendNumber(bestAngle);
    UART_SendString("  Best Distance: ");
    UART_SendNumber(bestDistance);
    UART_SendString("\r\n");

    // Decide turning direction based on best angle.
    if (bestAngle < 90)
    {
        turn_left();
        _delay_ms(500);
    }
    else if (bestAngle > 90)
    {
        turn_right();
        _delay_ms(500);
    }
    else
    {
        move_forward();
        _delay_ms(500);
    }
}

void setup(void)
{
    ADC_Init();
    Ultrasonic_Init();
    UART_Init(9600);
    Servo_Init();
    Init_Motor_IO();
}

void loop(void)
{
    // First, check for obstacles.
    uint16_t distance = Ultrasonic_ReadDistance();
    if (distance <= OBSTACLE_THRESHOLD)
    {
        obstacle_avoidance_scan();
        return;  // After scanning and turning, re-check sensors.
    }

    // Read the IR sensors for line following.
    uint16_t middleLeftSensor  = Read_IR_Sensor(MIDDLELEFTSENSORPIN);
    uint16_t middleSensor      = Read_IR_Sensor(MIDDLESENSORPIN);
    uint16_t middleRightSensor = Read_IR_Sensor(MIDDLERIGHTSENSORPIN);
    uint16_t rightSensor       = Read_IR_Sensor(RIGHTSENSORPIN);

    // Define safe values for the middle sensors.
    bool middleLeftSafe  = (middleLeftSensor  >= MIDDLE_LOW_THRESHOLD && middleLeftSensor  <= MIDDLE_HIGH_THRESHOLD);
    bool middleSafe      = (middleSensor      >= MIDDLE_LOW_THRESHOLD && middleSensor      <= MIDDLE_HIGH_THRESHOLD);
    bool middleRightSafe = (middleRightSensor >= MIDDLE_LOW_THRESHOLD && middleRightSensor <= MIDDLE_HIGH_THRESHOLD);

    // Debug output (optional).
    UART_SendString("MidL: ");
    UART_SendNumber(middleLeftSensor);
    UART_SendString(" | Mid: ");
    UART_SendNumber(middleSensor);
    UART_SendString(" | MidR: ");
    UART_SendNumber(middleRightSensor);
    UART_SendString(" | Right: ");
    UART_SendNumber(rightSensor);
    UART_SendString(" | Dist: ");
    UART_SendNumber(distance);
    UART_SendString("\r\n");

    // Decision logic for line following.
    if (middleLeftSafe || middleSafe || middleRightSafe)
    {
        move_forward();
    }
    else
    {
        if (rightSensor > RIGHT_HIGH_THRESHOLD)
        {
            turn_left();
        }
        else if (rightSensor < RIGHT_LOW_THRESHOLD)
        {
            turn_right();
        }
        else
        {
            move_forward();
        }
    }

    _delay_ms(50);
}

int main(void)
{
    setup();
    while (1)
    {
        loop();
    }
    return 0;
}

// Optional: Debug function to display sensor readings.
void testSensor(void)
{
    uint16_t leftSensor        = Read_IR_Sensor(LEFTSENSORPIN);
    uint16_t rightSensor       = Read_IR_Sensor(RIGHTSENSORPIN);
    uint16_t middleLeftSensor  = Read_IR_Sensor(MIDDLELEFTSENSORPIN);
    uint16_t middleSensor      = Read_IR_Sensor(MIDDLESENSORPIN);
    uint16_t middleRightSensor = Read_IR_Sensor(MIDDLERIGHTSENSORPIN);
    uint16_t ultraSonicSensor  = Ultrasonic_ReadDistance();

    UART_SendString("Left: ");
    UART_SendNumber(leftSensor);
    UART_SendString(" | Right: ");
    UART_SendNumber(rightSensor);
    UART_SendString(" | MidL: ");
    UART_SendNumber(middleLeftSensor);
    UART_SendString(" | Mid: ");
    UART_SendNumber(middleSensor);
    UART_SendString(" | MidR: ");
    UART_SendNumber(middleRightSensor);
    UART_SendString(" | US: ");
    UART_SendNumber(ultraSonicSensor);
    UART_SendString("\r\n");
}
