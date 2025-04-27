/*
    DickerBotController.h - Library for controlling the DickerBot's controller.
    Created by Keshav Shankar, February 6, 2025.
    Released into the public domain
*/

#include "DickerBotController.h"

DickerBotController::DickerBotController() {
}

void DickerBotController::Begin() {
    InitializeWheels();
    InitializeDistanceSensors();
    InitializeIMUSensor();
    InitializeCommunicationToCommunicator();
    InitializeController();
    SequenceLEDIndicator(0);
}

void DickerBotController::InitializeWheels() {
    pinMode(LEFT_WHEEL_EN, OUTPUT);
    pinMode(LEFT_WHEEL_IN1, OUTPUT);
    pinMode(LEFT_WHEEL_IN2, OUTPUT);
    pinMode(RIGHT_WHEEL_EN, OUTPUT);
    pinMode(RIGHT_WHEEL_IN1, OUTPUT);
    pinMode(RIGHT_WHEEL_IN2, OUTPUT);
    digitalWrite(LEFT_WHEEL_EN, 0);
    digitalWrite(LEFT_WHEEL_IN1, LOW);
    digitalWrite(LEFT_WHEEL_IN2, LOW);
    digitalWrite(RIGHT_WHEEL_EN, 0); 
    digitalWrite(RIGHT_WHEEL_IN1, LOW);
    digitalWrite(RIGHT_WHEEL_IN2, LOW);
}

void DickerBotController::InitializeDistanceSensors() {
    pinMode(LEFT_DISTANCE_SENSOR_TRIGGER, OUTPUT);
    pinMode(LEFT_DISTANCE_SENSOR_ECHO, INPUT);
    pinMode(FRONT_DISTANCE_SENSOR_TRIGGER, OUTPUT);
    pinMode(FRONT_DISTANCE_SENSOR_ECHO, INPUT);
    pinMode(RIGHT_DISTANCE_SENSOR_TRIGGER, OUTPUT);
    pinMode(RIGHT_DISTANCE_SENSOR_ECHO, INPUT);
    pinMode(BACK_DISTANCE_SENSOR_TRIGGER, OUTPUT);
    pinMode(BACK_DISTANCE_SENSOR_ECHO, INPUT);
    digitalWrite(LEFT_DISTANCE_SENSOR_TRIGGER, LOW);
    digitalWrite(FRONT_DISTANCE_SENSOR_TRIGGER, LOW);
    digitalWrite(RIGHT_DISTANCE_SENSOR_TRIGGER, LOW);
    digitalWrite(BACK_DISTANCE_SENSOR_TRIGGER, LOW);
}


void DickerBotController::InitializeIMUSensor() {
    imuSensor.begin();
    imuSensor.setAccelerometerRange(MPU6050_RANGE_8_G);
    imuSensor.setGyroRange(MPU6050_RANGE_500_DEG);
    imuSensor.setFilterBandwidth(MPU6050_BAND_21_HZ);
}


void DickerBotController::InitializeCommunicationToCommunicator() {
    controllerSerial.begin(115200, SERIAL_8N1, CONTROLLER_RX, CONTROLLER_TX);
}

void DickerBotController::InitializeController() {
    pinMode(CONTROLLER_STATUS_LED, OUTPUT);
    digitalWrite(CONTROLLER_STATUS_LED, LOW);
    pinMode(CONTROLLER_BUTTON, INPUT);
}

void DickerBotController::CheckControllerButton() {
    static unsigned long buttonPressStart = 0;
    static bool buttonPressed = false;

    if (digitalRead(CONTROLLER_BUTTON) == HIGH) {
        if (!buttonPressed) {
            buttonPressed = true;
            buttonPressStart = millis();
        }
    } 
    else {
        if (buttonPressed) {
            buttonPressed = false;
            unsigned long pressDuration = millis() - buttonPressStart;
            
            // Handle button press here: if (pressDuration < 1000) {}
        }
    }
}

void DickerBotController::SetLeftWheelSpeed(int speed) {
    digitalWrite(LEFT_WHEEL_EN, speed);
}

void DickerBotController::SetRightWheelSpeed(int speed) {
    digitalWrite(RIGHT_WHEEL_EN, speed);
}

void DickerBotController::SetLeftWheelForward() {
    digitalWrite(LEFT_WHEEL_IN1, LOW);
    digitalWrite(LEFT_WHEEL_IN2, HIGH);
}

void DickerBotController::SetLeftWheelBackward() {
    digitalWrite(LEFT_WHEEL_IN1, HIGH);
    digitalWrite(LEFT_WHEEL_IN2, LOW);
}

void DickerBotController::SetLeftWheelNeutral() {
    digitalWrite(LEFT_WHEEL_IN1, LOW);
    digitalWrite(LEFT_WHEEL_IN2, LOW);
}

void DickerBotController::SetRightWheelForward() {
    digitalWrite(RIGHT_WHEEL_IN1, HIGH);
    digitalWrite(RIGHT_WHEEL_IN2, LOW);
}

void DickerBotController::SetRightWheelBackward() {
    digitalWrite(RIGHT_WHEEL_IN1, LOW);
    digitalWrite(RIGHT_WHEEL_IN2, HIGH);
}

void DickerBotController::SetRightWheelNeutral() {
    digitalWrite(RIGHT_WHEEL_IN1, LOW);
    digitalWrite(RIGHT_WHEEL_IN2, LOW);
}

void DickerBotController::GetDistanceData(int* data) {
    data[0] = leftDistanceSensor.ping_cm();
    data[1] = frontDistanceSensor.ping_cm();
    data[2] = rightDistanceSensor.ping_cm();
    data[3] = backDistanceSensor.ping_cm();
}

void DickerBotController::GetIMUData(float* data) {
    sensors_event_t a, g, temp;
    imuSensor.getEvent(&a, &g, &temp);
    data[0] = a.acceleration.x;
    data[1] = a.acceleration.y;
    data[2] = a.acceleration.z;
    data[3] = g.gyro.x;
    data[4] = g.gyro.y;
    data[5] = g.gyro.z;
    data[6] = temp.temperature;
}

void DickerBotController::SendSensorDataToCommunicator() {
    int distanceData[4];
    GetDistanceData(distanceData);
    float imuData[7];
    GetIMUData(imuData);
    controllerSerial.print("SD,");
    for (int i = 0; i < 7; i++) {
        controllerSerial.print(imuData[i]);
        controllerSerial.print(",");
    }
    for (int i = 0; i < 4; i++) {
        controllerSerial.print(distanceData[i]);
        controllerSerial.print(i < 3 ? "," : ";");
    }
}

void DickerBotController::ReceiveDataFromCommunicator() {
    if (controllerSerial.available()) {
        String data = controllerSerial.readStringUntil(';');
        if (data.length() == 0) return;
        
        if (data.startsWith("CD,")) {
            HandleControlDataFromCommunicator(data);
        } 
        else if (data.startsWith("RD,")) {
            HandleConnectionDataFromCommunicator(data);
        }
    }
}

void DickerBotController::HandleControlDataFromCommunicator(String data) {
    data = data.substring(3);
    int left_wheel_speed, left_wheel_direction, right_wheel_speed, right_wheel_direction;
    int numValues = sscanf(data.c_str(), "%d,%d,%d,%d", &left_wheel_speed, &left_wheel_direction, &right_wheel_speed, &right_wheel_direction);
    if (numValues == 4) {
        SetLeftWheelSpeed(left_wheel_speed);
        if (left_wheel_direction == 0) {
            SetLeftWheelNeutral();
        } 
        else if (left_wheel_direction == 1) {
            SetLeftWheelForward();
        } 
        else if (left_wheel_direction == 2) {
            SetLeftWheelBackward();
        }
        SetRightWheelSpeed(right_wheel_speed);
        if (right_wheel_direction == 0) {
            SetRightWheelNeutral();
        }
        else if (right_wheel_direction == 1) {
            SetRightWheelForward();
        }
        else if (right_wheel_direction == 2) {
            SetRightWheelBackward();
        }
    }
}

void DickerBotController::HandleConnectionDataFromCommunicator(String data) {
    Serial.print(data + ";");
}

void DickerBotController::ReceiveDataFromComputer() {
    if (Serial.available()) {
        String data = Serial.readStringUntil(';');
        if (data.length() == 0) return;

        if (data.startsWith("WD,")) {
            HandleConnectionDataFromComputer(data);
        } 
    }
}

void DickerBotController::HandleConnectionDataFromComputer(String data) {
    controllerSerial.print(data + ";");
}

void DickerBotController::SequenceLEDIndicator(int event) {
    switch (event) {
        case 0: // Startup init complete (1.5-second flash)
            digitalWrite(CONTROLLER_STATUS_LED, HIGH);
            delay(1500);
            digitalWrite(CONTROLLER_STATUS_LED, LOW);
            break;
        default:
            break;
    }
}