import asyncio
import websockets
import base64
import numpy as np
import threading

class DickerBotClient:
    def __init__(self):
        self.ws = None
        self.uri = None
        self.running = False
        self.lock = threading.Lock()

        self.sensor_data = {}
        self.latest_image = None

    '''
    Connects to the websocket server asynchronously.
    :param uri: The URI of the websocket server.
    '''
    async def _connect(self, uri):
        self.uri = uri
        async with websockets.connect(uri) as ws:
            self.ws = ws
            self.running = True
            await self._listen()

    '''
    Connects to the websocket server.
    :param ip: The IP address of the websocket server.
    :param port: The port of the websocket server.
    :return: None
    '''
    def connect(self, ip, port=8765):
        uri = f"ws://{ip}:{port}"
        threading.Thread(target=asyncio.run, args=(self._connect(uri),), daemon=True).start()

    '''
    Listens for incoming messages from the websocket server.
    :return: None
    '''
    async def _listen(self):
        while self.running:
            try:
                message = await self.ws.recv()
                self._handle_message(message)
            except Exception as e:
                self.running = False
                break

    '''
    Handles incoming messages from the websocket server.
    :param message: The incoming message.
    :return: None
    '''
    def _handle_message(self, message):
        if message.startswith("SD,"):
            self._parse_sensor_data(message)
        elif message.startswith("ID,"):
            self._parse_image_data(message)

    '''
    Parses sensor data from the incoming message.
    :param message: The incoming message.
    :return: None
    '''
    def _parse_sensor_data(self, message):
        try:
            message = message.strip()
            
            data_string = message[3:].strip(';')
            data_values = data_string.split(",")
            data = list(map(float, data_values))

            with self.lock:
                self.sensor_data = {
                    "ax": data[0], "ay": data[1], "az": data[2],
                    "gx": data[3], "gy": data[4], "gz": data[5],
                    "t": data[6], "dL": data[7], "dF": data[8], "dR": data[9], "dB": data[10]
                }
        except ValueError:
            pass

    '''
    Parses image data from the incoming message.
    :param message: The incoming message.
    :return: None
    '''
    def _parse_image_data(self, message):
        try:
            parts = message.split(",", 1)

            image_data = base64.b64decode(parts[1].rstrip(";"))

            with self.lock:
                self.latest_image = np.frombuffer(image_data, dtype=np.uint8).reshape((96, 96))
        except Exception as e:
            pass

    '''
    Returns the latest sensor data.
    :return: The latest sensor data.
    '''
    def get_sensor_data(self):
        with self.lock:
            return self.sensor_data.copy()

    '''
    Returns the latest image data.
    :return: The latest image data.
    '''
    def get_image_data(self):
        with self.lock:
            return self.latest_image.copy() if self.latest_image is not None else None

    '''
    Sends control data to the websocket server.
    :param motor: The motor to control.
    :param speed: The speed to set.
    :param direction: The direction to set.
    :return: None
    '''
    async def _send_control_data(self, motor, speed, direction):
        if self.ws and self.running:
            message = f"CD,{motor},{speed},{direction};"
            await self.ws.send(message)

    '''
    Sets control data for the specified motor.
    :param motor: The motor to control.
    :param speed: The speed to set.
    :param direction: The direction to set.
    :return: None
    '''
    def set_control_data(self, motor, speed, direction):
        if self.ws and self.running:
            asyncio.run(self._send_control_data(motor, speed, direction))

    '''
    Disconnects from the websocket server.
    :return: None
    '''
    def disconnect(self):
        self.running = False
        if self.ws:
            async def close_ws():
                try:
                    await self.ws.close()
                except Exception as e:
                    pass

            loop = asyncio.new_event_loop()
            asyncio.set_event_loop(loop)
            asyncio.run_coroutine_threadsafe(close_ws(), loop)