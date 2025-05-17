# import sys
# from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QPushButton, QComboBox
# import serial

# class SerialSender(QWidget):
#     def __init__(self):
#         super().__init__()
#         self.setWindowTitle("Serial Sender with Parity")
#         self.serial_port = None
#         self.init_ui()

#     def init_ui(self):
#         layout = QVBoxLayout()

#         # Dropdown for parity selection
#         self.parity_box = QComboBox()
#         self.parity_box.addItems(["None", "Even", "Odd", "Mark", "Space"])
#         layout.addWidget(self.parity_box)

#         send_btn = QPushButton("Send 'Hello'")
#         send_btn.clicked.connect(self.send_data)
#         layout.addWidget(send_btn)

#         self.setLayout(layout)

#     def send_data(self):
#         parity_option = self.parity_box.currentText()
#         parity_map = {
#             "None": serial.PARITY_NONE,
#             "Even": serial.PARITY_EVEN,
#             "Odd": serial.PARITY_ODD,
#             "Mark": serial.PARITY_MARK,
#             "Space": serial.PARITY_SPACE
#         }

#         # Open the serial port (adjust COM and baudrate as needed)
#         if self.serial_port is None or not self.serial_port.is_open:
#             self.serial_port = serial.Serial(
#                 port='COM4', baudrate=9600,
#                 parity=parity_map[parity_option],
#                 stopbits=serial.STOPBITS_ONE,
#                 bytesize=serial.EIGHTBITS,
#                 timeout=1
#             )

#         # Send data
#         self.serial_port.write(b'Hello\n')

# if __name__ == '__main__':
#     app = QApplication(sys.argv)
#     window = SerialSender()
#     window.show()
#     sys.exit(app.exec_())
import sys
import serial
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QComboBox, QPushButton,
    QLineEdit, QLabel
)

class SerialSender(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Serial Sender with Parity")

        self.ser = serial.Serial()
        self.ser.port = 'COM4'  # Change this to your COM port
        self.ser.baudrate = 9600
        self.ser.bytesize = serial.EIGHTBITS
        self.ser.stopbits = serial.STOPBITS_ONE
        self.ser.timeout = 1

        layout = QVBoxLayout()

        self.label = QLabel("Enter Byte (e.g. 0x01 or 1):")
        self.input = QLineEdit()
        layout.addWidget(self.label)
        layout.addWidget(self.input)

        self.parity_box = QComboBox()
        self.parity_box.addItems([
            "None", "Even", "Odd", "Mark", "Space"
        ])
        layout.addWidget(QLabel("Select Parity:"))
        layout.addWidget(self.parity_box)

        self.send_button = QPushButton("Send Byte")
        self.send_button.clicked.connect(self.send_byte)
        layout.addWidget(self.send_button)

        self.setLayout(layout)

    def send_byte(self):
        value = self.input.text().strip()
        try:
            if value.lower().startswith("0x"):
                byte = int(value, 16)
            else:
                byte = int(value)
            if not 0 <= byte <= 255:
                raise ValueError

            parity_mode = self.parity_box.currentText()
            parity_map = {
                "None": serial.PARITY_NONE,
                "Even": serial.PARITY_EVEN,
                "Odd": serial.PARITY_ODD,
                "Mark": serial.PARITY_MARK,
                "Space": serial.PARITY_SPACE
            }

            self.ser.parity = parity_map[parity_mode]

            if not self.ser.is_open:
                self.ser.open()

            self.ser.write(bytes([byte]))
            print(f"Sent: {hex(byte)} with parity: {parity_mode}")

        except Exception as e:
            print("Error:", e)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = SerialSender()
    window.show()
    sys.exit(app.exec_())
