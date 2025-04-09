/*
    DickerBotController.h - Library for controlling the DickerBot's controller.
    Created by Keshav Shankar, February 6, 2025.
    Released into the public domain
*/
#ifndef DickerBotController_h
#define DickerBotController_h

#include <Arduino.h>
#include <NewPing.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <HardwareSerial.h>

class DickerBotController {
private:
    // ----- Wheels -----
    static const int LEFT_WHEEL_EN = 23;
    static const int LEFT_WHEEL_IN1 = 25;
    static const int LEFT_WHEEL_IN2 = 26;
    static const int RIGHT_WHEEL_EN = 19;
    static const int RIGHT_WHEEL_IN1 = 27;
    static const int RIGHT_WHEEL_IN2 = 32;

    // ----- Distance Sensors -----
    static const int LEFT_DISTANCE_SENSOR_TRIGGER = 33;
    static const int LEFT_DISTANCE_SENSOR_ECHO = 34;
    static const int FRONT_DISTANCE_SENSOR_TRIGGER = 16;
    static const int FRONT_DISTANCE_SENSOR_ECHO = 35;
    static const int RIGHT_DISTANCE_SENSOR_TRIGGER = 17;
    static const int RIGHT_DISTANCE_SENSOR_ECHO = 36;
    static const int BACK_DISTANCE_SENSOR_TRIGGER = 18;
    static const int BACK_DISTANCE_SENSOR_ECHO = 39;
    NewPing leftDistanceSensor = NewPing(LEFT_DISTANCE_SENSOR_TRIGGER, LEFT_DISTANCE_SENSOR_ECHO);
    NewPing frontDistanceSensor = NewPing(FRONT_DISTANCE_SENSOR_TRIGGER, FRONT_DISTANCE_SENSOR_ECHO);
    NewPing rightDistanceSensor = NewPing(RIGHT_DISTANCE_SENSOR_TRIGGER, RIGHT_DISTANCE_SENSOR_ECHO);
    NewPing backDistanceSensor = NewPing(BACK_DISTANCE_SENSOR_TRIGGER, BACK_DISTANCE_SENSOR_ECHO);

    // ----- IMU Sensor -----
    static const int IMU_SENSOR_SDA = 21;
    static const int IMU_SENSOR_SCL = 22;
    Adafruit_MPU6050 imuSensor;

    // ----- Communicator -----
    static const int CONTROLLER_TX = 13;
    static const int CONTROLLER_RX = 4;
    HardwareSerial controllerSerial = HardwareSerial(2);

    // ----- Controller -----
    static const int CONTROLLER_STATUS_LED = 5;
    static const int CONTROLLER_BUTTON = 15;

public:
    DickerBotController();

    /**
     * @brief Initializes the controller module.
     * @return void
     * @warning This function should be called once in setup() following start of Serial.
     */
    void Begin();

    /**
     * @brief Starts the wheels.
     * @return void
     */
    void InitializeWheels();

    /**
     * @brief Starts the distance sensors.
     * @return void
     */
    void InitializeDistanceSensors();

    /**
     * @brief Starts the IMU sensor.
     * @return void
     */
    void InitializeIMUSensor();

    /**
     * @brief Starts the hardware serial to the communicator module.
     * @return void
     */
    void InitializeCommunicationToCommunicator();

    /**
     * @brief Starts the LED and button.
     * @return void
     */
    void InitializeController();

    /**
     * @brief Checks and handles multi purpose button press.
     * @return void
     */
    void CheckControllerButton();

    /**
     * @brief Sets left wheel to a speed.
     * @param speed The speed to set the left wheel to.
     * @return void
     */
    void SetLeftWheelSpeed(int speed);

    /**
     * @brief Sets right wheel to a speed.
     * @param speed The speed to set the right wheel to.
     * @return void
     */
    void SetRightWheelSpeed(int speed);

    /**
     * @brief Sets left wheel to forward.
     * @return void
     */
    void SetLeftWheelForward();

    /**
     * @brief Sets left wheel to backward.
     * @return void
     */
    void SetLeftWheelBackward();

    /**
     * @brief Sets left wheel to neutral.
     * @return void
     */
    void SetLeftWheelNeutral();

    /**
     * @brief Sets right wheel to forward.
     * @return void
     */
    void SetRightWheelForward();

    /**
     * @brief Sets right wheel to backward.
     * @return void
     */
    void SetRightWheelBackward();
    
    /**
     * @brief Sets right wheel to neutral.
     * @return void
     */
    void SetRightWheelNeutral();

    /**
     * @brief Gets the distance data from the distance sensors.
     * @param data The array to store the distance data in.
     * @return void
     */
    void GetDistanceData(int* data);

    /**
     * @brief Gets the IMU data from the IMU sensor.
     * @param data The array to store the IMU data in.
     * @return void
     */
    void GetIMUData(float* data);

    /**
     * @brief Sends sensor data to the communicator module.
     * @return void
     */
    void SendSensorDataToCommunicator();

    /**
     * @brief Receives data from the communicator module.
     * @return void
     */
    void ReceiveDataFromCommunicator();

    /**
     * @brief Handles control data from the communicator module.
     * @param data The data received from the communicator module.
     * @return void
     */
    void HandleControlDataFromCommunicator(String data);

    /**
     * @brief Handles connection data from the communicator module.
     * @param data The data received from the communicator module.
     * @return void
     */
    void HandleConnectionDataFromCommunicator(String data);

    /**
     * @brief Receives data from the computer.
     * @return void
     */
    void ReceiveDataFromComputer();

    /**
     * @brief Handles connection data from the computer.
     * @param data The data received from the computer.
     * @return void
     */
    void HandleConnectionDataFromComputer(String data);

    /**
     * @brief Sequences the LED indicator for a given event.
     * @param event The event number to sequence the LED for.
     * @return void
     */
    void SequenceLEDIndicator(int event);
};

#endif