import sys
import json
import serial
from PyQt5.QtWidgets import (
    QApplication, QWidget, QLabel, QSpinBox, QVBoxLayout,
    QPushButton, QHBoxLayout, QMessageBox
)

class EEPROMEditor(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("ESP32 EEPROM Editor")
        self.ser = serial.Serial("COM4", 115200, timeout=1)  # Replace COM4 as needed
# 
        layout = QVBoxLayout()

        self.deviceSpin = QSpinBox()
        self.deviceSpin.setMaximum(255)
        self.nozzleSpin = QSpinBox()
        self.nozzleSpin.setMaximum(8)

        layout.addWidget(QLabel("Device ID:"))
        layout.addWidget(self.deviceSpin)
        layout.addWidget(QLabel("Nozzle Count:"))
        layout.addWidget(self.nozzleSpin)
 
        self.nozzleSpins = []
        for i in range(8):
            spin = QSpinBox()
            spin.setMaximum(255)
            layout.addWidget(QLabel(f"Nozzle {i+1} ID:"))
            layout.addWidget(spin)
            self.nozzleSpins.append(spin)

        btnLayout = QHBoxLayout()
        self.readBtn = QPushButton("Read EEPROM")
        self.writeBtn = QPushButton("Write EEPROM")
        self.readBtn.clicked.connect(self.read_data)
        self.writeBtn.clicked.connect(self.write_data)
        btnLayout.addWidget(self.readBtn)
        btnLayout.addWidget(self.writeBtn)

        layout.addLayout(btnLayout)
        self.setLayout(layout)

    def send_json(self, obj):
        self.ser.write((json.dumps(obj) + '\n').encode())

    def read_response(self):
        line = self.ser.readline().decode().strip()
        try:
            return json.loads(line)
        except json.JSONDecodeError:
            return {}

    def read_data(self):
        self.send_json({"read": True})
        data = self.read_response()

        if "device" in data:
            self.deviceSpin.setValue(data["device"])
            self.nozzleSpin.setValue(data["nozzles"])
            for i, val in enumerate(data["ids"]):
                self.nozzleSpins[i].setValue(val)
        else:
            QMessageBox.warning(self, "Error", "Failed to read data from ESP32.")

    def write_data(self):
        ids = [spin.value() for spin in self.nozzleSpins]
        obj = {
            "write": {
                "device": self.deviceSpin.value(),
                "nozzles": self.nozzleSpin.value(),
                "ids": ids
            }
        }
        self.send_json(obj)
        res = self.read_response()
        if res.get("status") == "ok":
            QMessageBox.information(self, "Success", "EEPROM updated.")
        else:
            QMessageBox.warning(self, "Error", "Failed to write to ESP32.")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    win = EEPROMEditor()
    win.show()
    sys.exit(app.exec_())
