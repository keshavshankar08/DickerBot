/*
    DickerBotCommunicator.cpp - Library for controlling the DickerBot's communicator.
    Created by Keshav Shankar, February 6, 2025.
    Released into the public domain
*/

#include "DickerBotCommunicator.h"

DickerBotCommunicator::DickerBotCommunicator() {
    cameraBuffer =  nullptr;
}

void DickerBotCommunicator::Begin() {
    InitializeCommunicationToController();
    InitializeCommunicator();
    InitializeCamera();
    SequenceLEDIndicator(0);
}

void DickerBotCommunicator::InitializeCommunicationToController() {
    communicatorSerial.begin(115200, SERIAL_8N1, COMMUNICATOR_RX, COMMUNICATOR_TX);
}

void DickerBotCommunicator::InitializeCommunicator() {
    pinMode(COMMUNICATOR_STATUS_LED, OUTPUT);
    digitalWrite(COMMUNICATOR_STATUS_LED, LOW);
    pinMode(COMMUNICATOR_BUTTON, INPUT);
    WiFi.mode(WIFI_STA);
}

void DickerBotCommunicator::InitializeCamera() {
    camera_config_t config;
  
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 10000000;               
    config.pixel_format = PIXFORMAT;
    config.frame_size = FRAME_SIZE_IMAGE;
    config.jpeg_quality = 10;             
    config.fb_count = 1;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        // ERROR: Camera init failed.
        return;
    }

    sensor_t *s = esp_camera_sensor_get();
    if (!s) {
        // Error: Could not retrieve camera sensor.
        return;
    }

    if (cameraImageExposure == 0 && cameraImageGain == 0) {              
        s->set_gain_ctrl(s, 1);                       // auto gain on 
        s->set_exposure_ctrl(s, 1);                   // auto exposure on 
        s->set_awb_gain(s, 1);                        // Auto White Balance enable (0 or 1)
    } 
    else {
        s->set_gain_ctrl(s, 0);                       // auto gain off 
        s->set_awb_gain(s, 1);                        // Auto White Balance enable (0 or 1)
        s->set_exposure_ctrl(s, 0);                   // auto exposure off 
        s->set_agc_gain(s, cameraImageGain);          // set gain manually (0 - 30)
        s->set_aec_value(s, cameraImageExposure);     // set exposure manually  (0-1200)
    }
}

void DickerBotCommunicator::ReceiveDataFromController() {
    if (communicatorSerial.available()) {
        String data = communicatorSerial.readStringUntil(';');
        if (data.length() == 0) return;

        if (data.startsWith("SD,")) {
            HandleSensorDataFromController(data);
        } 
        else if (data.startsWith("WD,")) {
            HandleConnectionDataFromController(data);
        }
    }
}

void DickerBotCommunicator::HandleSensorDataFromController(String data) {
    data = data.substring(3);
    int numValues = sscanf(data.c_str(), "%f,%f,%f,%f,%f,%f,%f,%d,%d,%d,%d", &sensorBuffer.ax, &sensorBuffer.ay, &sensorBuffer.az, &sensorBuffer.gx, &sensorBuffer.gy, &sensorBuffer.gz, &sensorBuffer.t, &sensorBuffer.dL, &sensorBuffer.dF, &sensorBuffer.dR, &sensorBuffer.dB);
}

void DickerBotCommunicator::HandleConnectionDataFromController(String data) {
    data = data.substring(3);
    char ssid[32], password[64], ip[16];
    int port;
    int numValues = sscanf(data.c_str(), "%31[^,],%63[^,],%15[^,],%d", ssid, password, ip, &port);
    if (numValues == 4) {
        SaveWifiCredentials(ssid, password, ip, port);
    }

    String macAddress = WiFi.macAddress();
    communicatorSerial.print("RD," + macAddress + ";");
    SequenceLEDIndicator(1);
}

void DickerBotCommunicator::SendSensorDataToSocket() {
    String data = "SD," + String(sensorBuffer.ax) + "," + String(sensorBuffer.ay) + "," + String(sensorBuffer.az) + "," +
                  String(sensorBuffer.gx) + "," + String(sensorBuffer.gy) + "," + String(sensorBuffer.gz) + "," +
                  String(sensorBuffer.t) + "," + String(sensorBuffer.dL) + "," + String(sensorBuffer.dF) + "," +
                  String(sensorBuffer.dR) + "," + String(sensorBuffer.dB) + ";";

    webSocket.sendTXT(data);
}

void DickerBotCommunicator::SendCameraDataToSocket() {
    camera_fb_t *cameraBuffer = esp_camera_fb_get();
    if (!cameraBuffer) {
        return;
    }

    String base64Image = base64::encode(cameraBuffer->buf, cameraBuffer->len);
    esp_camera_fb_return(cameraBuffer); 

    String data = "ID," + base64Image + ";";
    
    webSocket.sendTXT(data);
}

void DickerBotCommunicator::SendControlDataToController() {
    communicatorSerial.printf("CD,%d,%d,%d;", controlBuffer.wheel, controlBuffer.speed, controlBuffer.direction);
}

bool DickerBotCommunicator::GetConnectionStatus() { 
    return connected_to_socket;
}

void DickerBotCommunicator::CheckCommunicatorButton() {
    static unsigned long buttonPressStart = 0;
    static bool buttonPressed = false;

    if (digitalRead(COMMUNICATOR_BUTTON) == HIGH) {
        if (!buttonPressed) {
            buttonPressed = true;
            buttonPressStart = millis();
        }
    } 
    else {
        if (buttonPressed) {
            buttonPressed = false;
            unsigned long pressDuration = millis() - buttonPressStart;

            if (pressDuration < 1000) {
                ConnectToSocket();
            } 
            else if (pressDuration > 3000) {
                ClearWifiCredentials();
            }
        }
    }
}

void DickerBotCommunicator::SaveWifiCredentials(const char* ssid, const char* password, const char* ws_ip, int ws_port) {
    preferences.begin("wifi_data", false);
    preferences.putString("ssid", ssid);
    preferences.putString("pass", password);
    preferences.putString("ip", ws_ip);
    preferences.putInt("port", ws_port);
    preferences.end();
}

bool DickerBotCommunicator::LoadWifiCredentials(String &ssid, String &password, String &ws_ip, int &ws_port) {
    preferences.begin("wifi_data", true);
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("pass", "");
    ws_ip = preferences.getString("ip", "");
    ws_port = preferences.getInt("port", -1);
    preferences.end();

    return (ssid.length() > 0 && ws_port != -1);
}

void DickerBotCommunicator::ClearWifiCredentials() {
    preferences.begin("wifi_data", false);
    preferences.clear();
    preferences.end();

    String ssid, password, ws_ip;
    int ws_port;
    
    if (!LoadWifiCredentials(ssid, password, ws_ip, ws_port)) {
        return;
    }
}

void DickerBotCommunicator::ConnectToSocket() {
    String ssid, password, ws_ip;
    int ws_port;

    if (!LoadWifiCredentials(ssid, password, ws_ip, ws_port)) {
        return;
    }

    WiFi.begin(ssid.c_str(), password.c_str());
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        SequenceLEDIndicator(2);
        attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    webSocket.begin(ws_ip.c_str(), ws_port, "/");
    webSocket.onEvent([this](WStype_t type, uint8_t *payload, size_t length) {
        this->OnWebSocketEvent(type, payload, length);
    });
    
    webSocket.setReconnectInterval(3000);
    SequenceLEDIndicator(3);
}

void DickerBotCommunicator::OnWebSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            connected_to_socket = true;

            SequenceLEDIndicator(3);
            
            break;
        
        case WStype_DISCONNECTED:
            connected_to_socket = false;

            controlBuffer.wheel = 0;
            controlBuffer.speed = 0;
            controlBuffer.direction = 0;
            SendControlDataToController();

            controlBuffer.wheel = 1;
            controlBuffer.speed = 0;
            controlBuffer.direction = 0;
            SendControlDataToController();

            SequenceLEDIndicator(2);

            break;

        case WStype_TEXT:
            
        if (payload[0] == 'C' && payload[1] == 'D' && payload[2] == ',') {
            int motor, speed, direction;
            if (sscanf((char*)payload + 3, "%d,%d,%d", &motor, &speed, &direction) == 3) {
                controlBuffer.wheel = motor;
                controlBuffer.speed = speed;
                controlBuffer.direction = direction;
                SendControlDataToController();
            } 
        }
        break;

        default:
            break;
    }
}

void DickerBotCommunicator::HandleWebSocket() {
    webSocket.loop();
}

void DickerBotCommunicator::SequenceLEDIndicator(int event) {
    switch (event) {
        case 0: // Startup init complete (1.5-second flash)
            digitalWrite(COMMUNICATOR_STATUS_LED, HIGH);
            delay(1500);
            digitalWrite(COMMUNICATOR_STATUS_LED, LOW);
            break;
        case 1: // Sync success (600ms flash)
            digitalWrite(COMMUNICATOR_STATUS_LED, HIGH);
            delay(600);
            digitalWrite(COMMUNICATOR_STATUS_LED, LOW);
            break;
        case 2: // Connecting to socket (300ms flash)
            digitalWrite(COMMUNICATOR_STATUS_LED, HIGH);
            delay(300);
            digitalWrite(COMMUNICATOR_STATUS_LED, LOW);
            break;
        case 3: // Socket connection success (1-second flash)
            digitalWrite(COMMUNICATOR_STATUS_LED, HIGH);
            delay(1000);
            digitalWrite(COMMUNICATOR_STATUS_LED, LOW);
            break;
        default:
            break;
    }
}