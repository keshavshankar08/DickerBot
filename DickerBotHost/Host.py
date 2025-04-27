from PyQt5 import QtWidgets, uic
import PyQt5
import sys
import serial
import serial.tools.list_ports
import websockets
import socket
import asyncio
import threading
import os

os.environ["QT_PLUGIN_PATH"] = os.path.join(os.path.dirname(PyQt5.__file__), "Qt", "plugins")

def resource_path(relative_path):
    """Get the absolute path to a resource in a bundled app."""
    try:
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)

class DickerBotHost(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()

        ui_path = resource_path("Host.ui")
        uic.loadUi(ui_path, self)
        
        self.device_manager_refresh_button.clicked.connect(self.refresh_clicked) # refresh button bind
        self.device_manager_sync_button.clicked.connect(self.sync_clicked) # sync button bind
        self.socket_manager_start_button.clicked.connect(self.start_clicked) # start button bind
        self.socket_manager_stop_button.clicked.connect(self.stop_clicked) # stop button bind

        self.socket_manager_status_data.setText("Disconnected") # default status

        # storage for socket information
        self.robot_mac_address = ""
        self.ip_address = ""
        self.port = "8765"


        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        self.ip_address = s.getsockname()[0]
        s.close()

        self.device_manager_ssid_data.clear()
        self.device_manager_password_data.clear()
        self.socket_information_ip_address_data.clear()
        self.socket_information_port_data.clear()
        self.socket_information_ip_address_data.setText(self.ip_address)
        self.socket_information_port_data.setText(self.port)

        # websocket objects
        self.server = None
        self.websocket_server = None
        self.clients = set()

    '''
    Populates the port drop down with available ports
    :return: None
    '''
    def refresh_clicked(self):
        ports = serial.tools.list_ports.comports()
        self.device_manager_port_drop_down.clear()
        for port in ports:
            if "USB" in port.description or "UART" in port.description or "CP210" in port.description or "CH340" in port.description:
                self.device_manager_port_drop_down.addItem(port.device)

    '''
    Executes handshake with robot to share wifi, socket, and robot information.
    :return: None
    '''
    def sync_clicked(self):
        device_port = self.device_manager_port_drop_down.currentText()
        wifi_ssid = self.device_manager_ssid_data.text()
        wifi_password = self.device_manager_password_data.text()

        self.device_manager_ssid_data.clear()
        self.device_manager_password_data.clear()

        ser = None
        try:
            ser = serial.Serial(device_port, 115200, timeout=5)
            config_data = f"WD,{wifi_ssid},{wifi_password},{self.ip_address},{self.port};\n"
            ser.write(config_data.encode())

            response = ser.readline().decode().strip()

            if response.startswith("RD,"):
                device_mac = response.split(",")[1].strip().rstrip(";")
                self.robot_mac_address = device_mac

                self.device_information_mac_address_data.setText(device_mac)
        except serial.SerialException as e:
            pass
        finally:
            if ser is not None:
                ser.close()

    '''
    Starts the websocket server on a thread.
    :return: None
    '''
    def start_clicked(self):
        if self.server:
            return

        self.socket_manager_status_data.setText("Connected")
        self.websocket_server = threading.Thread(target=self.run_websocket_server, daemon=True)
        self.websocket_server.start()

    '''
    Stops the websocket server.
    :return: None
    '''
    def stop_clicked(self):
        self.socket_manager_status_data.setText("Disconnected")

        if self.server:
            asyncio.run_coroutine_threadsafe(self.stop_websocket_server(), self.loop)

    '''
    Runs the websocket server and continuously listens for data transfer asynchronously.
    :return: None
    '''
    def run_websocket_server(self):
        async def handler(websocket):
            try:
                client_ip = websocket.remote_address[0]                
                self.clients.add(websocket)
                
                async for message in websocket:
                    if not message.strip():
                        continue
                    
                    for client in list(self.clients):
                        try:
                            if client != websocket:
                                await client.send(message)
                        except Exception as e:
                            self.clients.remove(client)
                    for client in self.clients:
                        if client != websocket:
                            await client.send(message)
            except Exception as e:
                pass
            finally:
                if websocket in self.clients:
                    self.clients.remove(websocket)

        async def start_server():
            self.server = await websockets.serve(handler, self.ip_address, int(self.port))
            self.loop = asyncio.get_event_loop()
            await self.server.wait_closed()

        asyncio.run(start_server())

    '''
    Stops the websocket server asynchronously.
    :return: None
    '''
    async def stop_websocket_server(self):
        if self.server:
            self.server.close()
            await self.server.wait_closed()
            self.server = None


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = DickerBotHost()
    window.show()
    sys.exit(app.exec_())
