#!/usr/bin/env python3
"""
Protocol Analyzer UI - PyQt interface for ESP32 Protocol Analyzer
This application provides a graphical interface to control and monitor
the ESP32-based protocol analyzer (protocol-analyzer.ino).
"""

import sys
import time
import serial
import serial.tools.list_ports
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
                            QLabel, QPushButton, QComboBox, QTextEdit, QGroupBox, 
                            QGridLayout, QLineEdit, QTabWidget, QSpinBox, QMessageBox,
                            QSplitter, QCheckBox, QFileDialog)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QTimer
from PyQt5.QtGui import QFont, QTextCursor, QColor

class SerialThread(QThread):
    """Thread for handling serial communication without blocking the UI"""
    data_received = pyqtSignal(str)
    connection_error = pyqtSignal(str)

    def __init__(self, port, baudrate):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.running = False
        self.serial_port = None

    def run(self):
        try:
            self.serial_port = serial.Serial(self.port, self.baudrate, timeout=0.1)
            self.running = True
            
            while self.running:
                if self.serial_port.in_waiting:
                    data = self.serial_port.readline().decode('utf-8', errors='replace')
                    if data:
                        self.data_received.emit(data)
                time.sleep(0.01)  # Small delay to prevent CPU hogging
                
        except serial.SerialException as e:
            self.connection_error.emit(f"Serial Error: {str(e)}")
            self.running = False
        except Exception as e:
            self.connection_error.emit(f"Error: {str(e)}")
            self.running = False
        finally:
            if self.serial_port and self.serial_port.is_open:
                self.serial_port.close()

    def send_command(self, command):
        if self.serial_port and self.serial_port.is_open:
            try:
                self.serial_port.write((command + '\n').encode('utf-8'))
                return True
            except Exception as e:
                self.connection_error.emit(f"Send Error: {str(e)}")
                return False
        return False

    def stop(self):
        self.running = False
        self.wait()


class ProtocolAnalyzerUI(QMainWindow):
    """Main window for the Protocol Analyzer UI"""
    
    def __init__(self):
        super().__init__()
        self.serial_thread = None
        self.log_file = None
        self.logging_enabled = False
        self.init_ui()
        
    def init_ui(self):
        self.setWindowTitle("Protocol Analyzer UI")
        self.setGeometry(100, 100, 1000, 700)
        
        # Main layout
        main_widget = QWidget()
        main_layout = QVBoxLayout()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)
        
        # Connection settings
        connection_group = QGroupBox("Connection Settings")
        connection_layout = QHBoxLayout()
        
        # Port selection
        self.port_combo = QComboBox()
        self.refresh_ports()
        connection_layout.addWidget(QLabel("Port:"))
        connection_layout.addWidget(self.port_combo)
        
        # Baud rate selection
        self.baud_combo = QComboBox()
        self.baud_combo.addItems(["9600", "19200", "38400", "57600", "115200"])
        self.baud_combo.setCurrentText("115200")  # Default for ESP32 monitor
        connection_layout.addWidget(QLabel("Baud Rate:"))
        connection_layout.addWidget(self.baud_combo)
        
        # Connect/Disconnect button
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        connection_layout.addWidget(self.connect_button)
        
        # Refresh ports button
        refresh_button = QPushButton("Refresh Ports")
        refresh_button.clicked.connect(self.refresh_ports)
        connection_layout.addWidget(refresh_button)
        
        # Clear log button
        clear_button = QPushButton("Clear Log")
        clear_button.clicked.connect(self.clear_log)
        connection_layout.addWidget(clear_button)
        
        # Log to file checkbox
        self.log_checkbox = QCheckBox("Log to File")
        self.log_checkbox.toggled.connect(self.toggle_logging)
        connection_layout.addWidget(self.log_checkbox)
        
        connection_group.setLayout(connection_layout)
        main_layout.addWidget(connection_group)
        
        # Create tab widget for different functions
        self.tab_widget = QTabWidget()
        
        # Add tabs
        self.tab_widget.addTab(self.create_monitor_tab(), "Monitor")
        self.tab_widget.addTab(self.create_rs232_tab(), "RS232 Tests")
        self.tab_widget.addTab(self.create_rs485_tab(), "RS485/Modbus")
        self.tab_widget.addTab(self.create_u913_tab(), "U913 Device")
        
        main_layout.addWidget(self.tab_widget)
        
        # Log display
        log_group = QGroupBox("Communication Log")
        log_layout = QVBoxLayout()
        
        self.log_display = QTextEdit()
        self.log_display.setReadOnly(True)
        self.log_display.setFont(QFont("Courier New", 10))
        log_layout.addWidget(self.log_display)
        
        # Command line
        cmd_layout = QHBoxLayout()
        self.cmd_input = QLineEdit()
        self.cmd_input.setPlaceholderText("Enter command...")
        self.cmd_input.returnPressed.connect(self.send_manual_command)
        cmd_layout.addWidget(self.cmd_input)
        
        send_button = QPushButton("Send")
        send_button.clicked.connect(self.send_manual_command)
        cmd_layout.addWidget(send_button)
        
        log_layout.addLayout(cmd_layout)
        log_group.setLayout(log_layout)
        main_layout.addWidget(log_group)
        
        # Status bar
        self.statusBar().showMessage("Disconnected")
        
        # Set up auto-scroll for log
        self.log_timer = QTimer()
        self.log_timer.timeout.connect(self.auto_scroll_log)
        self.log_timer.start(100)  # Check every 100ms
        
    def create_monitor_tab(self):
        """Create the monitoring tab"""
        tab = QWidget()
        layout = QVBoxLayout()
        
        # Monitoring controls
        control_group = QGroupBox("Monitoring Controls")
        control_layout = QGridLayout()
        
        # Start/Stop monitoring
        start_button = QPushButton("Start Monitoring")
        start_button.clicked.connect(lambda: self.send_command("START"))
        control_layout.addWidget(start_button, 0, 0)
        
        stop_button = QPushButton("Stop Monitoring")
        stop_button.clicked.connect(lambda: self.send_command("STOP"))
        control_layout.addWidget(stop_button, 0, 1)
        
        # Parity controls
        mark_button = QPushButton("MARK Parity")
        mark_button.clicked.connect(lambda: self.send_command("MARK"))
        control_layout.addWidget(mark_button, 1, 0)
        
        space_button = QPushButton("SPACE Parity")
        space_button.clicked.connect(lambda: self.send_command("SPACE"))
        control_layout.addWidget(space_button, 1, 1)
        
        # Channel selection
        channel_group = QGroupBox("Multiplexer Channel")
        channel_layout = QHBoxLayout()
        
        for i in range(4):
            channel_btn = QPushButton(f"Channel {i}")
            channel_btn.clicked.connect(lambda checked, ch=i: self.send_command(f"CHANNEL {ch}"))
            channel_layout.addWidget(channel_btn)
        
        channel_group.setLayout(channel_layout)
        control_layout.addWidget(channel_group, 2, 0, 1, 2)
        
        # Send hex data
        hex_group = QGroupBox("Send Hex Data")
        hex_layout = QHBoxLayout()
        
        self.hex_input = QLineEdit()
        self.hex_input.setPlaceholderText("Enter hex bytes (e.g., 01 03 8F 6D)")
        hex_layout.addWidget(self.hex_input)
        
        hex_send_button = QPushButton("Send")
        hex_send_button.clicked.connect(self.send_hex_data)
        hex_layout.addWidget(hex_send_button)
        
        hex_group.setLayout(hex_layout)
        control_layout.addWidget(hex_group, 3, 0, 1, 2)
        
        # Status commands
        status_button = QPushButton("Show Status")
        status_button.clicked.connect(lambda: self.send_command("STATUS"))
        control_layout.addWidget(status_button, 4, 0)
        
        logs_button = QPushButton("Show All Logs")
        logs_button.clicked.connect(lambda: self.send_command("LOGS"))
        control_layout.addWidget(logs_button, 4, 1)
        
        # Help button
        help_button = QPushButton("Show Help")
        help_button.clicked.connect(lambda: self.send_command("HELP"))
        control_layout.addWidget(help_button, 5, 0, 1, 2)
        
        control_group.setLayout(control_layout)
        layout.addWidget(control_group)
        
        # Add help section
        help_group = QGroupBox("Quick Help")
        help_layout = QVBoxLayout()
        
        help_text = QTextEdit()
        help_text.setReadOnly(True)
        help_text.setHtml("""
        <h3>Protocol Analyzer Commands</h3>
        <ul>
            <li><b>START</b> - Start monitoring serial communication</li>
            <li><b>STOP</b> - Stop monitoring</li>
            <li><b>MARK</b> - Switch to MARK parity (8O1)</li>
            <li><b>SPACE</b> - Switch to SPACE parity (8E1)</li>
            <li><b>CHANNEL &lt;0-3&gt;</b> - Select multiplexer channel</li>
            <li><b>SEND &lt;hex data&gt;</b> - Send hex bytes</li>
            <li><b>STATUS</b> - Show current status</li>
            <li><b>LOGS</b> - Show all stored logs</li>
            <li><b>CLEAR</b> - Clear logs</li>
            <li><b>HELP</b> - Show help message</li>
        </ul>
        """)
        help_layout.addWidget(help_text)
        
        help_group.setLayout(help_layout)
        layout.addWidget(help_group)
        
        tab.setLayout(layout)
        return tab
    
    def create_rs232_tab(self):
        """Create the RS232 testing tab"""
        tab = QWidget()
        layout = QVBoxLayout()
        
        # RS232 Test controls
        test_group = QGroupBox("RS232 Tests")
        test_layout = QGridLayout()
        
        # Standard RS232 test
        rs232_test_button = QPushButton("Test RS232 Connection")
        rs232_test_button.clicked.connect(lambda: self.send_command("TEST"))
        test_layout.addWidget(rs232_test_button, 0, 0, 1, 2)
        
        # Loopback test
        loopback_button = QPushButton("RS232 Loopback Test")
        loopback_button.clicked.connect(lambda: self.send_command("LOOPBACK"))
        test_layout.addWidget(loopback_button, 1, 0, 1, 2)
        
        # Channel selection for tests
        channel_label = QLabel("Select channel before test:")
        test_layout.addWidget(channel_label, 2, 0)
        
        channel_combo = QComboBox()
        channel_combo.addItems(["Channel 0", "Channel 1", "Channel 2", "Channel 3"])
        test_layout.addWidget(channel_combo, 2, 1)
        
        select_channel_button = QPushButton("Select Channel")
        select_channel_button.clicked.connect(
            lambda: self.send_command(f"CHANNEL {channel_combo.currentIndex()}")
        )
        test_layout.addWidget(select_channel_button, 3, 0, 1, 2)
        
        test_group.setLayout(test_layout)
        layout.addWidget(test_group)
        
        # Custom command group
        custom_group = QGroupBox("Custom RS232 Command")
        custom_layout = QVBoxLayout()
        
        # Explanation
        explanation = QLabel("Create a custom command sequence with proper parity switching:")
        custom_layout.addWidget(explanation)
        
        # Address byte (MARK parity)
        addr_layout = QHBoxLayout()
        addr_layout.addWidget(QLabel("Address Byte (MARK parity):"))
        self.addr_input = QLineEdit()
        self.addr_input.setPlaceholderText("01")
        self.addr_input.setMaxLength(2)
        addr_layout.addWidget(self.addr_input)
        custom_layout.addLayout(addr_layout)
        
        # Command bytes (SPACE parity)
        cmd_layout = QHBoxLayout()
        cmd_layout.addWidget(QLabel("Command Bytes (SPACE parity):"))
        self.cmd_bytes_input = QLineEdit()
        self.cmd_bytes_input.setPlaceholderText("03 8F 6D")
        cmd_layout.addWidget(self.cmd_bytes_input)
        custom_layout.addLayout(cmd_layout)
        
        # Send custom command
        custom_send_button = QPushButton("Send Custom Command")
        custom_send_button.clicked.connect(self.send_custom_rs232_command)
        custom_layout.addWidget(custom_send_button)
        
        custom_group.setLayout(custom_layout)
        layout.addWidget(custom_group)
        
        # U913 Test
        u913_group = QGroupBox("U913 Test")
        u913_layout = QHBoxLayout()
        
        u913_button = QPushButton("Test U913 Device")
        u913_button.clicked.connect(lambda: self.send_command("U913"))
        u913_layout.addWidget(u913_button)
        
        u913_channel_combo = QComboBox()
        u913_channel_combo.addItems(["Channel 0", "Channel 1", "Channel 2", "Channel 3"])
        u913_layout.addWidget(u913_channel_combo)
        
        u913_channel_button = QPushButton("Test U913 on Channel")
        u913_channel_button.clicked.connect(
            lambda: self.send_command(f"U913 {u913_channel_combo.currentIndex()}")
        )
        u913_layout.addWidget(u913_channel_button)
        
        u913_group.setLayout(u913_layout)
        layout.addWidget(u913_group)
        
        tab.setLayout(layout)
        return tab
    
    def create_rs485_tab(self):
        """Create the RS485/Modbus testing tab"""
        tab = QWidget()
        layout = QVBoxLayout()
        
        # Simple test
        simple_group = QGroupBox("Simple RS485 Modbus Test")
        simple_layout = QHBoxLayout()
        
        simple_test_button = QPushButton("Run Simple RS485 Test")
        simple_test_button.clicked.connect(lambda: self.send_command("RS485"))
        simple_layout.addWidget(simple_test_button)
        
        simple_group.setLayout(simple_layout)
        layout.addWidget(simple_group)
        
        # Modbus read/write
        modbus_group = QGroupBox("Modbus Read/Write")
        modbus_layout = QGridLayout()
        
        # Slave ID
        modbus_layout.addWidget(QLabel("Slave ID:"), 0, 0)
        self.slave_id_input = QSpinBox()
        self.slave_id_input.setRange(1, 247)
        self.slave_id_input.setValue(1)
        modbus_layout.addWidget(self.slave_id_input, 0, 1)
        
        # Register
        modbus_layout.addWidget(QLabel("Register:"), 1, 0)
        self.register_input = QLineEdit()
        self.register_input.setPlaceholderText("40001")
        modbus_layout.addWidget(self.register_input, 1, 1)
        
        # Register type help
        register_help = QLabel("Register types: 1-9999 (Coils), 10001-19999 (Discrete Inputs),\n30001-39999 (Input Registers), 40001-49999 (Holding Registers)")
        register_help.setWordWrap(True)
        modbus_layout.addWidget(register_help, 2, 0, 1, 2)
        
        # Value for write
        modbus_layout.addWidget(QLabel("Value (for write):"), 3, 0)
        self.value_input = QSpinBox()
        self.value_input.setRange(0, 65535)
        self.value_input.setValue(0)
        modbus_layout.addWidget(self.value_input, 3, 1)
        
        # Read/Write buttons
        read_button = QPushButton("Read Register")
        read_button.clicked.connect(self.read_modbus_register)
        modbus_layout.addWidget(read_button, 4, 0)
        
        write_button = QPushButton("Write Register")
        write_button.clicked.connect(self.write_modbus_register)
        modbus_layout.addWidget(write_button, 4, 1)
        
        modbus_group.setLayout(modbus_layout)
        layout.addWidget(modbus_group)
        
        # Channel selection
        channel_group = QGroupBox("Multiplexer Channel")
        channel_layout = QHBoxLayout()
        
        for i in range(4):
            channel_btn = QPushButton(f"Channel {i}")
            channel_btn.clicked.connect(lambda checked, ch=i: self.send_command(f"CHANNEL {ch}"))
            channel_layout.addWidget(channel_btn)
        
        channel_group.setLayout(channel_layout)
        layout.addWidget(channel_group)
        
        # Help section
        help_group = QGroupBox("Modbus Information")
        help_layout = QVBoxLayout()
        
        help_text = QTextEdit()
        help_text.setReadOnly(True)
        help_text.setHtml("""
        <h3>Modbus RTU over RS485</h3>
        <p><b>Simple Test:</b> Runs a basic test that attempts to read register 40001 from slave ID 1.</p>
        <p><b>Read Register:</b> Reads a specific register from the specified slave device.</p>
        <p><b>Write Register:</b> Writes a value to a specific register on the specified slave device.</p>
        <p><b>Register Types:</b></p>
        <ul>
            <li><b>1-9999:</b> Coils (read/write)</li>
            <li><b>10001-19999:</b> Discrete Inputs (read-only)</li>
            <li><b>30001-39999:</b> Input Registers (read-only)</li>
            <li><b>40001-49999:</b> Holding Registers (read/write)</li>
        </ul>
        <p><b>Note:</b> The ESP32 uses a standard baud rate of 9600 for Modbus communication.</p>
        """)
        help_layout.addWidget(help_text)
        
        help_group.setLayout(help_layout)
        layout.addWidget(help_group)
        
        tab.setLayout(layout)
        return tab
    
    def create_u913_tab(self):
        """Create the U913 device testing tab"""
        tab = QWidget()
        layout = QVBoxLayout()
        
        # U913 Test controls
        u913_group = QGroupBox("U913 Device Test")
        u913_layout = QGridLayout()
        
        # Test button
        u913_test_button = QPushButton("Test U913 Device")
        u913_test_button.clicked.connect(lambda: self.send_command("U913"))
        u913_test_button.setMinimumHeight(50)
        u913_layout.addWidget(u913_test_button, 0, 0, 1, 2)
        
        # Channel selection
        u913_layout.addWidget(QLabel("Select Channel:"), 1, 0)
        self.u913_channel_combo = QComboBox()
        self.u913_channel_combo.addItems(["Channel 0", "Channel 1", "Channel 2", "Channel 3"])
        u913_layout.addWidget(self.u913_channel_combo, 1, 1)
        
        # Test with channel button
        u913_channel_button = QPushButton("Test U913 on Selected Channel")
        u913_channel_button.clicked.connect(self.test_u913_on_channel)
        u913_layout.addWidget(u913_channel_button, 2, 0, 1, 2)
        
        u913_group.setLayout(u913_layout)
        layout.addWidget(u913_group)
        
        # U913 Commands
        commands_group = QGroupBox("U913 Commands")
        commands_layout = QGridLayout()
        
        # Price change
        price_button = QPushButton("Price Change")
        price_button.clicked.connect(self.send_price_change)
        commands_layout.addWidget(price_button, 0, 0)
        
        self.price_input = QLineEdit()
        self.price_input.setPlaceholderText("Price (e.g., 1234)")
        commands_layout.addWidget(self.price_input, 0, 1)
        
        # Read shift total
        read_shift_button = QPushButton("Read Shift Total")
        read_shift_button.clicked.connect(self.send_read_shift_total)
        commands_layout.addWidget(read_shift_button, 1, 0, 1, 2)
        
        # Clear shift total
        clear_shift_button = QPushButton("Clear Shift Total")
        clear_shift_button.clicked.connect(self.send_clear_shift_total)
        commands_layout.addWidget(clear_shift_button, 2, 0, 1, 2)
        
        # Stop pump
        stop_pump_button = QPushButton("Stop Pump")
        stop_pump_button.clicked.connect(self.send_stop_pump)
        commands_layout.addWidget(stop_pump_button, 3, 0, 1, 2)
        
        # Request real-time data
        realtime_button = QPushButton("Request Real-time Data")
        realtime_button.clicked.connect(self.send_realtime_request)
        commands_layout.addWidget(realtime_button, 4, 0, 1, 2)
        
        commands_group.setLayout(commands_layout)
        layout.addWidget(commands_group)
        
        # Help section
        help_group = QGroupBox("U913 Information")
        help_layout = QVBoxLayout()
        
        help_text = QTextEdit()
        help_text.setReadOnly(True)
        help_text.setHtml("""
        <h3>U913 Fuel Dispenser Device</h3>
        <p>The U913 test sends a standard command to the fuel dispenser and analyzes the response.</p>
        <p><b>Communication Protocol:</b></p>
        <ol>
            <li>Address byte sent with MARK parity (8O1)</li>
            <li>Command bytes sent with SPACE parity (8E1)</li>
        </ol>
        <p><b>Available Commands:</b></p>
        <ul>
            <li><b>Price Change:</b> Updates the fuel price</li>
            <li><b>Read Shift Total:</b> Retrieves volume, amount, and transaction count</li>
            <li><b>Clear Shift Total:</b> Resets the shift totals</li>
            <li><b>Stop Pump:</b> Immediately stops the pump</li>
            <li><b>Request Real-time Data:</b> Gets current volume, amount, and status</li>
        </ul>
        <p><b>Note:</b> Always select the correct multiplexer channel before running tests.</p>
        """)
        help_layout.addWidget(help_text)
        
        help_group.setLayout(help_layout)
        layout.addWidget(help_group)
        
        tab.setLayout(layout)
        return tab
    
    def refresh_ports(self):
        """Refresh the list of available serial ports"""
        self.port_combo.clear()
        ports = [port.device for port in serial.tools.list_ports.comports()]
        if ports:
            self.port_combo.addItems(ports)
        else:
            self.port_combo.addItem("No ports available")
    
    def toggle_connection(self):
        """Connect to or disconnect from the serial port"""
        if self.serial_thread and self.serial_thread.running:
            # Disconnect
            self.serial_thread.stop()
            self.serial_thread = None
            self.connect_button.setText("Connect")
            self.statusBar().showMessage("Disconnected")
            self.log_message("Disconnected from serial port", "red")
        else:
            # Connect
            port = self.port_combo.currentText()
            baudrate = int(self.baud_combo.currentText())
            
            if port == "No ports available":
                QMessageBox.warning(self, "Connection Error", "No serial ports available")
                return
            
            self.serial_thread = SerialThread(port, baudrate)
            self.serial_thread.data_received.connect(self.handle_received_data)
            self.serial_thread.connection_error.connect(self.handle_connection_error)
            self.serial_thread.start()
            
            self.connect_button.setText("Disconnect")
            self.statusBar().showMessage(f"Connected to {port} at {baudrate} baud")
            self.log_message(f"Connected to {port} at {baudrate} baud", "green")
    
    def handle_received_data(self, data):
        """Handle data received from the serial port"""
        self.log_display.insertPlainText(data)
        
        # Write to log file if enabled
        if self.logging_enabled and self.log_file:
            try:
                self.log_file.write(data)
                self.log_file.flush()
            except Exception as e:
                self.log_message(f"Error writing to log file: {str(e)}", "red")
                self.toggle_logging(False)
                self.log_checkbox.setChecked(False)
    
    def handle_connection_error(self, error_msg):
        """Handle connection errors"""
        self.log_message(error_msg, "red")
        self.connect_button.setText("Connect")
        self.statusBar().showMessage("Connection Error")
        QMessageBox.critical(self, "Connection Error", error_msg)
    
    def send_command(self, command):
        """Send a command to the device"""
        if not self.serial_thread or not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return False
        
        success = self.serial_thread.send_command(command)
        if success:
            self.log_message(f"Sent: {command}", "blue")
        return success
    
    def send_manual_command(self):
        """Send a command from the command line input"""
        command = self.cmd_input.text().strip()
        if command:
            if self.send_command(command):
                self.cmd_input.clear()
    
    def send_hex_data(self):
        """Send hex data to the device"""
        hex_data = self.hex_input.text().strip()
        if hex_data:
            self.send_command(f"SEND {hex_data}")
    
    def send_custom_rs232_command(self):
        """Send a custom RS232 command with proper parity switching"""
        addr = self.addr_input.text().strip()
        cmd_bytes = self.cmd_bytes_input.text().strip()
        
        if not addr:
            QMessageBox.warning(self, "Input Error", "Please enter an address byte")
            return
        
        if not cmd_bytes:
            QMessageBox.warning(self, "Input Error", "Please enter command bytes")
            return
        
        # First switch to MARK parity and send address
        self.send_command("MARK")
        time.sleep(0.2)  # Small delay to ensure command is processed
        self.send_command(f"SEND {addr}")
        time.sleep(0.2)  # Small delay
        
        # Then switch to SPACE parity and send command
        self.send_command("SPACE")
        time.sleep(0.2)  # Small delay
        self.send_command(f"SEND {cmd_bytes}")
    
    def read_modbus_register(self):
        """Read a Modbus register"""
        slave_id = self.slave_id_input.value()
        register = self.register_input.text().strip()
        
        if not register:
            QMessageBox.warning(self, "Input Error", "Please enter a register address")
            return
        
        self.send_command(f"RS485READ {slave_id} {register}")
    
    def write_modbus_register(self):
        """Write to a Modbus register"""
        slave_id = self.slave_id_input.value()
        register = self.register_input.text().strip()
        value = self.value_input.value()
        
        if not register:
            QMessageBox.warning(self, "Input Error", "Please enter a register address")
            return
        
        self.send_command(f"RS485WRITE {slave_id} {register} {value}")
    
    def test_u913_on_channel(self):
        """Test U913 on the selected channel"""
        channel = self.u913_channel_combo.currentIndex()
        self.send_command(f"U913 {channel}")
    
    def send_price_change(self):
        """Send a price change command to U913"""
        price = self.price_input.text().strip()
        if not price:
            QMessageBox.warning(self, "Input Error", "Please enter a price value")
            return
        
        # First select the channel
        channel = self.u913_channel_combo.currentIndex()
        self.send_command(f"CHANNEL {channel}")
        time.sleep(0.2)  # Small delay
        
        # First switch to MARK parity and send address
        self.send_command("MARK")
        time.sleep(0.2)
        self.send_command("SEND 01")  # Default address
        time.sleep(0.2)
        
        # Then switch to SPACE parity and send command
        self.send_command("SPACE")
        time.sleep(0.2)
        
        # Format: Length (07), Command code (80), Price data, Checksum
        # Note: This is a simplified command, actual checksum calculation would be done on the ESP32
        self.send_command(f"SEND 07 80 00 00 {price[:2]} {price[2:]} FF")
    
    def send_read_shift_total(self):
        """Send read shift total command to U913"""
        # First select the channel
        channel = self.u913_channel_combo.currentIndex()
        self.send_command(f"CHANNEL {channel}")
        time.sleep(0.2)  # Small delay
        
        # First switch to MARK parity and send address
        self.send_command("MARK")
        time.sleep(0.2)
        self.send_command("SEND 01")  # Default address
        time.sleep(0.2)
        
        # Then switch to SPACE parity and send command
        self.send_command("SPACE")
        time.sleep(0.2)
        self.send_command("SEND 03 8D 6F")  # Read shift total command
    
    def send_clear_shift_total(self):
        """Send clear shift total command to U913"""
        # First select the channel
        channel = self.u913_channel_combo.currentIndex()
        self.send_command(f"CHANNEL {channel}")
        time.sleep(0.2)  # Small delay
        
        # First switch to MARK parity and send address
        self.send_command("MARK")
        time.sleep(0.2)
        self.send_command("SEND 01")  # Default address
        time.sleep(0.2)
        
        # Then switch to SPACE parity and send command
        self.send_command("SPACE")
        time.sleep(0.2)
        self.send_command("SEND 03 04 F8")  # Clear shift total command
    
    def send_stop_pump(self):
        """Send stop pump command to U913"""
        # First select the channel
        channel = self.u913_channel_combo.currentIndex()
        self.send_command(f"CHANNEL {channel}")
        time.sleep(0.2)  # Small delay
        
        # First switch to MARK parity and send address
        self.send_command("MARK")
        time.sleep(0.2)
        self.send_command("SEND 01")  # Default address
        time.sleep(0.2)
        
        # Then switch to SPACE parity and send command
        self.send_command("SPACE")
        time.sleep(0.2)
        self.send_command("SEND 03 10 EC")  # Stop pump command
    
    def send_realtime_request(self):
        """Send request real-time data command to U913"""
        # First select the channel
        channel = self.u913_channel_combo.currentIndex()
        self.send_command(f"CHANNEL {channel}")
        time.sleep(0.2)  # Small delay
        
        # First switch to MARK parity and send address
        self.send_command("MARK")
        time.sleep(0.2)
        self.send_command("SEND 01")  # Default address
        time.sleep(0.2)
        
        # Then switch to SPACE parity and send command
        self.send_command("SPACE")
        time.sleep(0.2)
        self.send_command("SEND 03 8F 6D")  # Request real-time data command
    
    def toggle_logging(self, enabled):
        """Toggle logging to file"""
        self.logging_enabled = enabled
        
        if enabled:
            try:
                # Open file dialog to select log file
                file_path, _ = QFileDialog.getSaveFileName(
                    self, "Save Log File", "", "Log Files (*.log);;All Files (*)"
                )
                
                if file_path:
                    self.log_file = open(file_path, 'a')
                    self.log_message(f"Logging to file: {file_path}", "green")
                else:
                    self.logging_enabled = False
                    self.log_checkbox.setChecked(False)
            except Exception as e:
                self.log_message(f"Error opening log file: {str(e)}", "red")
                self.logging_enabled = False
                self.log_checkbox.setChecked(False)
        else:
            if self.log_file:
                try:
                    self.log_file.close()
                    self.log_message("Logging to file stopped", "blue")
                except Exception:
                    pass
                self.log_file = None
    
    def clear_log(self):
        """Clear the log display"""
        self.log_display.clear()
        # Also send CLEAR command to the device
        self.send_command("CLEAR")
    
    def log_message(self, message, color="black"):
        """Add a message to the log with the specified color"""
        self.log_display.setTextColor(QColor(color))
        self.log_display.append(message)
        self.log_display.setTextColor(QColor("black"))
        
        # Write to log file if enabled
        if self.logging_enabled and self.log_file:
            try:
                timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
                self.log_file.write(f"[{timestamp}] {message}\n")
                self.log_file.flush()
            except Exception:
                pass
    
    def auto_scroll_log(self):
        """Auto-scroll the log to the bottom"""
        if self.log_display.verticalScrollBar().value() == self.log_display.verticalScrollBar().maximum():
            # Only auto-scroll if already at the bottom
            cursor = self.log_display.textCursor()
            cursor.movePosition(QTextCursor.End)
            self.log_display.setTextCursor(cursor)


if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setStyle('Fusion')  # Use Fusion style for a consistent look
    
    window = ProtocolAnalyzerUI()
    window.show()
    sys.exit(app.exec_())
