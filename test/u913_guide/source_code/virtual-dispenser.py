import sys
import time
import random
import serial
import serial.tools.list_ports
import threading
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
                           QLabel, QPushButton, QComboBox, QGroupBox, QFormLayout, 
                           QDoubleSpinBox, QSpinBox, QProgressBar, QFrame, QTextEdit,
                           QSplitter, QMessageBox, QCheckBox, QGridLayout, QLineEdit)
from PyQt5.QtCore import Qt, QTimer, pyqtSignal, QThread, QMutex
from PyQt5.QtGui import QFont, QPainter, QColor, QPen, QBrush, QTextCursor

class SerialThread(QThread):
    """Thread for handling serial communication without blocking the UI"""
    data_received = pyqtSignal(bytes)
    connection_status = pyqtSignal(bool)
    
    def __init__(self, port=None, baudrate=19200):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.running = False
        self.mutex = QMutex()
        
    def connect_serial(self, port, baudrate):
        self.port = port
        self.baudrate = baudrate
        try:
            if self.ser and self.ser.is_open:
                self.ser.close()
            
            self.ser = serial.Serial(port, baudrate, timeout=1)
            self.running = True
            self.connection_status.emit(True)
            return True
        except Exception as e:
            print(f"Serial connection error: {e}")
            self.connection_status.emit(False)
            return False
    
    def disconnect_serial(self):
        self.running = False
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.connection_status.emit(False)
    
    def send_data(self, data):
        if self.ser and self.ser.is_open:
            try:
                self.mutex.lock()
                self.ser.write(data)
                self.mutex.unlock()
                return True
            except Exception as e:
                print(f"Error sending data: {e}")
                self.mutex.unlock()
        return False
    
    def run(self):
        while self.running:
            if self.ser and self.ser.is_open:
                try:
                    if self.ser.in_waiting:
                        self.mutex.lock()
                        data = self.ser.read(self.ser.in_waiting)
                        self.mutex.unlock()
                        if data:
                            self.data_received.emit(data)
                except Exception as e:
                    print(f"Error reading data: {e}")
                    self.running = False
                    self.connection_status.emit(False)
            time.sleep(0.05)

class DispenserDisplay(QFrame):
    """Custom widget to display the dispenser LCD screen with a realistic look"""
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setMinimumSize(500, 300)  # Much larger display
        self.setFrameStyle(QFrame.Panel | QFrame.Sunken)
        self.setStyleSheet("background-color: #000000; color: #00ff00; border: 3px solid #636e72;")
        
        self.volume = 0.0
        self.amount = 0.0
        self.price = 0.0
        self.status = "READY"
        
    def paintEvent(self, event):
        super().paintEvent(event)
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Set background
        painter.fillRect(self.rect(), QColor("#000000"))
        
        width = self.width()
        height = self.height()
        
        # Draw header
        header_font = QFont("Arial", 16, QFont.Bold)
        painter.setFont(header_font)
        painter.setPen(QColor("#00ff00"))
        painter.drawText(10, 30, "FUEL DISPENSER")
        
        # Draw status with larger font
        status_font = QFont("Arial", 18, QFont.Bold)
        painter.setFont(status_font)
        painter.setPen(QColor("#ffff00"))
        painter.drawText(width - 200, 30, self.status)
        
        # Draw separator line
        painter.setPen(QPen(QColor("#00ff00"), 2))
        painter.drawLine(10, 40, width - 10, 40)
        
        # Set font for labels
        label_font = QFont("Arial", 14)
        
        # Set font for values - use a digital-looking font
        value_font = QFont("Courier New", 36, QFont.Bold)
        
        # Price section
        painter.setPen(QColor("#00ff00"))
        painter.setFont(label_font)
        painter.drawText(20, 70, "PRICE PER LITER:")
        
        painter.setFont(value_font)
        painter.setPen(QColor("#ff0000"))  # Red for price
        price_text = f"${self.price:.2f}"
        price_rect = painter.fontMetrics().boundingRect(price_text)
        painter.drawText(width - price_rect.width() - 20, 70, price_text)
        
        # Volume section
        painter.setPen(QColor("#00ff00"))
        painter.setFont(label_font)
        painter.drawText(20, 140, "VOLUME:")
        
        painter.setFont(value_font)
        painter.setPen(QColor("#ffffff"))  # White for volume
        volume_text = f"{self.volume:.3f} L"
        volume_rect = painter.fontMetrics().boundingRect(volume_text)
        painter.drawText(width - volume_rect.width() - 20, 140, volume_text)
        
        # Amount section
        painter.setPen(QColor("#00ff00"))
        painter.setFont(label_font)
        painter.drawText(20, 210, "AMOUNT:")
        
        painter.setFont(value_font)
        painter.setPen(QColor("#ffffff"))  # White for amount
        amount_text = f"${self.amount:.2f}"
        amount_rect = painter.fontMetrics().boundingRect(amount_text)
        painter.drawText(width - amount_rect.width() - 20, 210, amount_text)
        
        # Draw separator line at bottom
        painter.setPen(QPen(QColor("#00ff00"), 2))
        painter.drawLine(10, height - 40, width - 10, height - 40)
        
        # Draw company name or additional info
        info_font = QFont("Arial", 12)
        painter.setFont(info_font)
        painter.setPen(QColor("#00ff00"))
        painter.drawText(10, height - 15, "VIRTUAL FUEL SYSTEMS")
        
    def update_display(self, volume=None, amount=None, price=None, status=None):
        if volume is not None:
            self.volume = volume
        if amount is not None:
            self.amount = amount
        if price is not None:
            self.price = price
        if status is not None:
            self.status = status
        self.update()

class NozzleWidget(QFrame):
    """Custom widget to display the nozzle status"""
    nozzle_lifted = pyqtSignal(bool)
    
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setMinimumSize(150, 250)
        self.setFrameStyle(QFrame.Panel | QFrame.Sunken)
        self.setStyleSheet("background-color: #f0f0f0;")
        
        self.lifted = False
        self.dispensing = False
        
        # Create a button to lift/replace the nozzle
        layout = QVBoxLayout()
        self.lift_button = QPushButton("Lift Nozzle")
        self.lift_button.clicked.connect(self.toggle_nozzle)
        self.lift_button.setMinimumHeight(40)
        self.lift_button.setStyleSheet("font-weight: bold; background-color: #2ecc71;")
        layout.addWidget(self.lift_button)
        layout.addStretch()
        
        self.setLayout(layout)
    
    def toggle_nozzle(self):
        self.lifted = not self.lifted
        if self.lifted:
            self.lift_button.setText("Replace Nozzle")
            self.lift_button.setStyleSheet("font-weight: bold; background-color: #e74c3c;")
        else:
            self.lift_button.setText("Lift Nozzle")
            self.lift_button.setStyleSheet("font-weight: bold; background-color: #2ecc71;")
        self.nozzle_lifted.emit(self.lifted)
        self.update()
    
    def set_dispensing(self, dispensing):
        self.dispensing = dispensing
        self.update()
    
    def paintEvent(self, event):
        super().paintEvent(event)
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Draw nozzle
        width = self.width()
        height = self.height()
        
        # Nozzle base
        painter.setPen(QPen(Qt.black, 2))
        painter.setBrush(QBrush(QColor("#636e72")))
        painter.drawRect(width//2 - 20, height - 70, 40, 50)
        
        # Nozzle hose
        painter.setPen(QPen(Qt.black, 4))
        if self.lifted:
            # Nozzle is lifted - draw it to the side
            painter.drawLine(width//2, height - 70, width - 30, height//2)
            
            # Nozzle head
            painter.setBrush(QBrush(QColor("#d63031") if self.dispensing else QColor("#0984e3")))
            painter.drawRoundedRect(width - 60, height//2 - 20, 50, 40, 8, 8)
            
            # Nozzle spout
            painter.drawLine(width - 35, height//2 + 20, width - 35, height//2 + 40)
            
            # Dispensing indicator
            if self.dispensing:
                painter.setPen(QPen(QColor("#00ff00"), 3))
                for i in range(4):
                    y_pos = height//2 + 45 + i*10
                    painter.drawLine(width - 45, y_pos, width - 25, y_pos)
        else:
            # Nozzle is in the holder
            painter.drawLine(width//2, height - 70, width//2, height - 120)
            
            # Nozzle head
            painter.setBrush(QBrush(QColor("#0984e3")))
            painter.drawRoundedRect(width//2 - 25, height - 160, 50, 40, 8, 8)

class LogWidget(QTextEdit):
    """Widget to display communication logs"""
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setReadOnly(True)
        self.setFont(QFont("Courier New", 9))
        self.setStyleSheet("background-color: #2d3436; color: #dfe6e9;")
    
    def log_message(self, message, message_type="INFO"):
        timestamp = time.strftime("%H:%M:%S")
        color = "#dfe6e9"  # Default color
        
        if message_type == "TX":
            color = "#55efc4"
        elif message_type == "RX":
            color = "#74b9ff"
        elif message_type == "ERROR":
            color = "#ff7675"
        elif message_type == "SUCCESS":
            color = "#00b894"
        
        self.append(f'<span style="color:{color};">[{timestamp}] [{message_type}] {message}</span>')
        self.moveCursor(QTextCursor.End)

class VirtualDispenser(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Virtual Fuel Dispenser")
        self.setMinimumSize(800, 600)
        
        # Initialize serial communication
        self.serial_thread = SerialThread()
        self.serial_thread.data_received.connect(self.handle_serial_data)
        self.serial_thread.connection_status.connect(self.update_connection_status)
        
        # Initialize dispenser state
        self.pump_id = 1
        self.price_per_liter = 3.99
        self.volume = 0.0
        self.amount = 0.0
        self.status = "IDLE"
        self.authorized = False
        self.dispensing = False
        self.preset_amount = 0.0
        self.preset_volume = 0.0
        self.preset_type = None  # "AMOUNT" or "VOLUME"
        self.flow_rate = 0.5  # Liters per second
        
        # Setup UI
        self.setup_ui()
        
        # Start with a scan of available ports
        self.scan_ports()
        
        # Setup timer for dispensing simulation
        self.dispense_timer = QTimer(self)
        self.dispense_timer.timeout.connect(self.update_dispensing)
        self.dispense_timer.setInterval(100)  # Update every 100ms
        
        # Setup timer for status updates
        self.status_timer = QTimer(self)
        self.status_timer.timeout.connect(self.send_status_update)
        self.status_timer.setInterval(1000)  # Send status every second
        self.status_timer.start()
    
    def setup_ui(self):
        # Main widget and layout
        main_widget = QWidget()
        main_layout = QVBoxLayout()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)
        
        # Connection settings
        connection_group = QGroupBox("Connection Settings")
        connection_layout = QHBoxLayout()
        
        connection_layout.addWidget(QLabel("Port:"))
        self.port_combo = QComboBox()
        connection_layout.addWidget(self.port_combo)
        
        connection_layout.addWidget(QLabel("Baud Rate:"))
        self.baud_combo = QComboBox()
        self.baud_combo.addItems(["9600", "19200", "38400", "57600", "115200"])
        self.baud_combo.setCurrentText("19200")
        connection_layout.addWidget(self.baud_combo)
        
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        connection_layout.addWidget(self.connect_button)
        
        scan_button = QPushButton("Scan Ports")
        scan_button.clicked.connect(self.scan_ports)
        connection_layout.addWidget(scan_button)
        
        connection_layout.addWidget(QLabel("Status:"))
        self.connection_status = QLabel("Disconnected")
        self.connection_status.setStyleSheet("color: red; font-weight: bold;")
        connection_layout.addWidget(self.connection_status)
        
        connection_group.setLayout(connection_layout)
        main_layout.addWidget(connection_group)
        
        # Dispenser configuration
        config_group = QGroupBox("Dispenser Configuration")
        config_layout = QFormLayout()
        
        self.pump_id_spin = QSpinBox()
        self.pump_id_spin.setRange(1, 32)
        self.pump_id_spin.setValue(1)
        self.pump_id_spin.valueChanged.connect(self.update_pump_id)
        config_layout.addRow("Pump ID:", self.pump_id_spin)
        
        self.price_spin = QDoubleSpinBox()
        self.price_spin.setRange(0.01, 99.99)
        self.price_spin.setValue(3.99)
        self.price_spin.setSingleStep(0.01)
        self.price_spin.setDecimals(2)
        self.price_spin.valueChanged.connect(self.update_price)
        config_layout.addRow("Price per Liter:", self.price_spin)
        
        self.flow_rate_spin = QDoubleSpinBox()
        self.flow_rate_spin.setRange(0.1, 2.0)
        self.flow_rate_spin.setValue(0.5)
        self.flow_rate_spin.setSingleStep(0.1)
        self.flow_rate_spin.setDecimals(1)
        self.flow_rate_spin.valueChanged.connect(self.update_flow_rate)
        config_layout.addRow("Flow Rate (L/s):", self.flow_rate_spin)
        
        self.auto_authorize = QCheckBox("Auto-authorize when nozzle lifted")
        self.auto_authorize.setChecked(True)
        config_layout.addRow("", self.auto_authorize)
        
        config_group.setLayout(config_layout)
        main_layout.addWidget(config_group)
        
        # Dispenser display and controls
        dispenser_layout = QVBoxLayout()  # Changed to vertical layout
        
        # Display at the top
        self.display = DispenserDisplay()
        self.display.update_display(price=self.price_per_liter, status="IDLE")
        dispenser_layout.addWidget(self.display)
        
        # Controls below the display
        controls_layout = QHBoxLayout()
        
        # Left side - status and control buttons
        status_controls = QVBoxLayout()
        
        status_layout = QHBoxLayout()
        self.status_label = QLabel("Status: IDLE")
        self.status_label.setStyleSheet("font-weight: bold; font-size: 14px;")
        status_layout.addWidget(self.status_label)
        status_layout.addStretch()
        
        buttons_layout = QHBoxLayout()
        self.authorize_button = QPushButton("Authorize")
        self.authorize_button.clicked.connect(self.authorize_dispenser)
        self.authorize_button.setMinimumHeight(40)
        buttons_layout.addWidget(self.authorize_button)
        
        self.stop_button = QPushButton("Stop")
        self.stop_button.clicked.connect(self.stop_dispensing)
        self.stop_button.setEnabled(False)
        self.stop_button.setMinimumHeight(40)
        buttons_layout.addWidget(self.stop_button)
        
        status_controls.addLayout(status_layout)
        status_controls.addLayout(buttons_layout)
        
        # Right side - nozzle
        self.nozzle = NozzleWidget()
        self.nozzle.nozzle_lifted.connect(self.handle_nozzle_lifted)
        
        # Add both sides to the controls layout
        controls_layout.addLayout(status_controls, 2)  # Give more space to controls
        controls_layout.addWidget(self.nozzle, 1)
        
        dispenser_layout.addLayout(controls_layout)
        
        # Preset controls
        preset_group = QGroupBox("Preset")
        preset_layout = QFormLayout()
        
        self.preset_type_combo = QComboBox()
        self.preset_type_combo.addItems(["Amount ($)", "Volume (L)"])
        preset_layout.addRow("Preset Type:", self.preset_type_combo)
        
        self.preset_value_spin = QDoubleSpinBox()
        self.preset_value_spin.setRange(0.0, 999.99)
        self.preset_value_spin.setValue(20.0)
        self.preset_value_spin.setSingleStep(1.0)
        self.preset_value_spin.setDecimals(2)
        preset_layout.addRow("Preset Value:", self.preset_value_spin)
        
        self.set_preset_button = QPushButton("Set Preset")
        self.set_preset_button.clicked.connect(self.set_preset)
        preset_layout.addRow("", self.set_preset_button)
        
        preset_group.setLayout(preset_layout)
        dispenser_layout.addWidget(preset_group)
        
        main_layout.addLayout(dispenser_layout)
        
        # Log section
        log_group = QGroupBox("Communication Log")
        log_layout = QVBoxLayout()
        
        self.log_widget = LogWidget()
        log_layout.addWidget(self.log_widget)
        
        log_control_layout = QHBoxLayout()
        
        clear_log_button = QPushButton("Clear Log")
        clear_log_button.clicked.connect(self.log_widget.clear)
        log_control_layout.addWidget(clear_log_button)
        
        log_layout.addLayout(log_control_layout)
        
        log_group.setLayout(log_layout)
        main_layout.addWidget(log_group)
    
    def scan_ports(self):
        """Scan for available serial ports"""
        self.port_combo.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.port_combo.addItem(port.device)
        
        if self.port_combo.count() == 0:
            self.port_combo.addItem("No ports available")
    
    def toggle_connection(self):
        """Connect or disconnect from the serial port"""
        if self.serial_thread.running:
            # Disconnect
            self.serial_thread.disconnect_serial()
            self.connect_button.setText("Connect")
        else:
            # Connect
            port = self.port_combo.currentText()
            baudrate = int(self.baud_combo.currentText())
            
            if port and port != "No ports available":
                if self.serial_thread.connect_serial(port, baudrate):
                    self.serial_thread.start()
                    self.connect_button.setText("Disconnect")
                    self.log_widget.log_message(f"Connected to {port} at {baudrate} baud", "SUCCESS")
                else:
                    QMessageBox.critical(self, "Connection Error", f"Failed to connect to {port}")
            else:
                QMessageBox.warning(self, "No Port Selected", "Please select a valid port")
    
    def update_connection_status(self, connected):
        """Update the connection status display"""
        if connected:
            self.connection_status.setText("Connected")
            self.connection_status.setStyleSheet("color: green; font-weight: bold;")
            self.connect_button.setText("Disconnect")
        else:
            self.connection_status.setText("Disconnected")
            self.connection_status.setStyleSheet("color: red; font-weight: bold;")
            self.connect_button.setText("Connect")
    
    def handle_serial_data(self, data):
        """Handle data received from the serial port"""
        hex_str = ' '.join(f'{b:02X}' for b in data)
        self.log_widget.log_message(f"Received: {hex_str}", "RX")
        
        # Process the received command
        self.process_command(data)
    
    def process_command(self, data):
        """Process commands received from the controller"""
        if len(data) < 4:
            return
        
        # Check if this command is for this pump
        pump_id = data[0]
        if pump_id != self.pump_id:
            return
        
        command = data[1]
        
        # Read state command (0x03, 0x12)
        if command == 0x03 and data[2] == 0x12:
            self.send_state_response()
        
        # Authorize command (0x03, 0x08)
        elif command == 0x03 and data[2] == 0x08:
            self.authorize_dispenser()
            self.send_authorize_response()
        
        # Preset command (0x07)
        elif command == 0x07:
            preset_type = data[2]
            
            # Extract the preset value (4 bytes, big endian)
            value = (data[3] << 24) | (data[4] << 16) | (data[5] << 8) | data[6]
            
            if preset_type == 0x89:  # Amount preset
                self.preset_amount = value / 100.0  # Convert cents to dollars
                self.preset_volume = 0.0
                self.preset_type = "AMOUNT"
                self.log_widget.log_message(f"Received amount preset: ${self.preset_amount:.2f}", "INFO")
            
            elif preset_type == 0x8B:  # Volume preset
                self.preset_volume = value / 1000.0  # Convert milliliters to liters
                self.preset_amount = 0.0
                self.preset_type = "VOLUME"
                self.log_widget.log_message(f"Received volume preset: {self.preset_volume:.2f} L", "INFO")
            
            # Send preset response
            self.send_preset_response()
        
        # Price change command (0x07, 0x80)
        elif command == 0x07 and data[2] == 0x80:
            # Extract the price (4 bytes, big endian)
            price = (data[3] << 24) | (data[4] << 16) | (data[5] << 8) | data[6]
            price = price / 100.0  # Convert cents to dollars
            
            self.price_per_liter = price
            self.price_spin.setValue(price)
            self.display.update_display(price=price)
            
            self.log_widget.log_message(f"Price changed to ${price:.2f}/L", "INFO")
            
            # Send price change response
            self.send_price_change_response()
        
        # Read fuel data command (0x03, 0x8F)
        elif command == 0x03 and data[2] == 0x8F:
            self.send_fuel_data_response()
    
    def send_state_response(self):
        """Send the current state of the dispenser"""
        response = bytearray(10)
        response[0] = self.pump_id
        response[1] = 0x12  # Response code
        
        # Status byte
        if self.status == "IDLE":
            response[2] = 0x57 if self.pump_id == 1 else 0x56  # Idle status
        elif self.status == "AUTHORIZED":
            response[2] = 0x77 if self.pump_id == 1 else 0x76  # Authorized status
        elif self.status == "DISPENSING":
            response[2] = 0x72 if self.pump_id == 1 else 0x73  # Dispensing status
        elif self.status == "COMPLETED":
            response[2] = 0x53 if self.pump_id == 1 else 0x52  # Completed status
        
        # Fill the rest with zeros for now
        for i in range(3, 10):
            response[i] = 0x00
        
        self.send_serial_data(response)
        
        hex_str = ' '.join(f'{b:02X}' for b in response)
        self.log_widget.log_message(f"Sent state response: {hex_str}", "TX")
    
    def send_authorize_response(self):
        """Send response to authorize command"""
        response = bytearray(5)
        response[0] = self.pump_id
        response[1] = 0x03  # Command echo
        response[2] = 0x08  # Subcommand echo
        response[3] = 0x00  # Status OK
        response[4] = 0x00  # Reserved
        
        self.send_serial_data(response)
        
        hex_str = ' '.join(f'{b:02X}' for b in response)
        self.log_widget.log_message(f"Sent authorize response: {hex_str}", "TX")
    
    def send_preset_response(self):
        """Send response to preset command"""
        response = bytearray(5)
        response[0] = self.pump_id
        response[1] = 0x07  # Command echo
        response[2] = 0x00  # Status OK
        response[3] = 0x00  # Reserved
        response[4] = 0x00  # Reserved
        
        self.send_serial_data(response)
        
        hex_str = ' '.join(f'{b:02X}' for b in response)
        self.log_widget.log_message(f"Sent preset response: {hex_str}", "TX")
    
    def send_price_change_response(self):
        """Send response to price change command"""
        response = bytearray(5)
        response[0] = self.pump_id
        response[1] = 0x07  # Command echo
        response[2] = 0x00  # Status OK
        response[3] = 0x00  # Reserved
        response[4] = 0x00  # Reserved
        
        self.send_serial_data(response)
        
        hex_str = ' '.join(f'{b:02X}' for b in response)
        self.log_widget.log_message(f"Sent price change response: {hex_str}", "TX")
    
    def send_fuel_data_response(self):
        """Send fuel dispensing data"""
        response = bytearray(12)
        response[0] = self.pump_id
        response[1] = 0x12  # Response code
        response[2] = 0x5F if self.pump_id == 1 else 0x5B  # Fuel data status
        
        # Amount (4 bytes, big endian)
        amount_int = int(self.amount * 100)  # Convert to cents
        response[4] = (amount_int >> 24) & 0xFF
        response[5] = (amount_int >> 16) & 0xFF
        response[6] = (amount_int >> 8) & 0xFF
        response[7] = amount_int & 0xFF
        
        # Volume (4 bytes, big endian)
        volume_int = int(self.volume * 1000)  # Convert to milliliters
        response[8] = (volume_int >> 24) & 0xFF
        response[9] = (volume_int >> 16) & 0xFF
        response[10] = (volume_int >> 8) & 0xFF
        response[11] = volume_int & 0xFF
        
        self.send_serial_data(response)
        
        hex_str = ' '.join(f'{b:02X}' for b in response)
        self.log_widget.log_message(f"Sent fuel data response: {hex_str}", "TX")
    
    def send_serial_data(self, data):
        """Send data to the serial port"""
        if self.serial_thread.running:
            self.serial_thread.send_data(data)
    
    def update_pump_id(self, value):
        """Update the pump ID"""
        self.pump_id = value
    
    def update_price(self, value):
        """Update the price per liter"""
        self.price_per_liter = value
        self.display.update_display(price=value)
    
    def update_flow_rate(self, value):
        """Update the flow rate"""
        self.flow_rate = value
    
    def handle_nozzle_lifted(self, lifted):
        """Handle nozzle lift/replace events"""
        if lifted:
            self.log_widget.log_message("Nozzle lifted", "INFO")
            if self.auto_authorize.isChecked():
                self.authorize_dispenser()
        else:
            self.log_widget.log_message("Nozzle replaced", "INFO")
            self.stop_dispensing()
            self.status = "IDLE"
            self.update_status_display()
    
    def authorize_dispenser(self):
        """Authorize the dispenser for fueling"""
        if self.status != "DISPENSING":
            self.status = "AUTHORIZED"
            self.authorized = True
            self.update_status_display()
            self.log_widget.log_message("Dispenser authorized", "INFO")
            
            # Enable stop button
            self.stop_button.setEnabled(True)
            
            # If nozzle is already lifted, start dispensing
            if self.nozzle.lifted:
                self.start_dispensing()
    
    def start_dispensing(self):
        """Start dispensing fuel"""
        if self.authorized and self.nozzle.lifted:
            self.status = "DISPENSING"
            self.dispensing = True
            self.nozzle.set_dispensing(True)
            self.update_status_display()
            self.log_widget.log_message("Dispensing started", "INFO")
            
            # Start the dispensing timer
            self.dispense_timer.start()
    
    def stop_dispensing(self):
        """Stop dispensing fuel"""
        if self.dispensing:
            self.dispense_timer.stop()
            self.dispensing = False
            self.authorized = False
            self.nozzle.set_dispensing(False)
            
            if self.volume > 0:
                self.status = "COMPLETED"
            else:
                self.status = "IDLE"
            
            self.update_status_display()
            self.log_widget.log_message("Dispensing stopped", "INFO")
            
            # Disable stop button
            self.stop_button.setEnabled(False)
    
    def update_dispensing(self):
        """Update the dispensing simulation"""
        if not self.dispensing:
            return
        
        # Calculate the amount dispensed in this interval
        interval_seconds = self.dispense_timer.interval() / 1000.0
        volume_increment = self.flow_rate * interval_seconds
        
        # Update volume and amount
        self.volume += volume_increment
        self.amount = self.volume * self.price_per_liter
        
        # Check if we've reached a preset limit
        if self.preset_type == "AMOUNT" and self.amount >= self.preset_amount:
            self.volume = self.preset_amount / self.price_per_liter
            self.amount = self.preset_amount
            self.stop_dispensing()
        elif self.preset_type == "VOLUME" and self.volume >= self.preset_volume:
            self.volume = self.preset_volume
            self.amount = self.volume * self.price_per_liter
            self.stop_dispensing()
        
        # Update the display
        self.display.update_display(volume=self.volume, amount=self.amount)
    
    def update_status_display(self):
        """Update the status display"""
        self.status_label.setText(f"Status: {self.status}")
        self.display.update_display(status=self.status)
    
    def set_preset(self):
        """Set a preset amount or volume"""
        preset_type = self.preset_type_combo.currentText()
        preset_value = self.preset_value_spin.value()
        
        if preset_type == "Amount ($)":
            self.preset_amount = preset_value
            self.preset_volume = 0.0
            self.preset_type = "AMOUNT"
            self.log_widget.log_message(f"Preset amount set to ${preset_value:.2f}", "INFO")
        else:  # Volume (L)
            self.preset_volume = preset_value
            self.preset_amount = 0.0
            self.preset_type = "VOLUME"
            self.log_widget.log_message(f"Preset volume set to {preset_value:.2f} L", "INFO")
    
    def send_status_update(self):
        """Send periodic status updates to the controller"""
        if not self.serial_thread.running:
            return
        
        # Only send if we're in a state that needs reporting
        if self.status in ["AUTHORIZED", "DISPENSING", "COMPLETED"]:
            # For now, just send the state response
            self.send_state_response()

def main():
    app = QApplication(sys.argv)
    
    # Set application style
    app.setStyle("Fusion")
    
    window = VirtualDispenser()
    window.show()
    
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
