/*
    DickerBotCommunicator.h - Library for controlling the DickerBot's communicator.
    Created by Keshav Shankar, February 6, 2025.
    Released into the public domain
*/
#ifndef DickerBotCommunicator_h
#define DickerBotCommunicator_h

#include <Arduino.h>
#include <HardwareSerial.h>
#include "esp_camera.h"
#include <vector>
#include <WiFi.h>
#include "base64.h"
#include <Preferences.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>

struct SensorData {
    float ax = 999, ay = 999, az = 999;  // Accelerometer
    float gx = 999, gy = 999, gz = 999;  // Gyroscope
    float t = 999;  // Temperature
    int dL = 999, dF = 999, dR = 999, dB = 999;  // Distance sensors
};

struct ControlData {
    int wheel = 999;  // 0 = left, 1 = right
    int speed = 999;  // 0-255
    int direction = 999;  // 0 = neutral, 1 = forward, 2 = backward
};


class DickerBotCommunicator {
private:
    // ----- Controller -----
    static const int COMMUNICATOR_TX = 14;
    static const int COMMUNICATOR_RX = 13;
    HardwareSerial communicatorSerial = HardwareSerial(1);

    // ----- Communicator -----
    static const int COMMUNICATOR_STATUS_LED = 12;
    static const int COMMUNICATOR_BUTTON = 2;
    Preferences preferences;

    // ----- Socket -----
    WebSocketsClient webSocket;
    bool connected_to_socket = false;

    // ----- Camera -----
    framesize_t FRAME_SIZE_IMAGE = FRAMESIZE_96X96;
    pixformat_t PIXFORMAT = PIXFORMAT_GRAYSCALE;
    int cameraImageExposure = 0;
    int cameraImageGain = 0;
    int brightLED = 4;
    int PWDN_GPIO_NUM = 32;
    int RESET_GPIO_NUM = -1;
    int XCLK_GPIO_NUM = 0;
    int SIOD_GPIO_NUM = 26;
    int SIOC_GPIO_NUM = 27;
    int Y9_GPIO_NUM = 35;
    int Y8_GPIO_NUM = 34;
    int Y7_GPIO_NUM = 39;
    int Y6_GPIO_NUM = 36;
    int Y5_GPIO_NUM = 21;
    int Y4_GPIO_NUM = 19;
    int Y3_GPIO_NUM = 18;
    int Y2_GPIO_NUM = 5;
    int VSYNC_GPIO_NUM = 25;
    int HREF_GPIO_NUM = 23;
    int PCLK_GPIO_NUM = 22;

    // ----- Buffers -----
    SensorData sensorBuffer;
    ControlData controlBuffer;
    camera_fb_t *cameraBuffer;

public:
    DickerBotCommunicator();

    /**
     * @brief Initializes the communicator module.
     * @return void
     * @warning This function should be called once in setup() following start of Serial.
     */
    void Begin();
    
    /**
     * @brief Starts the hardware serial to the controller module.
     * @return void
     */
    void InitializeCommunicationToController();

    /**
     * @brief Starts the LED and button.
     * @return void
     */
    void InitializeCommunicator();

    /**
     * @brief Starts the camera.
     * @return void
     */
    void InitializeCamera();

    /**
     * @brief Receives data from the controller module.
     * @return void
     */
    void ReceiveDataFromController();

    /**
     * @brief Handles sensor data from the data receiver.
     * @param data The data received from the controller module.
     * @return void
     */
    void HandleSensorDataFromController(String data);

    /**
     * @brief Handles connection data from the data receiver.
     * @param data The data received from the controller module.
     * @return void
     */
    void HandleConnectionDataFromController(String data);

    /**
     * @brief Sends sensor data to the socket as string.
     * @return void
     */
    void SendSensorDataToSocket();

    /**
     * @brief Sends camera data to the socket as byte string.
     * @return void
     */
    void SendCameraDataToSocket();

    /**
     * @brief Sends control data to the control module as string.
     * @return void
     */
    void SendControlDataToController();

    /**
     * @brief Gets the connection status to the socket.
     * @return true if connected, false otherwise.
     */
    bool GetConnectionStatus();

    /**
     * @brief Checks and handles multi purpose button press.
     * @return void
     */
    void CheckCommunicatorButton();

    /**
     * @brief Saves the wifi credentials to memory.
     * @param ssid The SSID of the wifi network.
     * @param password The password of the wifi network.
     * @param ws_ip The IP address of the websocket server.
     * @param ws_port The port of the websocket server.
     * @return void
     */
    void SaveWifiCredentials(const char* ssid, const char* password, const char* ws_ip, int ws_port);

    /**
     * @brief Loads the wifi credentials from memory.
     * @param ssid The SSID of the wifi network.
     * @param password The password of the wifi network.
     * @param ws_ip The IP address of the websocket server.
     * @param ws_port The port of the websocket server.
     * @return true if credentials are loaded, false otherwise.
     */
    bool LoadWifiCredentials(String &ssid, String &password, String &ws_ip, int &ws_port);

    /**
     * @brief Clears the wifi credentials from memory.
     * @return void
     */
    void ClearWifiCredentials();

    /**
     * @brief Attempts to connect to the socket.
     * @return void
     */
    void ConnectToSocket();

    /**
     * @brief Handles events from the websocket.
     * @param type The type of event.
     * @param payload The payload of the event.
     * @param length The length of the payload.
     * @return void
     */
    void OnWebSocketEvent(WStype_t type, uint8_t *payload, size_t length);

    /**
     * @brief Loops the websocket for updates.
     * @return void
     */
    void HandleWebSocket();

    /**
     * @brief Sequences the LED indicator for a given event.
     * @param event The event number to sequence the LED for.
     * @return void
     */
    void SequenceLEDIndicator(int event);
};

#endif