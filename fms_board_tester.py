import sys
import serial
import time
import json
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLineEdit, QPushButton, 
    QTextEdit, QLabel, QComboBox, QFrame, QListWidget
)
from PyQt5.QtCore import Qt

class TasmotaWiFiTester(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()
        self.serial = None

    def initUI(self):
        layout = QVBoxLayout()

        # Serial port selection
        port_layout = QHBoxLayout()
        port_layout.addWidget(QLabel('Serial Port:'))
        self.port_combo = QComboBox()
        self.port_combo.addItems(['COM1', 'COM2', 'COM3', 'COM4', 'COM5'])  # Adjust as needed
        port_layout.addWidget(self.port_combo)
        self.connect_button = QPushButton('Connect')
        self.connect_button.clicked.connect(self.toggle_connection)
        port_layout.addWidget(self.connect_button)
        layout.addLayout(port_layout)

        # SSID input
        ssid_layout = QHBoxLayout()
        ssid_layout.addWidget(QLabel('SSID:'))
        self.ssid_input = QLineEdit()
        ssid_layout.addWidget(self.ssid_input)
        layout.addLayout(ssid_layout)

        # Password input
        password_layout = QHBoxLayout()
        password_layout.addWidget(QLabel('Password:'))
        self.password_input = QLineEdit()
        self.password_input.setEchoMode(QLineEdit.Password)
        password_layout.addWidget(self.password_input)
        layout.addLayout(password_layout)

        # Send WiFi Command Button
        self.send_button = QPushButton('Send WiFi Command')
        self.send_button.clicked.connect(self.send_wifi_command)
        layout.addWidget(self.send_button)

        # Restart Button
        self.restart_button = QPushButton('Send Restart Command')
        self.restart_button.clicked.connect(self.send_restart_command)
        layout.addWidget(self.restart_button)

        # WiFi Scan Button
        self.wifi_scan_button = QPushButton('Scan for WiFi Networks')
        self.wifi_scan_button.clicked.connect(self.send_wifiscan_command)
        layout.addWidget(self.wifi_scan_button)

        # WiFi Scan Results Box
        self.wifi_results = QListWidget()
        layout.addWidget(self.wifi_results)

        # Test result boxes
        result_layout = QHBoxLayout()
        self.pass_box = self.create_result_box("Test Pass", "#90EE90")  # Light Green
        self.fail_box = self.create_result_box("Test Fail", "#FFB6C1")  # Light Red
        result_layout.addWidget(self.pass_box)
        result_layout.addWidget(self.fail_box)
        layout.addLayout(result_layout)

        # Log display
        self.log_display = QTextEdit()
        self.log_display.setReadOnly(True)
        layout.addWidget(self.log_display)

        self.setLayout(layout)
        self.setWindowTitle('FMS Device Tester')
        self.reset_test_boxes()
        self.show()

    def create_result_box(self, label_text, color):
        frame = QFrame()
        frame.setFrameShape(QFrame.Box)
        frame.setLineWidth(2)
        frame.setMinimumSize(100, 50)
        label = QLabel(label_text)
        label.setAlignment(Qt.AlignCenter)
        layout = QVBoxLayout()
        layout.addWidget(label)
        frame.setLayout(layout)
        return frame

    def toggle_connection(self):
        if self.serial is None:
            try:
                self.serial = serial.Serial(self.port_combo.currentText(), 115200, timeout=1)
                self.connect_button.setText('Disconnect')
                self.log_display.append(f"Connected to {self.port_combo.currentText()}")
            except serial.SerialException as e:
                self.log_display.append(f"Error: {str(e)}")
        else:
            self.serial.close()
            self.serial = None
            self.connect_button.setText('Connect')
            self.log_display.append("Disconnected")

    def send_wifi_command(self):
        if self.serial is None:
            self.log_display.append("Error: Not connected to any port")
            return

        ssid = self.ssid_input.text()
        password = self.password_input.text()
        command = f'wifi ssid:"{ssid}" password:"{password}"'
        response = self.send_command(command)
        
        try:
            response_json = json.loads(response)
            if response_json.get("wifi") == "true":
                self.set_test_result(True)
                self.log_display.append("WiFi configuration successful!")
            else:
                self.set_test_result(False)
                self.log_display.append("WiFi configuration failed.")
        except json.JSONDecodeError:
            self.set_test_result(False)
            self.log_display.append("Invalid response format.")

    def send_restart_command(self):
        self.send_command("restart")

    def send_wifiscan_command(self):
        response = self.send_command("wifiscan")
        self.process_wifi_scan_response(response)

    def send_command(self, command):
        if self.serial is None:
            self.log_display.append("Error: Not connected to any port")
            return ""

        self.log_display.append(f"Sending: {command}")
        self.serial.write((command + '\n').encode())
        
        time.sleep(0.1)  # Wait for response
        response = self.serial.read(self.serial.in_waiting).decode().strip()
        self.log_display.append(f"Received: {response}")
        return response

    def process_wifi_scan_response(self, response):
        """ Parses the WiFi scan response and updates the list widget. """
        self.wifi_results.clear()

        try:
            response_json = json.loads(response)
            if "networks" in response_json:
                for network in response_json["networks"]:
                    ssid = network.get("SSID", "Unknown")
                    rssi = network.get("RSSI", "N/A")
                    ch = network.get("CH", "N/A")
                    encryption = network.get("Encryption", "N/A")
                    self.wifi_results.addItem(f"{ssid} | RSSI: {rssi} | CH: {ch} | {encryption}")
            else:
                self.wifi_results.addItem("No networks found.")
        except json.JSONDecodeError:
            self.wifi_results.addItem("Error parsing WiFi scan response.")

    def set_test_result(self, success):
        """ Updates the result box color based on success or failure. """
        if success:
            self.pass_box.setStyleSheet("background-color: #90EE90;")  # Light Green
            self.fail_box.setStyleSheet("background-color: white;")
        else:
            self.pass_box.setStyleSheet("background-color: white;")
            self.fail_box.setStyleSheet("background-color: #FFB6C1;")  # Light Red

    def reset_test_boxes(self):
        self.pass_box.setStyleSheet("background-color: white;")
        self.fail_box.setStyleSheet("background-color: white;")

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = TasmotaWiFiTester()
    sys.exit(app.exec_())
