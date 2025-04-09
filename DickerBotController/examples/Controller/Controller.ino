/*
 * File: Controller.ino
 * Author: Keshav Shankar
 * Description: This file is part of the DickerBot project.
 *              It is meant to be uploaded to the esp32-wroom32 board.
 */

#include "DickerBotController.h"

// Vars for controlling frequency of updates (currently 1000/33 = 30 Hz)
const int updateInterval = 33;

// Create an instance of the class
DickerBotController dickerBotController;

void setup() {
  // Initialize the serial
  Serial.begin(115200);

  // Start the controller
  dickerBotController.Begin();
}

void loop() {
  // Receive and send data to the socket and communicator on set frequency
  dickerBotController.ReceiveDataFromComputer();
  dickerBotController.ReceiveDataFromCommunicator();
  dickerBotController.SendSensorDataToCommunicator();
  delay(updateInterval);
}