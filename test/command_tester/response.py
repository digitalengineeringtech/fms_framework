import sys
import serial
import threading
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QTextEdit, QPushButton

class SerialResponder(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Serial Responder UI")
        self.setGeometry(100, 100, 600, 300)

        self.layout = QVBoxLayout()
        self.log = QTextEdit()
        self.log.setReadOnly(True)
        self.layout.addWidget(self.log)

        self.toggle_btn = QPushButton("Start Listening")
        self.toggle_btn.clicked.connect(self.toggle_serial)
        self.layout.addWidget(self.toggle_btn)

       # self.nozzle_btn = QPushButton("Nozzle2 Lift")
       # self.nozzle_btn.clicked.connect(self.send_nozzle_cmd)
       # self.layout.addWidget(self.nozzle_btn)

        self.setLayout(self.layout)

        self.ser = None
        self.running = False
        self.listen_thread = None

    def log_msg(self, msg):
        self.log.append(msg)

    def toggle_serial(self):
        if not self.running:
            try:
                self.ser = serial.Serial('COM11', 9600, timeout=0.1)  # Adjust COM port
                self.running = True
                self.listen_thread = threading.Thread(target=self.listen_serial, daemon=True)
                self.listen_thread.start()
                self.toggle_btn.setText("Stop Listening")
                self.log_msg("Started listening on COM11...")
            except Exception as e:
                self.log_msg(f"Error: {e}")
        else:
            self.running = False
            if self.ser:
                self.ser.close()
            self.toggle_btn.setText("Start Listening")
            self.log_msg("Stopped listening.")
    def send_nozzle_cmd(self):
        if self.ser and self.ser.is_open:
            nozzle_cmd = bytes.fromhex("02 03 12 72 1D CD E4 83 02 02 01 19")
            self.ser.write(nozzle_cmd)
            self.log_msg(f"Sent: {nozzle_cmd.hex().upper()}")
        else:
            self.log_msg("Serial port is not open.")

    def listen_serial(self):
        expected_cmd = bytes.fromhex("01 03 12 EA")
        expected_cmd2 = bytes.fromhex("02 03 12 E9")
        response_cmd = bytes.fromhex("01 0B 12 57 1D CD 6C 8E 02 01 02 A4")
        response_cmd2 = bytes.fromhex("02 0B 12 57 1D CD 6C 8E 02 01 02 A4")

        buffer = bytearray()
        while self.running:
            if self.ser.in_waiting:
                byte = self.ser.read(1)
                if byte:
                    buffer += byte
                    # Look for exact command
                    if len(buffer) >= 4:
                        if buffer[-4:] == expected_cmd:
                            self.log_msg(f"Received: {buffer[-4:].hex().upper()}")
                            self.ser.write(response_cmd)
                            self.log_msg(f"Sent: {response_cmd.hex().upper()}")
                            buffer.clear()  # clear after handling
                        elif buffer[-4:] == expected_cmd2:
                            self.log_msg(f"Received: {buffer[-4:].hex().upper()}")
                            self.ser.write(response_cmd2)
                            self.log_msg(f"Sent: {response_cmd2.hex().upper()}")
                            buffer.clear()
                        elif len(buffer) > 20:
                            buffer.clear()  # clear junk

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = SerialResponder()
    window.show()
    sys.exit(app.exec_())
