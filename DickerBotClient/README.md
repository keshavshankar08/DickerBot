# DickerBotClient

DickerBotClient is a Python client library designed for seamless communication with DickerBot robots. It provides a simple interface to interact with DickerBot robots, enabling control over sensors and motors via WebSocket communication.

## Features

- **WebSocket Communication**: Connects to a WebSocket server to send and receive sensor and control data.
- **Sensor Data**: Retrieves and processes sensor information such as accelerometer, gyroscope, and distance sensors.
- **Image Data**: Receives image data from the robot and converts it into a usable format.
- **Control Data**: Sends motor control data (speed and direction) to the robot.

## Installation

You can install the latest release of DickerBotClient from PyPI using `pip`:

```bash
pip install dickerbotclient
```

## Usage in Python

### Importing the library in your python script
```python
import dickerbotclient as dbc
```

### Creating an instance of the class
```python
bot = dbc.DickerBotClient()
```

### Connecting to the host socket
Note: Get `ip_address` and `port` from the Host UI
```python
bot.connect(ip_address, port)
```

### Polling sensor data
```python
sensor_data = bot.get_sensor_data()
```
Format: `[acceleration_x, acceleration_y, acceleration_z, angular_velocity_x, angular_velocity_y, angular_velocity_z, temperature, distance_left, distance_front, distance_right, distance_back]`

Value `999` anywhere indicates error

### Polling image data
```python
image = bot.get_image_data()
```

Format: `96x96 grayscale`

### Sending control data
```python
bot.set_control_data(wheel, speed, direction)
```
| **Parameter** | **Description** |
|---------------|-----------------|
| wheel | `0` = left; `1` = right; `999` = error |
| speed | `0`-`255`; `999` = error |
| direction | `0` = neutral; `1` = forward; `2` = backward; `999` = error |

### Disconnecting from host socket
```python
bot.disconnect()
```

## Example Teleop
```python
# TODO: Create a program to do keyboard basic teleop using w,a,s,d while showing realtime video stream and IMU/proximity data.
```

## DickerBot Project

You can find information about the DickerBot on the [GitHub page](https://github.com/keshavshankar08/DickerBot/tree/main).