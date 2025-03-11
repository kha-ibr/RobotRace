
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <irsensor/IRSensor.h>
#include <ultraSonic/ultraSonic.h>
#include <uart/uart.h>
#include <servo/servo.h>
#include <motormovement/motorMovement.h>

// // Define Safe Zone for Middle Sensors
// #define MIDDLE_LOW_THRESHOLD  400   // Below this is too white (off-track)
// #define MIDDLE_HIGH_THRESHOLD 750   // Above this is too dark (off-track)

// // Define Edge Detection for Left and Right Sensors
// #define LEFT_WHITE_THRESHOLD  407   // Above this, left sensor sees white (turn right)
// #define RIGHT_BLACK_THRESHOLD 583   // Below this, right sensor sees black (turn left)

#define MIDDLE_LOW_THRESHOLD  400   // Safe Zone Lower Bound
#define MIDDLE_HIGH_THRESHOLD 770   // Safe Zone Upper Bound

#define LEFT_WHITE_THRESHOLD  415   // If left sensor is above this, turn right
#define RIGHT_BLACK_THRESHOLD 648   // If right sensor is below this, turn left

// Obstacle Detection Threshold (Adjust as Needed)
#define OBSTACLE_THRESHOLD 10  // Distance in cm

// Define the commands to be received from the base station
#define START       'A'
#define STOP_AND_GO 'B'
#define STOP        'C'

// Test sensors for debugging.
void IR_sensor_test(void);
void sensorTest(uint16_t sensor);
void motor_movement_test();
void servo_movement_test();
void ultrasonic_test();

// Command receiving function that reads a command from the Bluetooth module.
void recieveCommand();
void getbluetoothCommand();

// Function to send sensor data over UART for debugging.
void sendIRData(uint16_t middleLeftSensor, uint16_t middleSensor, 
    uint16_t middleRightSensor, uint16_t rightSensor, uint16_t distance);

// Obstacle scanning routine that sweeps the servo from 0° to 180°
// and chooses the best angle based on ultrasonic distance.
void obstacle_avoidance_scan(void);

void setup(void)
{
    ADC_Init();
    Ultrasonic_Init();
    UART_Init(9600);
    servo_init();
    Init_Motor_IO();
}

void loop(void)
{
    // **Step 1: Check for Obstacles**
    // uint16_t distance = Ultrasonic_ReadDistance();
    // if (distance <= OBSTACLE_THRESHOLD)
    // {
    //     obstacle_avoidance_scan();
    //     return;
    // }

    // **Step 2: Read IR Sensor Values**
    uint16_t middleLeftSensor  = Read_IR_Sensor(MIDDLELEFTSENSORPIN);
    // uint16_t middleSensor      = Read_IR_Sensor(MIDDLESENSORPIN);
    uint16_t middleRightSensor = Read_IR_Sensor(MIDDLERIGHTSENSORPIN);
    uint16_t rightSensor       = Read_IR_Sensor(RIGHTSENSORPIN);
    uint16_t leftSensor        = Read_IR_Sensor(LEFTSENSORPIN);

    // **Step 3: Determine if Middle Sensors are in Safe Zone**
    bool middleLeftSafe  = (middleLeftSensor  >= MIDDLE_LOW_THRESHOLD && middleLeftSensor  <= MIDDLE_HIGH_THRESHOLD);
    // bool middleSafe      = (middleSensor      >= MIDDLE_LOW_THRESHOLD && middleSensor      <= MIDDLE_HIGH_THRESHOLD);
    bool middleRightSafe = (middleRightSensor >= MIDDLE_LOW_THRESHOLD && middleRightSensor <= MIDDLE_HIGH_THRESHOLD);

    bool safeZone = middleLeftSafe && middleRightSafe;

    // **Step 4: Check Edge Sensors**
    bool leftEdgeDetected  = (leftSensor > LEFT_WHITE_THRESHOLD);  // Left sees too much white
    bool rightEdgeDetected = (rightSensor < RIGHT_BLACK_THRESHOLD); // Right sees too much black

    // **Step 5: Send Sensor Data to Bluetooth for Debugging**
    //sendIRData(middleLeftSensor, middleSensor, middleRightSensor, rightSensor, leftSensor, distance);
    //sensorTest(leftEdgeDetected);

    // **Step 6: Main Decision Logic**
    if (safeZone)
    {
        move_forward();  // Move forward if all middle sensors are safe
    }
    else if (rightEdgeDetected)  // Right is too dark → Turn left
    {
        turn_right();
    }
    else if (leftEdgeDetected)   // Left is too white → Turn right
    {
        turn_left();
    }
    else
    {
        stop_motors();  // Lost track, stop
    }

    _delay_ms(50);
}

void loopTest() {
    set_angle(90);
    _delay_ms(1000);
}

int main(void)
{
    setup();
    while (1)
    {
        loop();
        // loopTest();
    }
    return 0;
}

void sensor_test()
{
    // servo movement test
    // servo_movement_test();

    // motor movement test
    // motor_movement_test();

    // ultrasonic test
    // ultrasonic_test();

    // IR sensor test
    // IR_sensor_test();
}

void sendIRData(uint16_t middleLeftSensor, uint16_t middleSensor, uint16_t middleRightSensor, uint16_t rightSensor, uint16_t distance)
{
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
}

void recieveCommand() 
{
    // **Step 1: Check for Bluetooth Commands**
    char command = UART_Receive();  

    if (command == START)
    {
        UART_SendString("\nCommand: START\n");
        move_forward();
    }
    else if (command == STOP_AND_GO)
    {
        UART_SendString("\nCommand: STOP_AND_GO\n");
        stop_motors();
        _delay_ms(5000);  // Pause for 5 seconds
        move_forward();  // Resume movement
    }
    else if (command == STOP)
    {
        UART_SendString("\nCommand: STOP\n");
        stop_motors();
    }

    _delay_ms(50);  
}

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
        set_angle(angle);
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
    set_angle(90);
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

// Sensor test functions for debugging.
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
        uint16_t distance = Ultrasonic_ReadDistance();
        UART_SendString("Distance: ");
        UART_SendNumber(distance);
        UART_SendString(" cm\r\n");
        _delay_ms(1000);
    }
}

void IR_sensor_test(void)
{
    uint16_t leftSensor        = Read_IR_Sensor(LEFTSENSORPIN);
    uint16_t rightSensor       = Read_IR_Sensor(RIGHTSENSORPIN);
    uint16_t middleLeftSensor  = Read_IR_Sensor(MIDDLELEFTSENSORPIN);
    uint16_t middleSensor      = Read_IR_Sensor(MIDDLESENSORPIN);
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

// test bluetooth command and output it on the app
void getbluetoothCommand()
{
    char command = UART_Receive();
    UART_SendString("Command: ");
    UART_Transmit(command);
    UART_SendString("\r\n");
}