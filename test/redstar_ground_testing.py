import sys
import time
import binascii
from PyQt5.QtWidgets import (QApplication, QMainWindow, QTabWidget, QWidget, QVBoxLayout, 
                            QHBoxLayout, QGridLayout, QLabel, QLineEdit, QPushButton, 
                            QComboBox, QGroupBox, QTextEdit, QCheckBox, QSpinBox, 
                            QDoubleSpinBox, QTableWidget, QTableWidgetItem, QHeaderView,
                            QSplitter, QFrame, QRadioButton, QButtonGroup, QScrollArea,
                            QStatusBar, QMessageBox)
from PyQt5.QtCore import Qt, QTimer, pyqtSignal, QThread
from PyQt5.QtGui import QFont, QColor, QTextCursor

try:
    import serial
    import serial.tools.list_ports
    SERIAL_AVAILABLE = True
except ImportError:
    SERIAL_AVAILABLE = False

class HongYangPumpControl(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("HongYang Pump Control Interface")
        self.resize(1000, 700)
        
        # Initialize variables
        self.serial_port = None
        self.connected = False
        self.pump_address = 1
        self.command_in_progress = False
        self.response_data = bytearray()
        self.expected_response_length = 0
        
        # Setup timers
        self.timer_receiving = QTimer(self)
        self.timer_receiving.timeout.connect(self.timer_receiving_handler)
        self.timer_receiving.setInterval(20)
        
        self.timer_status = QTimer(self)
        self.timer_status.timeout.connect(self.update_status)
        self.timer_status.setInterval(1000)  # Update every second
        
        # Initialize UI
        self.init_ui()
        
        # Initialize command table
        self.populate_command_table()
        
        # Start status timer
        self.timer_status.start()
        
    def init_ui(self):
        # Main layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Top section: Connection and Status
        top_layout = QHBoxLayout()
        
        # Connection group
        connection_group = QGroupBox("Connection Settings")
        connection_layout = QGridLayout(connection_group)
        
        connection_layout.addWidget(QLabel("Serial Port:"), 0, 0)
        self.port_combo = QComboBox()
        self.refresh_ports()
        connection_layout.addWidget(self.port_combo, 0, 1)
        
        refresh_button = QPushButton("Refresh")
        refresh_button.clicked.connect(self.refresh_ports)
        connection_layout.addWidget(refresh_button, 0, 2)
        
        connection_layout.addWidget(QLabel("Baud Rate:"), 1, 0)
        self.baud_combo = QComboBox()
        self.baud_combo.addItems(["2400"])  # Protocol specifies 2400 baud
        self.baud_combo.setCurrentText("2400")
        self.baud_combo.setEnabled(False)  # Disabled as per protocol
        connection_layout.addWidget(self.baud_combo, 1, 1)
        
        connection_layout.addWidget(QLabel("Pump Address (1-32):"), 2, 0)
        self.address_spin = QSpinBox()
        self.address_spin.setRange(1, 32)
        self.address_spin.setValue(1)
        connection_layout.addWidget(self.address_spin, 2, 1)
        
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        connection_layout.addWidget(self.connect_button, 3, 0, 1, 3)
        
        top_layout.addWidget(connection_group)
        
        # Status group
        status_group = QGroupBox("Pump Status")
        status_layout = QGridLayout(status_group)
        
        status_layout.addWidget(QLabel("Connection:"), 0, 0)
        self.connection_status = QLabel("Disconnected")
        self.connection_status.setStyleSheet("color: red;")
        status_layout.addWidget(self.connection_status, 0, 1)
        
        status_layout.addWidget(QLabel("Fuelling:"), 1, 0)
        self.fuelling_status = QLabel("Idle")
        status_layout.addWidget(self.fuelling_status, 1, 1)
        
        status_layout.addWidget(QLabel("Authorization Mode:"), 2, 0)
        self.auth_status = QLabel("Disabled")
        status_layout.addWidget(self.auth_status, 2, 1)
        
        status_layout.addWidget(QLabel("Handed Nozzle:"), 3, 0)
        self.nozzle_status = QLabel("No")
        status_layout.addWidget(self.nozzle_status, 3, 1)
        
        top_layout.addWidget(status_group)
        
        # Log group
        log_group = QGroupBox("Communication Log")
        log_layout = QVBoxLayout(log_group)
        
        self.log_widget = QTextEdit()
        self.log_widget.setReadOnly(True)
        log_layout.addWidget(self.log_widget)
        
        clear_log_button = QPushButton("Clear Log")
        clear_log_button.clicked.connect(self.log_widget.clear)
        log_layout.addWidget(clear_log_button)
        
        top_layout.addWidget(log_group)
        
        main_layout.addLayout(top_layout)
        
        # Tab widget for commands
        self.tab_widget = QTabWidget()
        
        # Basic Commands Tab
        basic_tab = QWidget()
        basic_layout = QVBoxLayout(basic_tab)
        
        # Price Set
        price_group = QGroupBox("Price Set (Command: 0x80)")
        price_layout = QGridLayout(price_group)
        
        price_layout.addWidget(QLabel("Price:"), 0, 0)
        self.price_spin = QDoubleSpinBox()
        self.price_spin.setRange(0, 9999.99)
        self.price_spin.setDecimals(2)
        self.price_spin.setSingleStep(0.01)
        price_layout.addWidget(self.price_spin, 0, 1)
        
        price_button = QPushButton("Set Price")
        price_button.clicked.connect(self.send_price_set)
        price_layout.addWidget(price_button, 1, 0, 1, 2)
        
        basic_layout.addWidget(price_group)
        
        # Delete Shift
        shift_group = QGroupBox("Delete Shift (Command: 0x04)")
        shift_layout = QVBoxLayout(shift_group)
        
        shift_button = QPushButton("Delete Shift")
        shift_button.clicked.connect(self.send_delete_shift)
        shift_layout.addWidget(shift_button)
        
        shift_layout.addWidget(QLabel("This will delete all accumulated data for the current shift."))
        
        shift_group.setLayout(shift_layout)
        basic_layout.addWidget(shift_group)
        
        # Lock/Unlock
        lock_group = QGroupBox("Lock/Unlock Dispenser (Commands: 0x15/0x14)")
        lock_layout = QVBoxLayout(lock_group)
        
        lock_button = QPushButton("Lock Dispenser (Authorization Mode)")
        lock_button.clicked.connect(self.send_lock_dispenser)
        lock_layout.addWidget(lock_button)
        
        unlock_button = QPushButton("Unlock Dispenser (Free Mode)")
        unlock_button.clicked.connect(self.send_unlock_dispenser)
        lock_layout.addWidget(unlock_button)
        
        lock_group.setLayout(lock_layout)
        basic_layout.addWidget(lock_group)
        
        self.tab_widget.addTab(basic_tab, "Basic Commands")
        
        # Fuelling Commands Tab
        fuelling_tab = QWidget()
        fuelling_layout = QVBoxLayout(fuelling_tab)
        
        # Fuelling Without Preset
        no_preset_group = QGroupBox("Fuelling Without Preset (Command: 0x08)")
        no_preset_layout = QVBoxLayout(no_preset_group)
        
        no_preset_button = QPushButton("Start Fuelling Without Preset")
        no_preset_button.clicked.connect(self.send_fuelling_without_preset)
        no_preset_layout.addWidget(no_preset_button)
        
        no_preset_layout.addWidget(QLabel("This will start fuelling without any preset limits."))
        
        no_preset_group.setLayout(no_preset_layout)
        fuelling_layout.addWidget(no_preset_group)
        
        # Preset Amount Fuelling
        amount_group = QGroupBox("Preset Amount Fuelling (Command: 0x89)")
        amount_layout = QGridLayout(amount_group)
        
        amount_layout.addWidget(QLabel("Amount:"), 0, 0)
        self.amount_spin = QDoubleSpinBox()
        self.amount_spin.setRange(0, 9999.99)
        self.amount_spin.setDecimals(2)
        self.amount_spin.setSingleStep(0.01)
        amount_layout.addWidget(self.amount_spin, 0, 1)
        
        amount_button = QPushButton("Start Fuelling with Amount Preset")
        amount_button.clicked.connect(self.send_preset_amount_fuelling)
        amount_layout.addWidget(amount_button, 1, 0, 1, 2)
        
        amount_group.setLayout(amount_layout)
        fuelling_layout.addWidget(amount_group)
        
        # Preset Volume Fuelling
        volume_group = QGroupBox("Preset Volume Fuelling (Command: 0x8B)")
        volume_layout = QGridLayout(volume_group)
        
        volume_layout.addWidget(QLabel("Volume:"), 0, 0)
        self.volume_spin = QDoubleSpinBox()
        self.volume_spin.setRange(0, 9999.99)
        self.volume_spin.setDecimals(2)
        self.volume_spin.setSingleStep(0.01)
        volume_layout.addWidget(self.volume_spin, 0, 1)
        
        volume_button = QPushButton("Start Fuelling with Volume Preset")
        volume_button.clicked.connect(self.send_preset_volume_fuelling)
        volume_layout.addWidget(volume_button, 1, 0, 1, 2)
        
        volume_group.setLayout(volume_layout)
        fuelling_layout.addWidget(volume_group)
        
        # Stop Fuelling
        stop_group = QGroupBox("Stop Fuelling (Command: 0x10)")
        stop_layout = QVBoxLayout(stop_group)
        
        stop_button = QPushButton("Stop Fuelling")
        stop_button.clicked.connect(self.send_stop)
        stop_button.setStyleSheet("background-color: #ffcccc;")
        stop_layout.addWidget(stop_button)
        
        stop_layout.addWidget(QLabel("This will immediately stop the current fuelling operation."))
        
        stop_group.setLayout(stop_layout)
        fuelling_layout.addWidget(stop_group)
        
        # Suspend/Resume
        suspend_group = QGroupBox("Suspend/Resume Fuelling (Commands: 0x42/0x43)")
        suspend_layout = QHBoxLayout(suspend_group)
        
        suspend_button = QPushButton("Suspend Fuelling")
        suspend_button.clicked.connect(self.send_suspend_fp)
        suspend_layout.addWidget(suspend_button)
        
        resume_button = QPushButton("Resume Fuelling")
        resume_button.clicked.connect(self.send_resume_fp)
        suspend_layout.addWidget(resume_button)
        
        suspend_group.setLayout(suspend_layout)
        fuelling_layout.addWidget(suspend_group)
        
        self.tab_widget.addTab(fuelling_tab, "Fuelling Commands")
        
        # Reading Commands Tab
        reading_tab = QWidget()
        reading_layout = QVBoxLayout(reading_tab)
        
        # Read Price
        read_price_group = QGroupBox("Read Price (Command: 0x8C)")
        read_price_layout = QVBoxLayout(read_price_group)
        
        read_price_button = QPushButton("Read Price")
        read_price_button.clicked.connect(self.send_read_price)
        read_price_layout.addWidget(read_price_button)
        
        read_price_group.setLayout(read_price_layout)
        reading_layout.addWidget(read_price_group)
        
        # Read Shift Accumulation
        read_shift_group = QGroupBox("Read Shift Accumulation (Command: 0x8D)")
        read_shift_layout = QVBoxLayout(read_shift_group)
        
        read_shift_button = QPushButton("Read Shift Accumulation")
        read_shift_button.clicked.connect(self.send_read_shift_accumulation)
        read_shift_layout.addWidget(read_shift_button)
        
        read_shift_group.setLayout(read_shift_layout)
        reading_layout.addWidget(read_shift_group)
        
        # Read Total Accumulation
        read_total_group = QGroupBox("Read Total Accumulation (Command: 0x8E)")
        read_total_layout = QVBoxLayout(read_total_group)
        
        read_total_button = QPushButton("Read Total Accumulation")
        read_total_button.clicked.connect(self.send_read_total_accumulation)
        read_total_layout.addWidget(read_total_button)
        
        read_total_group.setLayout(read_total_layout)
        reading_layout.addWidget(read_total_group)
        
        # Read Fuelled Amount
        read_fuelled_group = QGroupBox("Read Fuelled Amount (Command: 0x8F)")
        read_fuelled_layout = QVBoxLayout(read_fuelled_group)
        
        read_fuelled_button = QPushButton("Read Fuelled Amount")
        read_fuelled_button.clicked.connect(self.send_read_fuelled_amount)
        read_fuelled_layout.addWidget(read_fuelled_button)
        
        read_fuelled_group.setLayout(read_fuelled_layout)
        reading_layout.addWidget(read_fuelled_group)
        
        # Read State
        read_state_group = QGroupBox("Read State (Command: 0x12)")
        read_state_layout = QVBoxLayout(read_state_group)
        
        read_state_button = QPushButton("Read State")
        read_state_button.clicked.connect(self.send_read_state)
        read_state_layout.addWidget(read_state_button)
        
        read_state_group.setLayout(read_state_layout)
        reading_layout.addWidget(read_state_group)
        
        self.tab_widget.addTab(reading_tab, "Reading Commands")
        
        # Advanced Commands Tab
        advanced_tab = QWidget()
        advanced_layout = QVBoxLayout(advanced_tab)
        
        # Custom Command
        custom_group = QGroupBox("Custom Command")
        custom_layout = QGridLayout(custom_group)
        
        custom_layout.addWidget(QLabel("Command (Hex):"), 0, 0)
        self.custom_command = QLineEdit()
        self.custom_command.setPlaceholderText("Enter hex command (e.g., 80 01 02)")
        custom_layout.addWidget(self.custom_command, 0, 1)
        
        custom_button = QPushButton("Send Custom Command")
        custom_button.clicked.connect(self.send_custom_command)
        custom_layout.addWidget(custom_button, 1, 0, 1, 2)
        
        custom_group.setLayout(custom_layout)
        advanced_layout.addWidget(custom_group)
        
        # Command Table
        table_group = QGroupBox("Command Reference")
        table_layout = QVBoxLayout(table_group)
        
        self.command_table = QTableWidget()
        self.command_table.setColumnCount(4)
        self.command_table.setHorizontalHeaderLabels(["Command", "Description", "Hex Format", "Response"])
        self.command_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.command_table.setAlternatingRowColors(True)
        self.command_table.verticalHeader().setVisible(False)
        table_layout.addWidget(self.command_table)
        
        table_group.setLayout(table_layout)
        advanced_layout.addWidget(table_group)
        
        self.tab_widget.addTab(advanced_tab, "Advanced")
        
        main_layout.addWidget(self.tab_widget)
        
        # Status bar
        self.statusBar = QStatusBar()
        self.setStatusBar(self.statusBar)
        self.statusBar.showMessage("Ready")
        
    def refresh_ports(self):
        """Refresh the list of available serial ports"""
        self.port_combo.clear()
        
        if SERIAL_AVAILABLE:
            ports = [port.device for port in serial.tools.list_ports.comports()]
            if ports:
                self.port_combo.addItems(ports)
            else:
                self.port_combo.addItem("No ports available")
        else:
            self.port_combo.addItem("PySerial not installed")
            self.add_log("PySerial is not installed. Serial communication will be simulated.", "ERROR")
    
    def toggle_connection(self):
        """Connect to or disconnect from the serial port"""
        if not self.connected:
            port = self.port_combo.currentText()
            if port in ["No ports available", "PySerial not installed"]:
                self.add_log("No valid serial port selected", "ERROR")
                return
                
            baudrate = int(self.baud_combo.currentText())
            self.pump_address = self.address_spin.value()
            
            if SERIAL_AVAILABLE:
                try:
                    self.serial_port = serial.Serial(
                        port=port,
                        baudrate=baudrate,
                        bytesize=serial.EIGHTBITS,
                        parity=serial.PARITY_NONE,
                        stopbits=serial.STOPBITS_ONE,
                        timeout=0.1
                    )
                    
                    self.connected = True
                    self.connect_button.setText("Disconnect")
                    self.connection_status.setText("Connected")
                    self.connection_status.setStyleSheet("color: green;")
                    self.add_log(f"Connected to {port} at {baudrate} baud", "INFO")
                    self.statusBar.showMessage(f"Connected to {port}")
                    
                except Exception as e:
                    self.add_log(f"Failed to connect: {str(e)}", "ERROR")
            else:
                # Simulate connection for demo
                self.connected = True
                self.connect_button.setText("Disconnect")
                self.connection_status.setText("Connected (Simulated)")
                self.connection_status.setStyleSheet("color: green;")
                self.add_log(f"Connected to {port} at {baudrate} baud (SIMULATED)", "INFO")
                self.statusBar.showMessage(f"Connected to {port} (Simulated)")
        else:
            if self.serial_port and self.serial_port.is_open:
                self.serial_port.close()
                self.serial_port = None
                
            self.connected = False
            self.connect_button.setText("Connect")
            self.connection_status.setText("Disconnected")
            self.connection_status.setStyleSheet("color: red;")
            self.add_log("Disconnected from serial port", "INFO")
            self.statusBar.showMessage("Disconnected")
    
    def update_status(self):
        """Update the status display"""
        # In a real implementation, this would poll the pump for status
        pass
    
    def add_log(self, message, message_type="INFO"):
        """Add a message to the log widget"""
        color_map = {
            "INFO": "black",
            "ERROR": "red",
            "COMMAND": "blue",
            "RESPONSE": "green"
        }
        
        color = color_map.get(message_type, "black")
        timestamp = time.strftime("%H:%M:%S", time.localtime())
        
        self.log_widget.setTextColor(QColor(color))
        self.log_widget.append(f"[{timestamp}] [{message_type}] {message}")
        self.log_widget.moveCursor(QTextCursor.End)
    
    def calculate_checksum(self, data):
        """Calculate the checksum for the given data"""
        # The complement code of each byte added (reversal plus 1)
        checksum = 0
        for byte in data:
            checksum += byte
        
        # Take only the lowest byte and complement
        checksum = (~checksum & 0xFF) + 1
        return checksum & 0xFF
    
    def send_command(self, command_code, parameters=None):
        """Send a command to the pump"""
        if not self.connected:
            self.add_log("Not connected to serial port", "ERROR")
            return
            
        if parameters is None:
            parameters = []
            
        if self.command_in_progress:
            self.add_log("Another command is in progress", "ERROR")
            return
            
        # Command format: <Add><Length><Command><Parameter><Checksum>
        address = self.pump_address
        length = 3 + len(parameters)  # Address + Length + Command + Parameters
        
        # Construct command
        command = [address, length, command_code] + parameters
        
        # Calculate checksum
        checksum = self.calculate_checksum(command)
        
        # Add checksum to command
        command.append(checksum)
        
        # Convert to bytes
        command_bytes = bytes(command)
        
        # Log the command
        hex_command = ' '.join([f"{b:02X}" for b in command])
        self.add_log(f"Sending command: {hex_command}", "COMMAND")
        
        # Set expected response length based on command
        if command_code == 0x80:  # Price set
            self.expected_response_length = 3
        elif command_code == 0x04:  # Delete shift
            self.expected_response_length = 3
        elif command_code == 0x08:  # Fueling without preset
            self.expected_response_length = 3
        elif command_code == 0x89:  # Preset amount fuelling
            self.expected_response_length = 3
        elif command_code == 0x8B:  # Preset volume fuelling
            self.expected_response_length = 3
        elif command_code == 0x8C:  # Read price
            self.expected_response_length = 8
        elif command_code == 0x8D:  # Read shift accumulation
            self.expected_response_length = 15
        elif command_code == 0x8E:  # Read total accumulation
            self.expected_response_length = 15
        elif command_code == 0x8F:  # Read fuelled amount
            self.expected_response_length = 13
        elif command_code == 0x10:  # Stop
            self.expected_response_length = 3
        elif command_code == 0x15:  # Lock dispenser
            self.expected_response_length = 3
        elif command_code == 0x14:  # Unlock dispenser
            self.expected_response_length = 3
        elif command_code == 0x12:  # Read state
            self.expected_response_length = 10
        elif command_code == 0x42:  # Suspend FP
            self.expected_response_length = 4
        elif command_code == 0x43:  # Resume FP
            self.expected_response_length = 4
        else:
            self.expected_response_length = 3  # Default
        
        if SERIAL_AVAILABLE and self.serial_port and self.serial_port.is_open:
            try:
                # Send address byte with MARK parity
                current_parity = self.serial_port.parity
                self.serial_port.close()
                
                # Reopen with MARK parity for address byte
                self.serial_port = serial.Serial(
                    port=self.port_combo.currentText(),
                    baudrate=int(self.baud_combo.currentText()),
                    bytesize=serial.EIGHTBITS,
                    parity=serial.PARITY_MARK,
                    stopbits=serial.STOPBITS_ONE,
                    timeout=0.1
                )
                
                # Send address byte
                self.serial_port.write(bytes([address]))
                
                # Close and reopen with SPACE parity for command
                self.serial_port.close()
                self.serial_port = serial.Serial(
                    port=self.port_combo.currentText(),
                    baudrate=int(self.baud_combo.currentText()),
                    bytesize=serial.EIGHTBITS,
                    parity=serial.PARITY_SPACE,
                    stopbits=serial.STOPBITS_ONE,
                    timeout=0.1
                )
                
                # Clear input buffer
                self.serial_port.reset_input_buffer()
                
                # Send command (excluding address)
                self.serial_port.write(bytes(command[1:]))
                
                # Start receiving timer
                self.command_in_progress = True
                self.response_data = bytearray()
                self.timer_receiving.start()
                
            except Exception as e:
                self.add_log(f"Failed to send command: {str(e)}", "ERROR")
                self.command_in_progress = False
        else:
            # Simulate response
            self.simulate_response(command_code)
    
    def timer_receiving_handler(self):
        """Handle receiving data from the serial port"""
        if not self.connected or not self.command_in_progress:
            self.timer_receiving.stop()
            return
            
        if SERIAL_AVAILABLE and self.serial_port and self.serial_port.is_open:
            try:
                # Check if data is available
                if self.serial_port.in_waiting > 0:
                    # Read available data
                    data = self.serial_port.read(self.serial_port.in_waiting)
                    self.response_data.extend(data)
                    
                    # Check if we have received the expected response
                    if len(self.response_data) >= self.expected_response_length:
                        # Process the response
                        self.process_response(bytes(self.response_data[:self.expected_response_length]))
                        self.command_in_progress = False
                        self.timer_receiving.stop()
                        
            except Exception as e:
                self.add_log(f"Error receiving data: {str(e)}", "ERROR")
                self.command_in_progress = False
                self.timer_receiving.stop()
        else:
            # Timeout after 10 iterations (200ms)
            if self.timer_receiving.remainingTime() <= 0:
                self.add_log("Response timeout", "ERROR")
                self.command_in_progress = False
                self.timer_receiving.stop()
    
    def simulate_response(self, command_code):
        """Simulate a response from the pump for demonstration"""
        # Basic response structure: <Length><State><Parameter><Checksum>
        state_value = 0x41  # Basic success state
        response = []
        
        if command_code == 0x80:  # Price set
            response = [0x03, state_value]
        elif command_code == 0x04:  # Delete shift
            response = [0x03, state_value]
        elif command_code == 0x08:  # Fueling without preset
            response = [0x03, state_value]
            self.fuelling_status.setText("Active")
        elif command_code == 0x89:  # Preset amount fuelling
            response = [0x03, state_value]
            self.fuelling_status.setText("Active")
        elif command_code == 0x8B:  # Preset volume fuelling
            response = [0x03, state_value]
            self.fuelling_status.setText("Active")
        elif command_code == 0x8C:  # Read price
            price_value = int(self.price_spin.value() * 100)
            response = [
                0x08, 0x46,
                price_value & 0xFF, (price_value >> 8) & 0xFF, 
                (price_value >> 16) & 0xFF, (price_value >> 24) & 0xFF,
                0x02  # Decimal type
            ]
        elif command_code == 0x8D:  # Read shift accumulation
            response = [0x0F, 0x46] + [0x00] * 12  # Simplified response
        elif command_code == 0x8E:  # Read total accumulation
            response = [0x0F, 0x46] + [0x00] * 12  # Simplified response
        elif command_code == 0x8F:  # Read fuelled amount
            response = [
                0x0D, 0x40,
                0x00, 0x00, 0x00, 0x00,  # Amount
                0x00, 0x00, 0x00, 0x00,  # Fuelled amount
                0x08,  # Printer state
                0x20   # Self-test state
            ]
        elif command_code == 0x10:  # Stop
            response = [0x03, state_value]
            self.fuelling_status.setText("Idle")
        elif command_code == 0x15:  # Lock dispenser
            response = [0x03, state_value]
            self.auth_status.setText("Enabled")
        elif command_code == 0x14:  # Unlock dispenser
            response = [0x03, state_value]
            self.auth_status.setText("Disabled")
        elif command_code == 0x12:  # Read state
            response = [
                0x0A, state_value,
                0x16, 0x21, 0x00, 0x20,  # Serial number
                0x02,  # Meter amount
                0x01,  # First address
                0x02   # Second address
            ]
        elif command_code == 0x42:  # Suspend FP
            response = [0x04, 0x42, state_value]
        elif command_code == 0x43:  # Resume FP
            response = [0x04, 0x43, state_value]
        else:
            response = [0x03, state_value]
        
        # Calculate checksum
        checksum = self.calculate_checksum(response)
        response.append(checksum)
        
        # Log the simulated response
        hex_response = ' '.join([f"{b:02X}" for b in response])
        self.add_log(f"Received (simulated): {hex_response}", "RESPONSE")
        
        # Parse the response
        self.process_response(bytes(response))
    
    def process_response(self, data):
        """Process a response from the pump"""
        if len(data) < 3:  # Minimum length: Length + State + Checksum
            self.add_log("Response too short", "ERROR")
            return
            
        # Log the raw response
        hex_data = ' '.join([f"{b:02X}" for b in data])
        self.add_log(f"Received: {hex_data}", "RESPONSE")
            
        length = data[0]
        state = data[1]
        
        # Check if the response is complete
        if len(data) != length + 1:  # +1 because length doesn't include itself
            self.add_log(f"Incomplete response: expected {length + 1} bytes, got {len(data)}", "ERROR")
            return
            
        # Verify checksum
        calculated_checksum = self.calculate_checksum(data[:-1])
        received_checksum = data[-1]
        
        if calculated_checksum != received_checksum:
            self.add_log(f"Checksum error: calculated {calculated_checksum:02X}, received {received_checksum:02X}", "ERROR")
            return
            
        # Parse state byte
        if state & 0x08:  # BIT3: 1=Fuelling, 0=Idle
            self.fuelling_status.setText("Active")
        else:
            self.fuelling_status.setText("Idle")
            
        if state & 0x10:  # BIT4: 1=Authorization mode, 0=Freedom mode
            self.auth_status.setText("Enabled")
        else:
            self.auth_status.setText("Disabled")
            
        if state & 0x20:  # BIT5: 1=Handed nozzle waiting for authorization
            self.nozzle_status.setText("Yes")
        else:
            self.nozzle_status.setText("No")
            
        # Process specific response types based on length and content
        if length == 0x03 and state == 0x41:  # Basic success response
            self.add_log("Command executed successfully", "INFO")
        elif length == 0x08 and state == 0x46:  # Read price response
            if len(data) >= 7:
                price_bytes = data[2:6]
                price_value = price_bytes[0] | (price_bytes[1] << 8) | (price_bytes[2] << 16) | (price_bytes[3] << 24)
                decimal_type = data[6]
                
                if decimal_type == 0x02:  # 2 decimal places
                    price = price_value / 100.0
                else:
                    price = price_value
                    
                self.add_log(f"Price: {price:.2f}", "INFO")
        elif length == 0x0F and state == 0x46:  # Read accumulation response
            if len(data) >= 14:
                # Parse BCD volume and amount
                volume_bytes = data[2:8]
                amount_bytes = data[8:14]
                
                volume_str = ''.join([f"{b:02X}" for b in reversed(volume_bytes)])
                amount_str = ''.join([f"{b:02X}" for b in reversed(amount_bytes)])
                
                self.add_log(f"Volume: {volume_str}", "INFO")
                self.add_log(f"Amount: {amount_str}", "INFO")
        elif length == 0x0D and state == 0x40:  # Read fuelled amount response
            if len(data) >= 12:
                amount_bytes = data[2:6]
                fuelled_bytes = data[6:10]
                printer_state = data[10]
                self_test_state = data[11]
                
                amount = amount_bytes[0] | (amount_bytes[1] << 8) | (amount_bytes[2] << 16) | (amount_bytes[3] << 24)
                fuelled = fuelled_bytes[0] | (fuelled_bytes[1] << 8) | (fuelled_bytes[2] << 16) | (fuelled_bytes[3] << 24)
                
                self.add_log(f"Amount: {amount}", "INFO")
                self.add_log(f"Fuelled amount: {fuelled}", "INFO")
                
                # Parse printer state
                printer_connected = bool(printer_state & 0x20)
                out_of_paper = bool(printer_state & 0x40)
                printer_error = not bool(printer_state & 0x80)
                
                self.add_log(f"Printer connected: {printer_connected}", "INFO")
                self.add_log(f"Out of paper: {out_of_paper}", "INFO")
                self.add_log(f"Printer error: {printer_error}", "INFO")
                
                # Parse self-test state
                eeprom_error = bool(self_test_state & 0x01)
                solenoid_error = bool(self_test_state & 0x02)
                sensor_error = bool(self_test_state & 0x04)
                
                self.add_log(f"EEPROM error: {eeprom_error}", "INFO")
                self.add_log(f"Solenoid error: {solenoid_error}", "INFO")
                self.add_log(f"Sensor error: {sensor_error}", "INFO")
        elif length == 0x0A and state == 0x41:  # Read state response
            if len(data) >= 9:
                serial_bytes = data[2:6]
                meter_amount = data[6]
                first_address = data[7]
                second_address = data[8]
                
                serial_number = serial_bytes[0] | (serial_bytes[1] << 8) | (serial_bytes[2] << 16) | (serial_bytes[3] << 24)
                
                self.add_log(f"Serial number: {serial_number}", "INFO")
                self.add_log(f"Meter amount: {meter_amount}", "INFO")
                self.add_log(f"First address: {first_address}", "INFO")
                self.add_log(f"Second address: {second_address}", "INFO")
    
    # Command handlers
    def send_price_set(self):
        """Send price set command (0x80)"""
        price = int(self.price_spin.value() * 100)  # Convert to integer with 2 decimal places
        parameters = [
            price & 0xFF,
            (price >> 8) & 0xFF,
            (price >> 16) & 0xFF,
            (price >> 24) & 0xFF
        ]
        self.send_command(0x80, parameters)
    
    def send_delete_shift(self):
        """Send delete shift command (0x04)"""
        self.send_command(0x04)
    
    def send_fuelling_without_preset(self):
        """Send fuelling without preset command (0x08)"""
        self.send_command(0x08)
    
    def send_preset_amount_fuelling(self):
        """Send preset amount fuelling command (0x89)"""
        amount = int(self.amount_spin.value() * 100)  # Convert to integer with 2 decimal places
        parameters = [
            amount & 0xFF,
            (amount >> 8) & 0xFF,
            (amount >> 16) & 0xFF,
            (amount >> 24) & 0xFF
        ]
        self.send_command(0x89, parameters)
    
    def send_preset_volume_fuelling(self):
        """Send preset volume fuelling command (0x8B)"""
        volume = int(self.volume_spin.value() * 100)  # Convert to integer with 2 decimal places
        parameters = [
            volume & 0xFF,
            (volume >> 8) & 0xFF,
            (volume >> 16) & 0xFF,
            (volume >> 24) & 0xFF
        ]
        self.send_command(0x8B, parameters)
    
    def send_read_price(self):
        """Send read price command (0x8C)"""
        self.send_command(0x8C)
    
    def send_read_shift_accumulation(self):
        """Send read shift accumulation command (0x8D)"""
        self.send_command(0x8D)
    
    def send_read_total_accumulation(self):
        """Send read total accumulation command (0x8E)"""
        self.send_command(0x8E)
    
    def send_read_fuelled_amount(self):
        """Send read fuelled amount command (0x8F)"""
        self.send_command(0x8F)
    
    def send_stop(self):
        """Send stop command (0x10)"""
        self.send_command(0x10)
    
    def send_lock_dispenser(self):
        """Send lock dispenser command (0x15)"""
        self.send_command(0x15)
    
    def send_unlock_dispenser(self):
        """Send unlock dispenser command (0x14)"""
        self.send_command(0x14)
    
    def send_read_state(self):
        """Send read state command (0x12)"""#define RS485_TX 17
#define RS485_RX 16
#define RS485_DIR 5  // MAX485 DE/RE

#define BAUD_RATE 2400
#define RESPONSE_TIMEOUT 1000  // ms

void setup() {
  Serial.begin(115200);
  Serial2.begin(BAUD_RATE, SERIAL_8N1, RS485_RX, RS485_TX);
  pinMode(RS485_DIR, OUTPUT);
  digitalWrite(RS485_DIR, LOW);  // Start in receive mode

  delay(1000);
  Serial.println("Starting HONGYANG protocol test...");

  sendPriceSetCommand(0x01, 1234);  // Set price to 1234
}

void loop() {
  readResponse();  // Continuously look for response
  delay(5000);     // Slow polling
}

// ====== SEND COMMAND ======
void sendPriceSetCommand(uint8_t address, uint32_t price) {
  std::vector<uint8_t> frame;

  frame.push_back(address);         // Address
  frame.push_back(0x07);            // Length
  frame.push_back(0x80);            // Command

  frame.push_back(price & 0xFF);        // Little endian
  frame.push_back((price >> 8) & 0xFF);
  frame.push_back((price >> 16) & 0xFF);
  frame.push_back((price >> 24) & 0xFF);

  uint8_t checksum = calculateChecksum({frame.begin(), frame.end()});
  frame.push_back(checksum);

  Serial.println("Sending price set command...");
  sendFrame(frame);
}

void sendFrame(const std::vector<uint8_t>& data) {
  digitalWrite(RS485_DIR, HIGH);  // Transmit mode
  delayMicroseconds(100);

  for (uint8_t b : data) {
    Serial2.write(b);
    delayMicroseconds(500);
  }

  Serial2.flush();
  delayMicroseconds(100);
  digitalWrite(RS485_DIR, LOW);  // Receive mode
}

// ====== RECEIVE RESPONSE ======
void readResponse() {
  uint32_t startTime = millis();
  std::vector<uint8_t> buffer;

  while (millis() - startTime < RESPONSE_TIMEOUT) {
    if (Serial2.available()) {
      uint8_t b = Serial2.read();
      buffer.push_back(b);
    }
  }

  if (buffer.empty()) {
    Serial.println("No response received.");
    return;
  }

  Serial.println("Raw Response:");
  for (uint8_t b : buffer) {
    Serial.printf("%02X ", b);
  }
  Serial.println();

  if (!parseResponse(buffer)) {
    Serial.println("Invalid or corrupt response.");
  }
}

bool parseResponse(const std::vector<uint8_t>& data) {
  if (data.size() < 3) return false;

  uint8_t length = data[0];
  if (data.size() < length) return false;

  uint8_t state = data[1];
  std::vector<uint8_t> param(data.begin() + 2, data.begin() + data.size() - 1);
  uint8_t receivedChecksum = data.back();

  std::vector<uint8_t> checkRegion(data.begin(), data.end() - 1);
  uint8_t calculated = calculateChecksum(checkRegion);

  if (calculated != receivedChecksum) {
    Serial.printf("Checksum mismatch: expected %02X, got %02X\n", calculated, receivedChecksum);
    return false;
  }

  Serial.println("Valid response received.");
  Serial.printf("State Byte: 0x%02X\n", state);
  if (!param.empty()) {
    Serial.print("Parameters: ");
    for (uint8_t b : param) Serial.printf("%02X ", b);
    Serial.println();
  }

  return true;
}

// ====== CHECKSUM ======
uint8_t calculateChecksum(const std::vector<uint8_t>& data) {
  uint8_t sum = 0;
  for (uint8_t b : data) sum += b;
  return (~sum + 1);  // Two's complement
}
        self.send_command(0x12)
    
    def send_suspend_fp(self):
        """Send suspend FP command (0x42)"""
        self.send_command(0x42)
    
    def send_resume_fp(self):
        """Send resume FP command (0x43)"""
        self.send_command(0x43)
    
    def send_custom_command(self):
        """Send a custom command"""
        command_text = self.custom_command.text().strip()
        if not command_text:
            self.add_log("No command entered", "ERROR")
            return
            
        try:
            # Parse hex string into bytes
            command_bytes = bytearray.fromhex(command_text)
            if len(command_bytes) < 1:
                self.add_log("Command too short", "ERROR")
                return
                
            # Extract command code
            command_code = command_bytes[0]
            
            # Extract parameters if any
            parameters = list(command_bytes[1:]) if len(command_bytes) > 1 else []
            
            # Send the command
            self.send_command(command_code, parameters)
        except ValueError:
            self.add_log("Invalid hex format", "ERROR")
    
    def populate_command_table(self):
        """Populate the command reference table"""
        commands = [
            ("Price Set", "Set the price for the pump", "<Add> 07 80 [Price 4 bytes] <Checksum>", "<03> 41 <Checksum>"),
            ("Delete Shift", "Delete the current shift data", "<Add> 03 04 <Checksum>", "<03> 41 <Checksum>"),
            ("Fuelling Without Preset", "Start fuelling without limits", "<Add> 03 08 <Checksum>", "<03> 41 <Checksum>"),
            ("Preset Amount Fuelling", "Set a monetary limit for fuelling", "<Add> 07 89 [Amount 4 bytes] <Checksum>", "<03> 41 <Checksum>"),
            ("Preset Volume Fuelling", "Set a volume limit for fuelling", "<Add> 07 8B [Volume 4 bytes] <Checksum>", "<03> 41 <Checksum>"),
            ("Read Price", "Get the current price setting", "<Add> 03 8C <Checksum>", "<08> 46 [Price 4 bytes] [Decimal 1 byte] <Checksum>"),
            ("Read Shift Accumulation", "Get the current shift totals", "<Add> 03 8D <Checksum>", "<0F> 46 [Volume 6 bytes] [Amount 6 bytes] <Checksum>"),
            ("Read Total Accumulation", "Get the lifetime totals", "<Add> 03 8E <Checksum>", "<0F> 46 [Volume 6 bytes] [Amount 6 bytes] <Checksum>"),
            ("Read Fuelled Amount", "Get the current transaction data", "<Add> 03 8F <Checksum>", "<0D> 40 [Amount 4 bytes] [Fuelled 4 bytes] [Printer state] [Self-test state] <Checksum>"),
            ("Stop", "Stop the current fuelling operation", "<Add> 03 10 <Checksum>", "<03> 41 <Checksum>"),
            ("Lock Dispenser", "Set to authorization mode", "<Add> 03 15 <Checksum>", "<03> 41 <Checksum>"),
            ("Unlock Dispenser", "Set to free mode", "<Add> 03 14 <Checksum>", "<03> 41 <Checksum>"),
            ("Read State", "Get the current pump state", "<Add> 03 12 <Checksum>", "<0A> 41 [Serial 4 bytes] [Meter] [First addr] [Second addr] <Checksum>"),
            ("Suspend FP", "Enter the suspend state", "<Add> 03 42 <Checksum>", "<04> 42 [State] <Checksum>"),
            ("Resume FP", "Suspend recovery", "<Add> 03 43 <Checksum>", "<04> 43 [State] <Checksum>"),
        ]
        
        for command in commands:
            row = self.command_table.rowCount()
            self.command_table.insertRow(row)
            for col, value in enumerate(command):
                self.command_table.setItem(row, col, QTableWidgetItem(value))
    
    def closeEvent(self, event):
        """Handle window close event"""
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
        event.accept()

def main():
    app = QApplication(sys.argv)
    window = HongYangPumpControl()
    window.show()
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()