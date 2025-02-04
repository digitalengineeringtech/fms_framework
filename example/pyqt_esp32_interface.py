import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QPushButton, QVBoxLayout, 
                             QWidget, QLabel, QHBoxLayout, QTextEdit, QLineEdit)
from PyQt5.QtCore import QThread, pyqtSignal, QTimer
from PyQt5.QtGui import QTextCursor
import serial
import time

class SerialThread(QThread):
    data_received = pyqtSignal(str)

    def __init__(self, port, baudrate):
        super().__init__()
        self.ser = serial.Serial(port, baudrate, timeout=0)
        self.running = True

    def run(self):
        while self.running:
            if self.ser.in_waiting:
                data = self.ser.readline().decode('utf-8').strip()
                self.data_received.emit(data)
            time.sleep(0.01)  # Small delay to prevent CPU hogging

    def stop(self):
        self.running = False
        self.ser.close()

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("ESP32 FreeRTOS Semaphore Monitor")
        self.setGeometry(100, 100, 600, 400)
        
        main_layout = QVBoxLayout()
        
        # Status display
        self.status_label = QLabel("Waiting for data...")
        main_layout.addWidget(self.status_label)
        
        # Log display
        self.log_display = QTextEdit()
        self.log_display.setReadOnly(True)
        main_layout.addWidget(self.log_display)
        
        # Command input
        command_layout = QHBoxLayout()
        self.command_input = QLineEdit()
        command_layout.addWidget(self.command_input)
        self.send_button = QPushButton("Send")
        self.send_button.clicked.connect(self.send_command)
        command_layout.addWidget(self.send_button)
        main_layout.addLayout(command_layout)
        
        # Task trigger buttons
        button_layout = QHBoxLayout()
        self.task1_button = QPushButton("Trigger Task 1")
        self.task1_button.clicked.connect(lambda: self.send_command("1"))
        button_layout.addWidget(self.task1_button)
        
        self.task2_button = QPushButton("Trigger Task 2")
        self.task2_button.clicked.connect(lambda: self.send_command("2"))
        button_layout.addWidget(self.task2_button)
        main_layout.addLayout(button_layout)
        
        container = QWidget()
        container.setLayout(main_layout)
        self.setCentralWidget(container)
        
        self.serial_thread = SerialThread("COM4", 115200)  # Adjust port as needed
        self.serial_thread.data_received.connect(self.update_log)
        self.serial_thread.start()

        # Timer for updating status
        self.status_timer = QTimer()
        self.status_timer.timeout.connect(self.update_status)
        self.status_timer.start(1000)  # Update every second

    def send_command(self, command=None):
        if command is None:
            command = self.command_input.text()
        if command:
            self.serial_thread.ser.write(command.encode())
            self.command_input.clear()
            self.log_display.append(f"Sent command: {command}")

    def update_log(self, data):
        self.log_display.append(data)
        self.log_display.moveCursor(QTextCursor.End)

    def update_status(self):
        # This method could be expanded to show more detailed status information
        self.status_label.setText(f"Last update: {time.strftime('%H:%M:%S')}")

    def closeEvent(self, event):
        self.serial_thread.stop()
        self.serial_thread.wait()
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())