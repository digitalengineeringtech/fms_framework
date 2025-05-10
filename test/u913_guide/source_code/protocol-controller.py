import sys
import serial
import serial.tools.list_ports
import time
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QLabel, QComboBox, QSpinBox, QPushButton, 
                             QGroupBox, QFormLayout, QStatusBar, QMessageBox)
from PyQt5.QtCore import Qt, QTimer

class ProtocolController(QMainWindow):
    def __init__(self):
        super().__init__()
        self.serial_port = None
        self.initUI()
        
    def initUI(self):
        self.setWindowTitle('ESP32 Protocol Controller')
        self.setGeometry(100, 100, 500, 400)
        
        # Main widget and layout
        main_widget = QWidget()
        main_layout = QVBoxLayout()
        
        # Connection group
        connection_group = QGroupBox("Serial Connection")
        connection_layout = QFormLayout()
        
        # Port selection
        self.port_combo = QComboBox()
        self.refresh_ports()
        connection_layout.addRow("Port:", self.port_combo)
        
        # Refresh and connect buttons
        button_layout = QHBoxLayout()
        self.refresh_button = QPushButton("Refresh Ports")
        self.refresh_button.clicked.connect(self.refresh_ports)
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        button_layout.addWidget(self.refresh_button)
        button_layout.addWidget(self.connect_button)
        connection_layout.addRow("", button_layout)
        
        connection_group.setLayout(connection_layout)
        main_layout.addWidget(connection_group)
        
        # Protocol settings group
        settings_group = QGroupBox("Protocol Settings")
        settings_layout = QFormLayout()
        
        # Protocol selection
        self.protocol_combo = QComboBox()
        self.protocol_combo.addItems(["RedStar Protocol", "Lanfeng Protocol", "Tasuno Protocol"])
        settings_layout.addRow("Protocol:", self.protocol_combo)
        
        # Nozzle count
        self.nozzle_spin = QSpinBox()
        self.nozzle_spin.setRange(1, 8)
        self.nozzle_spin.setValue(4)
        settings_layout.addRow("Number of Nozzles:", self.nozzle_spin)
        
        settings_group.setLayout(settings_layout)
        main_layout.addWidget(settings_group)
        
        # Upload button
        self.upload_button = QPushButton("Upload Configuration")
        self.upload_button.clicked.connect(self.upload_config)
        self.upload_button.setEnabled(False)
        main_layout.addWidget(self.upload_button)
        
        # Status display
        status_group = QGroupBox("ESP32 Status")
        status_layout = QVBoxLayout()
        self.status_label = QLabel("Not connected")
        self.status_label.setAlignment(Qt.AlignCenter)
        status_layout.addWidget(self.status_label)
        status_group.setLayout(status_layout)
        main_layout.addWidget(status_group)
        
        # Set main layout
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)
        
        # Status bar - FIXED: Use the statusBar() method instead of creating a new attribute
        self.setStatusBar(QStatusBar())
        self.statusBar().showMessage("Ready")
        
        # Timer for reading serial data
        self.timer = QTimer()
        self.timer.timeout.connect(self.read_serial)
        self.timer.start(100)  # Check every 100ms
        
    def refresh_ports(self):
        """Refresh the list of available serial ports"""
        self.port_combo.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.port_combo.addItem(port.device)
        
        if self.port_combo.count() == 0:
            self.statusBar().showMessage("No serial ports found")  # FIXED: Use statusBar() method
        else:
            self.statusBar().showMessage(f"Found {self.port_combo.count()} serial ports")  # FIXED: Use statusBar() method
    
    def toggle_connection(self):
        """Connect to or disconnect from the selected serial port"""
        if self.serial_port is None:
            # Connect
            try:
                port = self.port_combo.currentText()
                self.serial_port = serial.Serial(port, 115200, timeout=0.1)
                time.sleep(2)  # Wait for Arduino to reset
                self.connect_button.setText("Disconnect")
                self.upload_button.setEnabled(True)
                self.statusBar().showMessage(f"Connected to {port}")  # FIXED: Use statusBar() method
                self.request_status()
            except Exception as e:
                QMessageBox.critical(self, "Connection Error", f"Failed to connect: {str(e)}")
                self.serial_port = None
        else:
            # Disconnect
            try:
                self.serial_port.close()
            except:
                pass
            self.serial_port = None
            self.connect_button.setText("Connect")
            self.upload_button.setEnabled(False)
            self.status_label.setText("Not connected")
            self.statusBar().showMessage("Disconnected")  # FIXED: Use statusBar() method
    
    def upload_config(self):
        """Upload the configuration to the ESP32"""
        if self.serial_port is None:
            return
        
        # Get settings
        protocol_index = self.protocol_combo.currentIndex()
        protocol_names = ["REDSTAR", "LANFENG", "TASUNO"]
        protocol = protocol_names[protocol_index]
        nozzles = self.nozzle_spin.value()
        
        # Create command
        command = f"CONFIG:{protocol}:{nozzles}\n"
        
        try:
            self.serial_port.write(command.encode())
            self.statusBar().showMessage(f"Configuration sent: {protocol} with {nozzles} nozzles")  # FIXED: Use statusBar() method
        except Exception as e:
            QMessageBox.critical(self, "Communication Error", f"Failed to send configuration: {str(e)}")
    
    def request_status(self):
        """Request status from ESP32"""
        if self.serial_port is None:
            return
        
        try:
            self.serial_port.write("STATUS\n".encode())
        except:
            pass
    
    def read_serial(self):
        """Read and process data from the serial port"""
        if self.serial_port is None:
            return
        
        try:
            if self.serial_port.in_waiting:
                data = self.serial_port.readline().decode('utf-8').strip()
                if data:
                    if data.startswith("STATUS:"):
                        # Process status response
                        parts = data.split(":")
                        if len(parts) >= 4:
                            protocol = parts[1]
                            nozzles = parts[2]
                            state = parts[3]
                            status_text = f"Protocol: {protocol}\nNozzles: {nozzles}\nState: {state}"
                            self.status_label.setText(status_text)
                    else:
                        # Just log other messages
                        self.statusBar().showMessage(f"ESP32: {data}")  # FIXED: Use statusBar() method
        except Exception as e:
            self.statusBar().showMessage(f"Serial read error: {str(e)}")  # FIXED: Use statusBar() method

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = ProtocolController()
    window.show()
    sys.exit(app.exec_())
