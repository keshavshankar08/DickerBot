# DickerBotCommunicator

DickerBotCommunicator is the embedded library for the communicator module of the DickerBot. It provides communication between the socket and the controller module of the DickerBot.

## Installation

You can install the latest release of DickerBotCommunicator from [here](https://github.com/keshavshankar08/DickerBot/releases). 

See the video below for a step-by-step tutorial on updating the software for the DickerBotCommunicator module.

[![DickerBotCommunicator Software Update Tutorial](https://img.youtube.com/vi/m8kM3JT4JY0/0.jpg)](https://www.youtube.com/watch?v=m8kM3JT4JY0)

## Usage

An example of how the library should be used can be found in [examples/Communicator/Communicator.ino](examples/Communicator/Communicator.ino). You should not edit this code unless you read the documentation thoroughly, which is located at [src/DickerBotCommunicator.h](src/DickerBotCommunicator.h).

## Documentation

### Serial Data Format
| Prefix | Meaning       | Structure                                |
|--------|---------------|------------------------------------------|
| WD     | Wifi Data     | WD,ssid,password,ip,port;               |
| RD     | Robot Data    | RD,mac_address;                         |
| CD     | Control Data  | CD,left_wheel_speed,left_wheel_direction,right_wheel_speed,right_wheel_direction;               |
| SD     | Sensor Data   | SD,ax,ay,az,gx,gy,gz,t,dL,dF,dR,dB;     |
| ID     | Image Data    | ID,byte64;                          |

### Data Defintions

#### IMU
| Field  | Type   | Default Value | Description          |
|--------|--------|---------------|----------------------|
| **ax** | float  | 999           | X-axis acceleration  |
| **ay** | float  | 999           | Y-axis acceleration  |
| **az** | float  | 999           | Z-axis acceleration  |
| **gx** | float  | 999           | X-axis gyroscope value  |
| **gy** | float  | 999           | Y-axis gyroscope value  |
| **gz** | float  | 999           | Z-axis gyroscope value  |
| **temp**    | float  | 999           | Temperature in Celsius     |

#### Ultrasonic Sensor
| Field  | Type   | Default Value | Description          |
|--------|--------|---------------|----------------------|
| **dL**      | int    | 999           | Distance sensor (Left)     |
| **dF**      | int    | 999           | Distance sensor (Front)    |
| **dR**      | int    | 999           | Distance sensor (Right)    |
| **dB**      | int    | 999           | Distance sensor (Back)     |

#### Camera
| Field       | Type   | Default Value | Description                          |
|-------------|--------|---------------|--------------------------------------|
| **data**    | String | N/A           | Base64-encoded image data            |

#### Wheels

| Field  | Type   | Default Value | Description          |
|-------------|--------|---------------|----------------------------|
| **left_wheel_speed**   | int    | 999           | Speed (0-255)              |
| **left_wheel_direction** | int  | 999           | 0 = neutral, 1 = forward, 2 = backward |
| **right_wheel_speed**   | int    | 999           | Speed (0-255)              |
| **right_wheel_direction** | int  | 999           | 0 = neutral, 1 = forward, 2 = backward |

## DickerBot Project

You can find information about the DickerBot on the [GitHub page](https://github.com/keshavshankar08/DickerBot/tree/main).