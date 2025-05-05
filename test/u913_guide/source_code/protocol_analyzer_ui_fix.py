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
import csv
from datetime import datetime
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
                            QLabel, QPushButton, QComboBox, QTextEdit, QGroupBox, 
                            QGridLayout, QLineEdit, QTabWidget, QSpinBox, QMessageBox,
                            QSplitter, QCheckBox, QFileDialog, QProgressBar, QTableWidget,
                            QTableWidgetItem, QHeaderView, QFrame, QRadioButton, QButtonGroup)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QTimer
from PyQt5.QtGui import QFont, QTextCursor, QColor, QPalette

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
        self.test_results = []  # Store test results
        self.init_ui()
        
    def init_ui(self):
        self.setWindowTitle("Protocol Analyzer UI - Industrial Test System")
        self.setGeometry(100, 100, 1200, 800)
        
        # Main layout with splitter for resizable sections
        main_widget = QWidget()
        main_layout = QVBoxLayout()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)
        
        # Top section: Connection settings and test status
        top_section = QHBoxLayout()
        
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
        self.connect_button.setMinimumHeight(40)
        connection_layout.addWidget(self.connect_button)
        
        # Refresh ports button
        refresh_button = QPushButton("Refresh Ports")
        refresh_button.clicked.connect(self.refresh_ports)
        connection_layout.addWidget(refresh_button)
        
        connection_group.setLayout(connection_layout)
        top_section.addWidget(connection_group, 3)
        
        # Test status indicator
        self.status_group = QGroupBox("Test Status")
        status_layout = QVBoxLayout()
        
        self.status_label = QLabel("NOT TESTED")
        self.status_label.setAlignment(Qt.AlignCenter)
        self.status_label.setStyleSheet("font-size: 24pt; font-weight: bold; color: gray;")
        self.status_label.setMinimumHeight(60)
        status_layout.addWidget(self.status_label)
        
        self.status_group.setLayout(status_layout)
        top_section.addWidget(self.status_group, 2)
        
        main_layout.addLayout(top_section)
        
        # Protocol status indicators
        protocol_status = QHBoxLayout()
        
        # RS232 Status
        self.rs232_status = self.create_protocol_status("RS232")
        protocol_status.addWidget(self.rs232_status)
        
        # RS485 Status
        self.rs485_status = self.create_protocol_status("RS485")
        protocol_status.addWidget(self.rs485_status)
        
        # U913 Status
        self.u913_status = self.create_protocol_status("U913")
        protocol_status.addWidget(self.u913_status)
        
        main_layout.addLayout(protocol_status)
        
        # Middle section: Splitter with tabs and large RX display
        middle_splitter = QSplitter(Qt.Horizontal)
        
        # Left side: Control tabs
        self.tab_widget = QTabWidget()
        self.tab_widget.addTab(self.create_monitor_tab(), "Monitor")
        self.tab_widget.addTab(self.create_rs232_tab(), "RS232 Tests")
        self.tab_widget.addTab(self.create_rs485_tab(), "RS485/Modbus")
        self.tab_widget.addTab(self.create_u913_tab(), "U913 Device")
        self.tab_widget.addTab(self.create_test_suite_tab(), "Test Suite")
        middle_splitter.addWidget(self.tab_widget)
        
        # Right side: Large RX display
        rx_widget = QWidget()
        rx_layout = QVBoxLayout()
        rx_widget.setLayout(rx_layout)
        
        # RX display header with controls
        rx_header = QHBoxLayout()
        rx_header.addWidget(QLabel("<b>RECEIVE DATA</b>"))
        
        clear_button = QPushButton("Clear Log")
        clear_button.clicked.connect(self.clear_log)
        rx_header.addWidget(clear_button)
        
        self.auto_scroll_check = QCheckBox("Auto-scroll")
        self.auto_scroll_check.setChecked(True)
        rx_header.addWidget(self.auto_scroll_check)
        
        self.log_checkbox = QCheckBox("Log to File")
        self.log_checkbox.toggled.connect(self.toggle_logging)
        rx_header.addWidget(self.log_checkbox)
        
        # Export results button
        export_button = QPushButton("Export Results")
        export_button.clicked.connect(self.export_test_results)
        rx_header.addWidget(export_button)
        
        rx_layout.addLayout(rx_header)
        
        # TX/RX Activity Indicators
        activity_layout = QHBoxLayout()
        
        # TX indicator
        tx_frame = QFrame()
        tx_frame.setFrameShape(QFrame.Box)
        tx_frame.setFixedSize(20, 20)
        tx_frame.setStyleSheet("background-color: gray;")
        self.tx_indicator = tx_frame
        
        activity_layout.addWidget(QLabel("TX:"))
        activity_layout.addWidget(self.tx_indicator)
        
        # RX indicator
        rx_frame = QFrame()
        rx_frame.setFrameShape(QFrame.Box)
        rx_frame.setFixedSize(20, 20)
        rx_frame.setStyleSheet("background-color: gray;")
        self.rx_indicator = rx_frame
        
        activity_layout.addWidget(QLabel("RX:"))
        activity_layout.addWidget(self.rx_indicator)
        
        activity_layout.addStretch()
        rx_layout.addLayout(activity_layout)
        
        # Large RX display
        self.log_display = QTextEdit()
        self.log_display.setReadOnly(True)
        self.log_display.setFont(QFont("Courier New", 12))
        self.log_display.setMinimumWidth(600)
        rx_layout.addWidget(self.log_display)
        
        # Command line
        cmd_layout = QHBoxLayout()
        cmd_layout.addWidget(QLabel("Command:"))
        self.cmd_input = QLineEdit()
        self.cmd_input.setPlaceholderText("Enter command...")
        self.cmd_input.returnPressed.connect(self.send_manual_command)
        cmd_layout.addWidget(self.cmd_input)
        
        send_button = QPushButton("Send")
        send_button.clicked.connect(self.send_manual_command)
        cmd_layout.addWidget(send_button)
        
        rx_layout.addLayout(cmd_layout)
        
        middle_splitter.addWidget(rx_widget)
        middle_splitter.setSizes([400, 800])  # Set initial sizes (control panel, RX display)
        
        main_layout.addWidget(middle_splitter)
        
        # Status bar
        self.statusBar().showMessage("Disconnected")
        
        # Set up auto-scroll for log
        self.log_timer = QTimer()
        self.log_timer.timeout.connect(self.auto_scroll_log)
        self.log_timer.start(100)  # Check every 100ms
        
        # Set up activity indicator timers
        self.tx_timer = QTimer()
        self.tx_timer.timeout.connect(self.reset_tx_indicator)
        
        self.rx_timer = QTimer()
        self.rx_timer.timeout.connect(self.reset_rx_indicator)
        
    def create_protocol_status(self, name):
        """Create a protocol status indicator"""
        group = QGroupBox(f"{name} Status")
        layout = QVBoxLayout()
        
        status = QLabel("NOT TESTED")
        status.setAlignment(Qt.AlignCenter)
        status.setStyleSheet("font-size: 16pt; font-weight: bold; color: gray;")
        layout.addWidget(status)
        
        group.setLayout(layout)
        setattr(self, f"{name.lower()}_status_label", status)
        
        return group
        
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
        parity_group = QGroupBox("Parity Settings")
        parity_layout = QGridLayout()
        
        # RS232 is fixed at 8N1
        rs232_label = QLabel("RS232: Fixed at 8N1 (8 data bits, No parity, 1 stop bit)")
        rs232_label.setStyleSheet("font-weight: bold; color: blue;")
        parity_layout.addWidget(rs232_label, 0, 0, 1, 2)
        
        # U913 parity controls
        parity_layout.addWidget(QLabel("U913 Parity:"), 1, 0)
        
        mark_button = QPushButton("MARK Parity (8O1)")
        mark_button.clicked.connect(lambda: self.send_command("MARK"))
        parity_layout.addWidget(mark_button, 1, 1)
        
        space_button = QPushButton("SPACE Parity (8E1)")
        space_button.clicked.connect(lambda: self.send_command("SPACE"))
        parity_layout.addWidget(space_button, 2, 1)
        
        parity_group.setLayout(parity_layout)
        control_layout.addWidget(parity_group, 1, 0, 1, 2)
        
        # Channel selection
        channel_group = QGroupBox("Multiplexer Channel (for U913)")
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
            <li><b>MARK</b> - Switch to MARK parity (8O1) for U913</li>
            <li><b>SPACE</b> - Switch to SPACE parity (8E1) for U913</li>
            <li><b>CHANNEL &lt;0-3&gt;</b> - Select multiplexer channel for U913</li>
            <li><b>SEND &lt;hex data&gt;</b> - Send hex bytes</li>
            <li><b>STATUS</b> - Show current status</li>
            <li><b>LOGS</b> - Show all stored logs</li>
            <li><b>CLEAR</b> - Clear logs</li>
            <li><b>HELP</b> - Show help message</li>
        </ul>
        <p><b>Note:</b> RS232 is fixed at 8N1 (8 data bits, No parity, 1 stop bit)</p>
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
        
        # RS232 Format notice
        rs232_format = QLabel("RS232 Format: 8N1 (8 data bits, No parity, 1 stop bit)")
        rs232_format.setStyleSheet("color: blue; font-weight: bold;")
        test_layout.addWidget(rs232_format, 0, 0, 1, 2)
        
        # Standard RS232 test
        rs232_test_button = QPushButton("Test RS232 Connection")
        rs232_test_button.setMinimumHeight(50)
        rs232_test_button.setStyleSheet("font-size: 14pt;")
        rs232_test_button.clicked.connect(self.run_rs232_test)
        test_layout.addWidget(rs232_test_button, 1, 0, 1, 2)
        
        # Loopback test
        loopback_button = QPushButton("RS232 Loopback Test")
        loopback_button.setMinimumHeight(50)
        loopback_button.setStyleSheet("font-size: 14pt;")
        loopback_button.clicked.connect(self.run_loopback_test)
        test_layout.addWidget(loopback_button, 2, 0, 1, 2)
        
        # Direct connection notice
        connection_notice = QLabel("RS232 is directly connected to the RS232 chip (no multiplexer)")
        connection_notice.setStyleSheet("color: blue; font-weight: bold;")
        test_layout.addWidget(connection_notice, 3, 0, 1, 2)
        
        # RS232 Echo Test
        echo_button = QPushButton("RS232 Echo Test")
        echo_button.setMinimumHeight(50)
        echo_button.clicked.connect(self.run_rs232_echo_test)
        test_layout.addWidget(echo_button, 4, 0, 1, 2)
        
        # RS232 Baud Rate Test
        baud_test_button = QPushButton("RS232 Baud Rate Test")
        baud_test_button.setMinimumHeight(50)
        baud_test_button.clicked.connect(self.run_rs232_baud_test)
        test_layout.addWidget(baud_test_button, 5, 0, 1, 2)
        
        test_group.setLayout(test_layout)
        layout.addWidget(test_group)
        
        # Custom command group
        custom_group = QGroupBox("Custom RS232 Command")
        custom_layout = QVBoxLayout()
        
        # Explanation
        explanation = QLabel("Send custom RS232 command:")
        custom_layout.addWidget(explanation)
        
        # Command bytes
        cmd_layout = QHBoxLayout()
        cmd_layout.addWidget(QLabel("Command Bytes:"))
        self.rs232_cmd_input = QLineEdit()
        self.rs232_cmd_input.setPlaceholderText("Enter bytes (e.g., 01 02 03)")
        cmd_layout.addWidget(self.rs232_cmd_input)
        custom_layout.addLayout(cmd_layout)
        
        # Send custom command
        custom_send_button = QPushButton("Send RS232 Command")
        custom_send_button.clicked.connect(self.send_rs232_command)
        custom_layout.addWidget(custom_send_button)
        
        custom_group.setLayout(custom_layout)
        layout.addWidget(custom_group)
        
        tab.setLayout(layout)
        return tab
    
    def create_rs485_tab(self):
        """Create the RS485/Modbus testing tab"""
        tab = QWidget()
        layout = QVBoxLayout()
        
        # Simple test
        simple_group = QGroupBox("RS485 Modbus Tests")
        simple_layout = QGridLayout()
        
        # Direct connection notice
        rs485_notice = QLabel("RS485 is directly connected to the ESP32 MCU (no multiplexer)")
        rs485_notice.setStyleSheet("color: blue; font-weight: bold;")
        simple_layout.addWidget(rs485_notice, 0, 0, 1, 2)
        
        # Simple test button
        simple_test_button = QPushButton("Run Simple RS485 Test")
        simple_test_button.setMinimumHeight(50)
        simple_test_button.setStyleSheet("font-size: 14pt;")
        simple_test_button.clicked.connect(self.run_rs485_simple_test)
        simple_layout.addWidget(simple_test_button, 1, 0, 1, 2)
        
        # RS485 Echo Test
        rs485_echo_button = QPushButton("RS485 Echo Test")
        rs485_echo_button.setMinimumHeight(50)
        rs485_echo_button.clicked.connect(self.run_rs485_echo_test)
        simple_layout.addWidget(rs485_echo_button, 2, 0, 1, 2)
        
        # RS485 Scan Test
        rs485_scan_button = QPushButton("Scan for Modbus Devices")
        rs485_scan_button.setMinimumHeight(50)
        rs485_scan_button.clicked.connect(self.run_rs485_scan)
        simple_layout.addWidget(rs485_scan_button, 3, 0, 1, 2)
        
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
        
        # Multiplexer notice
        u913_notice = QLabel("U913 is connected through the multiplexer - select channel before testing")
        u913_notice.setStyleSheet("color: blue; font-weight: bold;")
        u913_layout.addWidget(u913_notice, 0, 0, 1, 2)
        
        # Channel selection
        u913_layout.addWidget(QLabel("Select Channel:"), 1, 0)
        self.u913_channel_combo = QComboBox()
        self.u913_channel_combo.addItems(["Channel 0", "Channel 1", "Channel 2", "Channel 3"])
        u913_layout.addWidget(self.u913_channel_combo, 1, 1)
        
        # Test button
        u913_test_button = QPushButton("Test U913 Device")
        u913_test_button.clicked.connect(self.test_u913_on_channel)
        u913_test_button.setMinimumHeight(50)
        u913_test_button.setStyleSheet("font-size: 14pt;")
        u913_layout.addWidget(u913_test_button, 2, 0, 1, 2)
        
        # U913 Communication Test
        u913_comm_button = QPushButton("U913 Communication Test")
        u913_comm_button.clicked.connect(self.run_u913_comm_test)
        u913_comm_button.setMinimumHeight(50)
        u913_layout.addWidget(u913_comm_button, 3, 0, 1, 2)
        
        # U913 Status Test
        u913_status_button = QPushButton("U913 Status Test")
        u913_status_button.clicked.connect(self.run_u913_status_test)
        u913_status_button.setMinimumHeight(50)
        u913_layout.addWidget(u913_status_button, 4, 0, 1, 2)
        
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
    
    def create_test_suite_tab(self):
        """Create the test suite tab for running comprehensive tests"""
        tab = QWidget()
        layout = QVBoxLayout()
        
        # Test suite controls
        suite_group = QGroupBox("Comprehensive Test Suite")
        suite_layout = QVBoxLayout()
        
        # Test selection
        test_selection = QGroupBox("Select Tests to Run")
        test_selection_layout = QGridLayout()
        
        # RS232 Tests
        self.rs232_test_check = QCheckBox("RS232 Connection Test")
        self.rs232_test_check.setChecked(True)
        test_selection_layout.addWidget(self.rs232_test_check, 0, 0)
        
        self.rs232_loopback_check = QCheckBox("RS232 Loopback Test")
        self.rs232_loopback_check.setChecked(True)
        test_selection_layout.addWidget(self.rs232_loopback_check, 0, 1)
        
        self.rs232_echo_check = QCheckBox("RS232 Echo Test")
        self.rs232_echo_check.setChecked(True)
        test_selection_layout.addWidget(self.rs232_echo_check, 1, 0)
        
        self.rs232_baud_check = QCheckBox("RS232 Baud Rate Test")
        self.rs232_baud_check.setChecked(True)
        test_selection_layout.addWidget(self.rs232_baud_check, 1, 1)
        
        # RS485 Tests
        self.rs485_test_check = QCheckBox("RS485 Simple Test")
        self.rs485_test_check.setChecked(True)
        test_selection_layout.addWidget(self.rs485_test_check, 2, 0)
        
        self.rs485_echo_check = QCheckBox("RS485 Echo Test")
        self.rs485_echo_check.setChecked(True)
        test_selection_layout.addWidget(self.rs485_echo_check, 2, 1)
        
        self.rs485_scan_check = QCheckBox("RS485 Device Scan")
        self.rs485_scan_check.setChecked(True)
        test_selection_layout.addWidget(self.rs485_scan_check, 3, 0)
        
        # U913 Tests
        self.u913_test_check = QCheckBox("U913 Device Test")
        self.u913_test_check.setChecked(True)
        test_selection_layout.addWidget(self.u913_test_check, 4, 0)
        
        self.u913_comm_check = QCheckBox("U913 Communication Test")
        self.u913_comm_check.setChecked(True)
        test_selection_layout.addWidget(self.u913_comm_check, 4, 1)
        
        self.u913_status_check = QCheckBox("U913 Status Test")
        self.u913_status_check.setChecked(True)
        test_selection_layout.addWidget(self.u913_status_check, 5, 0)
        
        # U913 Channel selection
        test_selection_layout.addWidget(QLabel("U913 Channel:"), 6, 0)
        self.test_u913_channel = QComboBox()
        self.test_u913_channel.addItems(["Channel 0", "Channel 1", "Channel 2", "Channel 3"])
        test_selection_layout.addWidget(self.test_u913_channel, 6, 1)
        
        # Select/Deselect All
        select_all_button = QPushButton("Select All")
        select_all_button.clicked.connect(self.select_all_tests)
        test_selection_layout.addWidget(select_all_button, 7, 0)
        
        deselect_all_button = QPushButton("Deselect All")
        deselect_all_button.clicked.connect(self.deselect_all_tests)
        test_selection_layout.addWidget(deselect_all_button, 7, 1)
        
        test_selection.setLayout(test_selection_layout)
        suite_layout.addWidget(test_selection)
        
        # Run test suite button
        run_suite_button = QPushButton("Run Test Suite")
        run_suite_button.setMinimumHeight(50)
        run_suite_button.setStyleSheet("font-size: 16pt; font-weight: bold; background-color: #4CAF50; color: white;")
        run_suite_button.clicked.connect(self.run_test_suite)
        suite_layout.addWidget(run_suite_button)
        
        # Progress bar
        self.test_progress = QProgressBar()
        self.test_progress.setRange(0, 100)
        self.test_progress.setValue(0)
        suite_layout.addWidget(self.test_progress)
        
        # Test results table
        results_group = QGroupBox("Test Results")
        results_layout = QVBoxLayout()
        
        self.results_table = QTableWidget()
        self.results_table.setColumnCount(4)
        self.results_table.setHorizontalHeaderLabels(["Test", "Protocol", "Result", "Time"])
        self.results_table.horizontalHeader().setSectionResizeMode(0, QHeaderView.Stretch)
        self.results_table.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.results_table.horizontalHeader().setSectionResizeMode(2, QHeaderView.ResizeToContents)
        self.results_table.horizontalHeader().setSectionResizeMode(3, QHeaderView.ResizeToContents)
        results_layout.addWidget(self.results_table)
        
        # Export results button
        export_results_button = QPushButton("Export Results to CSV")
        export_results_button.clicked.connect(self.export_test_results)
        results_layout.addWidget(export_results_button)
        
        results_group.setLayout(results_layout)
        suite_layout.addWidget(results_group)
        
        suite_group.setLayout(suite_layout)
        layout.addWidget(suite_group)
        
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
        
        # Show RX activity
        self.rx_indicator.setStyleSheet("background-color: green;")
        self.rx_timer.start(100)  # Reset after 100ms
        
        # Write to log file if enabled
        if self.logging_enabled and self.log_file:
            try:
                self.log_file.write(data)
                self.log_file.flush()
            except Exception as e:
                self.log_message(f"Error writing to log file: {str(e)}", "red")
                self.toggle_logging(False)
                self.log_checkbox.setChecked(False)
        
        # Auto-detect test results
        if "SUCCESS" in data or "✓ SUCCESS" in data:
            self.set_test_status("PASS", "Device responded correctly")
            
            # Check which protocol test succeeded
            if "RS232 Test" in data or "RS232 Loopback" in data:
                self.set_protocol_status("rs232", "PASS")
            elif "RS485 Test" in data or "Modbus" in data:
                self.set_protocol_status("rs485", "PASS")
            elif "U913 Test" in data or "U913 device" in data:
                self.set_protocol_status("u913", "PASS")
                
        elif "FAILED" in data or "✗ FAILED" in data:
            self.set_test_status("FAIL", "Device failed to respond correctly")
            
            # Check which protocol test failed
            if "RS232 Test" in data or "RS232 Loopback" in data:
                self.set_protocol_status("rs232", "FAIL")
            elif "RS485 Test" in data or "Modbus" in data:
                self.set_protocol_status("rs485", "FAIL")
            elif "U913 Test" in data or "U913 device" in data:
                self.set_protocol_status("u913", "FAIL")
    
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
            
            # Show TX activity
            self.tx_indicator.setStyleSheet("background-color: red;")
            self.tx_timer.start(100)  # Reset after 100ms
            
        return success
    
    def reset_tx_indicator(self):
        """Reset the TX activity indicator"""
        self.tx_indicator.setStyleSheet("background-color: gray;")
        self.tx_timer.stop()
    
    def reset_rx_indicator(self):
        """Reset the RX activity indicator"""
        self.rx_indicator.setStyleSheet("background-color: gray;")
        self.rx_timer.stop()
    
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
    
    def send_rs232_command(self):
        """Send a custom RS232 command"""
        cmd_bytes = self.rs232_cmd_input.text().strip()
        
        if not cmd_bytes:
            QMessageBox.warning(self, "Input Error", "Please enter command bytes")
            return
        
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
        self.set_test_status("RUNNING")
        self.send_command(f"CHANNEL {channel}")
        time.sleep(0.2)  # Small delay
        self.send_command(f"U913")
    
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
        if self.auto_scroll_check.isChecked() and self.log_display.verticalScrollBar().value() == self.log_display.verticalScrollBar().maximum():
            # Only auto-scroll if enabled and already at the bottom
            cursor = self.log_display.textCursor()
            cursor.movePosition(QTextCursor.End)
            self.log_display.setTextCursor(cursor)

    def set_test_status(self, status, message=""):
        """Set the test status indicator"""
        if status == "PASS":
            self.status_label.setText("PASS")
            self.status_label.setStyleSheet("font-size: 24pt; font-weight: bold; background-color: #4CAF50; color: white; border-radius: 5px;")
            self.log_message(f"TEST PASSED: {message}", "green")
            # Play sound for pass
            QApplication.beep()
        elif status == "FAIL":
            self.status_label.setText("FAIL")
            self.status_label.setStyleSheet("font-size: 24pt; font-weight: bold; background-color: #F44336; color: white; border-radius: 5px;")
            self.log_message(f"TEST FAILED: {message}", "red")
            # Play sound for fail (two beeps)
            QApplication.beep()
            QTimer.singleShot(300, QApplication.beep)
        elif status == "RUNNING":
            self.status_label.setText("TESTING...")
            self.status_label.setStyleSheet("font-size: 24pt; font-weight: bold; background-color: #2196F3; color: white; border-radius: 5px;")
        else:
            self.status_label.setText("NOT TESTED")
            self.status_label.setStyleSheet("font-size: 24pt; font-weight: bold; color: gray;")
    
    def set_protocol_status(self, protocol, status):
        """Set the status for a specific protocol"""
        label = getattr(self, f"{protocol}_status_label")
        
        if status == "PASS":
            label.setText("PASS")
            label.setStyleSheet("font-size: 16pt; font-weight: bold; background-color: #4CAF50; color: white; border-radius: 5px;")
        elif status == "FAIL":
            label.setText("FAIL")
            label.setStyleSheet("font-size: 16pt; font-weight: bold; background-color: #F44336; color: white; border-radius: 5px;")
        elif status == "RUNNING":
            label.setText("TESTING...")
            label.setStyleSheet("font-size: 16pt; font-weight: bold; background-color: #2196F3; color: white; border-radius: 5px;")
        else:
            label.setText("NOT TESTED")
            label.setStyleSheet("font-size: 16pt; font-weight: bold; color: gray;")
    
    def run_rs232_test(self):
        """Run RS232 test with status updates"""
        self.set_test_status("RUNNING")
        self.set_protocol_status("rs232", "RUNNING")
        self.send_command("TEST")
        
        # Add to test results
        self.add_test_result("RS232 Connection Test", "RS232")
    
    def run_loopback_test(self):
        """Run loopback test with status updates"""
        self.set_test_status("RUNNING")
        self.set_protocol_status("rs232", "RUNNING")
        self.send_command("LOOPBACK")
        
        # Add to test results
        self.add_test_result("RS232 Loopback Test", "RS232")
    
    def run_rs232_echo_test(self):
        """Run RS232 echo test"""
        self.set_test_status("RUNNING")
        self.set_protocol_status("rs232", "RUNNING")
        self.send_command("RS232ECHO")
        
        # Add to test results
        self.add_test_result("RS232 Echo Test", "RS232")
    
    def run_rs232_baud_test(self):
        """Run RS232 baud rate test"""
        self.set_test_status("RUNNING")
        self.set_protocol_status("rs232", "RUNNING")
        self.send_command("RS232BAUD")
        
        # Add to test results
        self.add_test_result("RS232 Baud Rate Test", "RS232")
    
    def run_rs485_simple_test(self):
        """Run simple RS485 test"""
        self.set_test_status("RUNNING")
        self.set_protocol_status("rs485", "RUNNING")
        self.send_command("RS485")
        
        # Add to test results
        self.add_test_result("RS485 Simple Test", "RS485")
    
    def run_rs485_echo_test(self):
        """Run RS485 echo test"""
        self.set_test_status("RUNNING")
        self.set_protocol_status("rs485", "RUNNING")
        self.send_command("RS485ECHO")
        
        # Add to test results
        self.add_test_result("RS485 Echo Test", "RS485")
    
    def run_rs485_scan(self):
        """Run RS485 device scan"""
        self.set_test_status("RUNNING")
        self.set_protocol_status("rs485", "RUNNING")
        self.send_command("RS485SCAN")
        
        # Add to test results
        self.add_test_result("RS485 Device Scan", "RS485")
    
    def run_u913_test(self):
        """Run U913 test with status updates"""
        self.set_test_status("RUNNING")
        self.set_protocol_status("u913", "RUNNING")
        self.send_command("U913")
        
        # Add to test results
        self.add_test_result("U913 Device Test", "U913")
    
    def run_u913_comm_test(self):
        """Run U913 communication test"""
        channel = self.u913_channel_combo.currentIndex()
        self.set_test_status("RUNNING")
        self.set_protocol_status("u913", "RUNNING")
        self.send_command(f"CHANNEL {channel}")
        time.sleep(0.2)
        self.send_command("U913COMM")
        
        # Add to test results
        self.add_test_result("U913 Communication Test", "U913")
    
    def run_u913_status_test(self):
        """Run U913 status test"""
        channel = self.u913_channel_combo.currentIndex()
        self.set_test_status("RUNNING")
        self.set_protocol_status("u913", "RUNNING")
        self.send_command(f"CHANNEL {channel}")
        time.sleep(0.2)
        self.send_command("U913STATUS")
        
        # Add to test results
        self.add_test_result("U913 Status Test", "U913")
    
    def run_u913_test_on_channel(self, channel):
        """Run U913 test on specific channel with status updates"""
        self.set_test_status("RUNNING")
        self.set_protocol_status("u913", "RUNNING")
        self.send_command(f"U913 {channel}")
        
        # Add to test results
        self.add_test_result(f"U913 Test on Channel {channel}", "U913")
    
    def select_all_tests(self):
        """Select all tests in the test suite"""
        self.rs232_test_check.setChecked(True)
        self.rs232_loopback_check.setChecked(True)
        self.rs232_echo_check.setChecked(True)
        self.rs232_baud_check.setChecked(True)
        self.rs485_test_check.setChecked(True)
        self.rs485_echo_check.setChecked(True)
        self.rs485_scan_check.setChecked(True)
        self.u913_test_check.setChecked(True)
        self.u913_comm_check.setChecked(True)
        self.u913_status_check.setChecked(True)
    
    def deselect_all_tests(self):
        """Deselect all tests in the test suite"""
        self.rs232_test_check.setChecked(False)
        self.rs232_loopback_check.setChecked(False)
        self.rs232_echo_check.setChecked(False)
        self.rs232_baud_check.setChecked(False)
        self.rs485_test_check.setChecked(False)
        self.rs485_echo_check.setChecked(False)
        self.rs485_scan_check.setChecked(False)
        self.u913_test_check.setChecked(False)
        self.u913_comm_check.setChecked(False)
        self.u913_status_check.setChecked(False)
    
    def run_test_suite(self):
        """Run the selected tests in sequence"""
        # Get selected tests
        tests = []
        
        if self.rs232_test_check.isChecked():
            tests.append(("RS232 Connection Test", "RS232", self.run_rs232_test))
        if self.rs232_loopback_check.isChecked():
            tests.append(("RS232 Loopback Test", "RS232", self.run_loopback_test))
        if self.rs232_echo_check.isChecked():
            tests.append(("RS232 Echo Test", "RS232", self.run_rs232_echo_test))
        if self.rs232_baud_check.isChecked():
            tests.append(("RS232 Baud Rate Test", "RS232", self.run_rs232_baud_test))
        
        if self.rs485_test_check.isChecked():
            tests.append(("RS485 Simple Test", "RS485", self.run_rs485_simple_test))
        if self.rs485_echo_check.isChecked():
            tests.append(("RS485 Echo Test", "RS485", self.run_rs485_echo_test))
        if self.rs485_scan_check.isChecked():
            tests.append(("RS485 Device Scan", "RS485", self.run_rs485_scan))
        
        u913_channel = self.test_u913_channel.currentIndex()
        if self.u913_test_check.isChecked():
            tests.append(("U913 Device Test", "U913", lambda: self.run_u913_test_on_channel(u913_channel)))
        if self.u913_comm_check.isChecked():
            tests.append(("U913 Communication Test", "U913", self.run_u913_comm_test))
        if self.u913_status_check.isChecked():
            tests.append(("U913 Status Test", "U913", self.run_u913_status_test))
        
        if not tests:
            QMessageBox.warning(self, "No Tests Selected", "Please select at least one test to run")
            return
        
        # Clear previous results
        self.results_table.setRowCount(0)
        self.test_results = []
        
        # Run tests in sequence
        self.log_message("Starting Test Suite", "blue")
        self.test_progress.setValue(0)
        
        # Create a timer to run tests with delay between them
        self.test_index = 0
        self.tests_to_run = tests
        self.test_timer = QTimer()
        self.test_timer.timeout.connect(self.run_next_test)
        self.test_timer.start(1000)  # Start first test after 1 second
    
    def run_next_test(self):
        """Run the next test in the sequence"""
        if self.test_index < len(self.tests_to_run):
            test_name, protocol, test_func = self.tests_to_run[self.test_index]
            self.log_message(f"Running test: {test_name}", "blue")
            
            # Update progress
            progress = int((self.test_index / len(self.tests_to_run)) * 100)
            self.test_progress.setValue(progress)
            
            # Run the test
            test_func()
            
            # Move to next test
            self.test_index += 1
            self.test_timer.start(3000)  # Wait 3 seconds before next test
        else:
            # All tests completed
            self.test_timer.stop()
            self.test_progress.setValue(100)
            self.log_message("Test Suite Completed", "blue")
    
    def add_test_result(self, test_name, protocol, result="PENDING"):
        """Add a test result to the results table"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        # Add to internal list
        self.test_results.append({
            "test": test_name,
            "protocol": protocol,
            "result": result,
            "time": timestamp
        })
        
        # Add to table
        row = self.results_table.rowCount()
        self.results_table.insertRow(row)
        self.results_table.setItem(row, 0, QTableWidgetItem(test_name))
        self.results_table.setItem(row, 1, QTableWidgetItem(protocol))
        self.results_table.setItem(row, 2, QTableWidgetItem(result))
        self.results_table.setItem(row, 3, QTableWidgetItem(timestamp))
        
        # Color code result
        if result == "PASS":
            self.results_table.item(row, 2).setBackground(QColor("#4CAF50"))
            self.results_table.item(row, 2).setForeground(QColor("white"))
        elif result == "FAIL":
            self.results_table.item(row, 2).setBackground(QColor("#F44336"))
            self.results_table.item(row, 2).setForeground(QColor("white"))
    
    def export_test_results(self):
        """Export test results to CSV file"""
        if not self.test_results:
            QMessageBox.warning(self, "No Results", "No test results to export")
            return
        
        file_path, _ = QFileDialog.getSaveFileName(
            self, "Export Test Results", "", "CSV Files (*.csv);;All Files (*)"
        )
        
        if file_path:
            try:
                with open(file_path, 'w', newline='') as csvfile:
                    fieldnames = ['test', 'protocol', 'result', 'time']
                    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                    
                    writer.writeheader()
                    for result in self.test_results:
                        writer.writerow(result)
                
                self.log_message(f"Test results exported to {file_path}", "green")
            except Exception as e:
                self.log_message(f"Error exporting results: {str(e)}", "red")
                QMessageBox.critical(self, "Export Error", f"Error exporting results: {str(e)}")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setStyle('Fusion')  # Use Fusion style for a consistent look
    
    window = ProtocolAnalyzerUI()
    window.show()
    sys.exit(app.exec_())
