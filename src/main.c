#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <IRSensor.h>
#include <ultraSonic.h>
#include <uart.h>
#include <servo.h>
#include <stdio.h> // Include for sprintf
#include <motorMovement.h>

#define MIDDLE_LOW_THRESHOLD 400  // Safe Zone Lower Bound
#define MIDDLE_HIGH_THRESHOLD 770 // Safe Zone Upper Bound

#define LEFT_WHITE_THRESHOLD 410  // If left sensor is above this, turn right
#define RIGHT_BLACK_THRESHOLD 663 // If right sensor is below this, turn left

// Obstacle Detection Threshold (Adjust as Needed)
#define OBSTACLE_THRESHOLD 15 // Distance in cm

bool isMoving = false; // Flag to track robot movement state

// Define the commands to be received from the base station
#define START 'A'
#define STOP_AND_GO 'B'
#define STOP 'C'

// Test sensors for debugging.
void IR_sensor_test(void);
void sensorTest(uint16_t sensor);
void motor_movement_test();
void servo_movement_test();
void ultrasonic_test();
void sensor_test();

void follow_track(void);

// Obstacle scanning routine that sweeps the servo from 0° to 180°
// and chooses the best angle based on ultrasonic distance.
void obstacle_avoidance_scan(void);
void obstacle_avoidance(void);

void setup(void)
{
    ADC_Init();
    ultrasonic_init();
    UART_Init(9600);
    servo_init();
    Init_Motor_IO();
}

void loop(void)
{
    // Read Ultrasonic Sensor
    // uint16_t obstacle_distance = ultrasonic_read_distance();

    // if (!obstacle_distance)
    // {
    //     obstacle_avoidance();
    // }
    // else
    // {
       
    // }

    if (UCSR0A & (1 << RXC0)) // Check if data is available to read
    {
        char command = UART_Receive(); // Read received character

        // Check the command and update movement state
        if (command == 'A') // Start button
        {
            isMoving = true; // Set moving state to true
            move_forward();  // Start moving forward
        }
        else if (command == 'B') // Stop and Go button
        {
            stop_motors();   // Stop the motors
            _delay_ms(5000); // Wait for 5 seconds (optional)
            isMoving = true; // Allow moving again
            move_forward();  // Start moving forward
        }
        else if (command == 'C') // Stop button
        {
            isMoving = false; // Set moving state to false (stop the robot)
            stop_motors();    // Stop the motors immediately
        }
    }

    // If not receiving any command, the robot should follow the track if it's moving
    if (isMoving)
    {
        follow_track(); // Keep following the track if the robot is moving
    }
}

void loopTest()
{
    // Read Ultrasonic Sensor
    uint16_t distance = ultrasonic_read_distance();

    if (distance < OBSTACLE_THRESHOLD)
    {
        obstacle_avoidance();
        return;
    }

    // **Step 2: Read IR Sensor Values**
    uint16_t middleLeftSensor = Read_IR_Sensor(MIDDLELEFTSENSORPIN);
    // uint16_t middleSensor      = Read_IR_Sensor(MIDDLESENSORPIN);
    uint16_t middleRightSensor = Read_IR_Sensor(MIDDLERIGHTSENSORPIN);
    uint16_t rightSensor = Read_IR_Sensor(RIGHTSENSORPIN);
    uint16_t leftSensor = Read_IR_Sensor(LEFTSENSORPIN);

    // **Step 3: Determine if Middle Sensors are in Safe Zone**
    bool middleLeftSafe = (middleLeftSensor >= MIDDLE_LOW_THRESHOLD && middleLeftSensor <= MIDDLE_HIGH_THRESHOLD);
    // bool middleSafe      = (middleSensor      >= MIDDLE_LOW_THRESHOLD && middleSensor      <= MIDDLE_HIGH_THRESHOLD);
    bool middleRightSafe = (middleRightSensor >= MIDDLE_LOW_THRESHOLD && middleRightSensor <= MIDDLE_HIGH_THRESHOLD);

    bool safeZone = middleLeftSafe && middleRightSafe;

    // **Step 4: Check Edge Sensors**
    bool leftEdgeDetected = (leftSensor > LEFT_WHITE_THRESHOLD);    // Left sees too much white
    bool rightEdgeDetected = (rightSensor < RIGHT_BLACK_THRESHOLD); // Right sees too much black

    // **Step 6: Main Decision Logic**
    if (safeZone)
    {
        move_forward(); // Move forward if all middle sensors are safe
    }
    else if (rightEdgeDetected) // Right is too dark → Turn left
    {
        turn_right();
    }
    else if (leftEdgeDetected) // Left is too white → Turn right
    {
        turn_left();
    }
    else
    {
        stop_motors(); // Lost track, stop
    }

    _delay_ms(100);
}

int main(void)
{
    setup();

    while (1)
    {
        loop();
        // loopTest();
        // sensor_test();
        // ultrasonic_test();
    }
    return 0;
}

// Function to handle normal line-following behavior
void follow_track(void)
{
    if (isMoving)
    {
        // **Step 2: Read IR Sensor Values**
        uint16_t middleLeftSensor = Read_IR_Sensor(MIDDLELEFTSENSORPIN);
        // uint16_t middleSensor      = Read_IR_Sensor(MIDDLESENSORPIN);
        uint16_t middleRightSensor = Read_IR_Sensor(MIDDLERIGHTSENSORPIN);
        uint16_t rightSensor = Read_IR_Sensor(RIGHTSENSORPIN);
        uint16_t leftSensor = Read_IR_Sensor(LEFTSENSORPIN);

        // **Step 3: Determine if Middle Sensors are in Safe Zone**
        bool middleLeftSafe = (middleLeftSensor >= MIDDLE_LOW_THRESHOLD && middleLeftSensor <= MIDDLE_HIGH_THRESHOLD);
        // bool middleSafe      = (middleSensor      >= MIDDLE_LOW_THRESHOLD && middleSensor      <= MIDDLE_HIGH_THRESHOLD);
        bool middleRightSafe = (middleRightSensor >= MIDDLE_LOW_THRESHOLD && middleRightSensor <= MIDDLE_HIGH_THRESHOLD);

        bool safeZone = middleLeftSafe && middleRightSafe;

        // **Step 4: Check Edge Sensors**
        bool leftEdgeDetected = (leftSensor > LEFT_WHITE_THRESHOLD);    // Left sees too much white
        bool rightEdgeDetected = (rightSensor < RIGHT_BLACK_THRESHOLD); // Right sees too much black

        // **Step 6: Main Decision Logic**
        if (safeZone)
        {
            move_forward(); // Move forward if all middle sensors are safe
        }
        else if (rightEdgeDetected) // Right is too dark → Turn left
        {
            turn_right();
        }
        else if (leftEdgeDetected) // Left is too white → Turn right
        {
            turn_left();
        }
        else
        {
            stop_motors(); // Lost track, stop
        }
    }
    _delay_ms(50);
}

void obstacle_avoidance(void)
{
    stop_motors();
    _delay_ms(2000);
}

void obstacle_avoidance_scan(void)
{
    // Stop the robot.
    stop_motors();

    _delay_ms(100);

    // Reset servo position before scanning
    set_angle(90);
    _delay_ms(300);

    uint8_t bestAngle = 90;    // Default to center
    uint16_t bestDistance = 0; // Maximum distance found

    // Sweep from 0° to 180° in 15° increments.
    for (uint8_t angle = 0; angle <= 180; angle += 15)
    {
        set_angle(angle);
        _delay_ms(300); // Allow time for the servo to reach the position.
        uint16_t read_distance = ultrasonic_read_distance();

        // Record the best angle.
        if (read_distance > bestDistance)
        {
            bestDistance = read_distance;
            bestAngle = angle;
        }
    }

    // Return servo to center.
    set_angle(90);
    _delay_ms(300);

    // Decide turning direction based on best angle.
    if (bestAngle < 90)
    {
        turn_left();
        _delay_ms(700);
    }
    else if (bestAngle > 90)
    {
        turn_right();
        _delay_ms(700);
    }
}

// ------------ Test Functions ------------ //
void sensor_test()
{
    // servo movement test
    // servo_movement_test();

    // motor movement test
    // motor_movement_test();

    // ultrasonic test
    ultrasonic_test();

    // IR sensor test
    // IR_sensor_test();
}

void motor_movement_test()
{
    // Test the motor movement functions.
    move_forward();
    _delay_ms(1000);

    turn_left();
    _delay_ms(1000);

    turn_right();
    _delay_ms(1000);

    stop_motors();
    _delay_ms(1000);
}

void servo_movement_test()
{
    // Test the servo movement functions.
    set_angle(0);
    _delay_ms(1000);

    set_angle(90);
    _delay_ms(1000);

    set_angle(180);
    _delay_ms(1000);
}

void ultrasonic_test()
{
    // Test the ultrasonic sensor.
    while (1)
    {
        uint16_t distance = ultrasonic_read_distance();

        UART_SendString("Distance: ");
        UART_SendNumber(distance);
        UART_SendString(" cm\r\n");

        _delay_ms(1000);
    }
}

void IR_sensor_test(void)
{
    uint16_t leftSensor = Read_IR_Sensor(LEFTSENSORPIN);
    uint16_t rightSensor = Read_IR_Sensor(RIGHTSENSORPIN);
    uint16_t middleLeftSensor = Read_IR_Sensor(MIDDLELEFTSENSORPIN);
    uint16_t middleSensor = Read_IR_Sensor(MIDDLESENSORPIN);
    uint16_t middleRightSensor = Read_IR_Sensor(MIDDLERIGHTSENSORPIN);

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
    UART_SendString("\r\n");

    _delay_ms(1000);
}

void sensorTest(uint16_t sensor)
{
    UART_SendString("Sensor: ");
    UART_SendNumber(sensor);
    UART_SendString("\r\n");
}
