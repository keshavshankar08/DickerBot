/*
 * File: Communicator.ino
 * Author: Keshav Shankar
 * Description: This file is part of the DickerBot project.
 *              It is meant to be uploaded to the esp32-cam board.
 */

#include "DickerBotCommunicator.h"

// Vars for controlling frequency of updates (currently 1000/33 = 30 Hz)
unsigned long lastUpdateTime = 0; 
const int updateInterval = 33;

// Create instance of class
DickerBotCommunicator dickerBotCommunicator;

void setup() {
    // Start serial communication
    Serial.begin(115200);

    // Initialize the communicator
    dickerBotCommunicator.Begin();
}

void loop() {
    // Get current time
    unsigned long currentTime = millis();
    
    // Update on set interval
    if (currentTime - lastUpdateTime >= updateInterval) {
        lastUpdateTime = currentTime;

        // Send data to socket if connected
        if (dickerBotCommunicator.GetConnectionStatus()) {
            dickerBotCommunicator.SendSensorDataToSocket();
            dickerBotCommunicator.SendCameraDataToSocket();
        }
        else {
            dickerBotCommunicator.CheckCommunicatorButton();
        }
    }

    // Get updates from controller
    dickerBotCommunicator.ReceiveDataFromController();

    // Get updates from socket
    dickerBotCommunicator.HandleWebSocket(); 
}