import sys
import json
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QLineEdit, QPushButton, QVBoxLayout, QGridLayout, QMessageBox
)
import paho.mqtt.client as mqtt

MQTT_BROKER = "192.168.1.124"  # Or your local broker IP
MQTT_PORT = 1883
MQTT_TOPIC = "detpos/local_server/initial1/det/0A0000"   # Change this to match ESP32 topic

class MqttJsonSender(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("ESP32 JSON MQTT Sender")

        # Input fields
        self.inputs = {}
        labels = ["devicenum", "nozzlenum"] + [f"pumpid{i}" for i in range(1, 9)]

        layout = QGridLayout()
        for i, key in enumerate(labels):
            layout.addWidget(QLabel(key), i, 0)
            line_edit = QLineEdit()
            line_edit.setPlaceholderText("Enter number")
            self.inputs[key] = line_edit
            layout.addWidget(line_edit, i, 1)

        self.send_button = QPushButton("Send JSON via MQTT")
        self.send_button.clicked.connect(self.send_json)

        layout.addWidget(self.send_button, len(labels), 0, 1, 2)

        self.setLayout(layout)

    def send_json(self):
        try:
            data = {key: int(self.inputs[key].text()) for key in self.inputs}
        except ValueError:
            QMessageBox.critical(self, "Input Error", "Please enter only integers.")
            return

        payload = json.dumps(data)
        print("Sending JSON:\n", payload)

        client = mqtt.Client()
        try:
            client.connect(MQTT_BROKER, MQTT_PORT, 60)
            client.publish(MQTT_TOPIC, payload)
            client.disconnect()
            QMessageBox.information(self, "Success", "JSON sent via MQTT successfully.")
        except Exception as e:
            QMessageBox.critical(self, "MQTT Error", str(e))

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MqttJsonSender()
    window.show()
    sys.exit(app.exec_())
