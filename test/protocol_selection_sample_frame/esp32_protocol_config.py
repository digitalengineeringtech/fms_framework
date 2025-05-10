#!/usr/bin/env python3
"""
ESP32 Protocol Configuration Tool
A PyQt GUI application for configuring ESP32 protocol settings
"""

import sys
import time
import serial
import serial.tools.list_ports
import json
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QComboBox, QSpinBox, QPushButton, QGroupBox, QRadioButton,
    QTabWidget, QFormLayout, QLineEdit, QMessageBox, QCheckBox,
    QStatusBar, QGridLayout, QFrame, QSplitter, QTextEdit, QFileDialog
)
from PyQt5.QtCore import Qt, QTimer, pyqtSignal, QThread
from PyQt5.QtGui import QFont, QIcon, QTextCursor

class SerialThread(QThread):
    """Thread for handling serial communication without blocking the GUI"""
    data_received = pyqtSignal(bytes)
    connection_error = pyqtSignal(str)
    
    def __init__(self, port, baudrate):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.running = False
        self.ser = None
    
    def run(self):
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=0.1
            )
            self.running = True
            
            while self.running:
                if self.ser.in_waiting:
                    data = self.ser.read(self.ser.in_waiting)
                    self.data_received.emit(data)
                time.sleep(0.01)
                
        except serial.SerialException as e:
            self.connection_error.emit(str(e))
            self.running = False
        finally:
            if self.ser and self.ser.is_open:
                self.ser.close()
    
    def write(self, data):
        if self.ser and self.ser.is_open:
            self.ser.write(data)
    
    def stop(self):
        self.running = False
        self.wait()

class ESP32ProtocolConfig(QMainWindow):
    def __init__(self):
        super().__init__()
        self.serial_thread = None
        # Create console attribute before calling any methods that might use it
        self.console = None
        self.initUI()
        
        # Response parsing state
        self.response_buffer = ""
        self.waiting_for_response = False
        self.response_timeout = 3000  # 3 seconds
        self.response_timer = QTimer()
        self.response_timer.timeout.connect(self.handle_response_timeout)
    
    def initUI(self):
        self.setWindowTitle("ESP32 Protocol Configuration")
        self.setMinimumSize(800, 600)
        
        # Main widget and layout
        main_widget = QWidget()
        main_layout = QVBoxLayout()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)
        
        # Create console output first
        console_group = QGroupBox("Console Output")
        console_layout = QVBoxLayout()
        
        self.console = QTextEdit()
        self.console.setReadOnly(True)
        self.console.setFont(QFont("Courier New", 10))
        console_layout.addWidget(self.console)
        
        # Console buttons layout
        console_buttons_layout = QHBoxLayout()
        
        # Clear console button
        clear_button = QPushButton("Clear Console")
        clear_button.clicked.connect(self.clear_console)
        console_buttons_layout.addWidget(clear_button)
        
        # Save console button
        save_console_button = QPushButton("Save Console")
        save_console_button.clicked.connect(self.save_console)
        console_buttons_layout.addWidget(save_console_button)
        
        console_layout.addLayout(console_buttons_layout)
        console_group.setLayout(console_layout)
        
        # Serial connection group
        connection_group = QGroupBox("Serial Connection")
        connection_layout = QHBoxLayout()
        
        # Port selection
        port_label = QLabel("Port:")
        self.port_combo = QComboBox()
        
        # Baud rate selection
        baud_label = QLabel("Baud Rate:")
        self.baud_combo = QComboBox()
        baud_rates = ["9600", "19200", "38400", "57600", "115200", "230400", "460800", "921600"]
        self.baud_combo.addItems(baud_rates)
        self.baud_combo.setCurrentText("115200")
        
        # Connect and refresh buttons
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        
        refresh_button = QPushButton("Refresh Ports")
        refresh_button.clicked.connect(self.refresh_ports)
        
        # Add widgets to connection layout
        connection_layout.addWidget(port_label)
        connection_layout.addWidget(self.port_combo)
        connection_layout.addWidget(baud_label)
        connection_layout.addWidget(self.baud_combo)
        connection_layout.addWidget(self.connect_button)
        connection_layout.addWidget(refresh_button)
        connection_group.setLayout(connection_layout)
        
        # Add connection group to main layout
        main_layout.addWidget(connection_group)
        
        # Create tab widget for configuration
        tab_widget = QTabWidget()
        
        # Protocol tab
        protocol_tab = QWidget()
        protocol_layout = QVBoxLayout()
        protocol_tab.setLayout(protocol_layout)
        
        # Protocol selection group
        protocol_group = QGroupBox("Protocol Selection")
        protocol_grid = QGridLayout()
        
        # Protocol radio buttons
        self.protocol_radios = []
        
        lanfeng_radio = QRadioButton("Lanfeng")
        lanfeng_radio.setChecked(True)
        self.protocol_radios.append(lanfeng_radio)
        protocol_grid.addWidget(lanfeng_radio, 0, 0)
        
        huangyang_radio = QRadioButton("Huangyang")
        self.protocol_radios.append(huangyang_radio)
        protocol_grid.addWidget(huangyang_radio, 0, 1)
        
        redstar_radio = QRadioButton("Redstar")
        self.protocol_radios.append(redstar_radio)
        protocol_grid.addWidget(redstar_radio, 1, 0)
        
        tasuno_radio = QRadioButton("Tasuno")
        self.protocol_radios.append(tasuno_radio)
        protocol_grid.addWidget(tasuno_radio, 1, 1)
        
        protocol_group.setLayout(protocol_grid)
        protocol_layout.addWidget(protocol_group)
        
        # Nozzle configuration group
        nozzle_group = QGroupBox("Nozzle Configuration")
        nozzle_layout = QFormLayout()
        
        # Nozzle count spinner
        self.nozzle_count = QSpinBox()
        self.nozzle_count.setMinimum(1)
        self.nozzle_count.setMaximum(10)
        self.nozzle_count.setValue(1)
        nozzle_layout.addRow("Number of Nozzles:", self.nozzle_count)
        
        nozzle_group.setLayout(nozzle_layout)
        protocol_layout.addWidget(nozzle_group)
        
        # Add protocol tab to tab widget
        tab_widget.addTab(protocol_tab, "Protocol")
        
        # Serial settings tab
        serial_tab = QWidget()
        serial_layout = QFormLayout()
        serial_tab.setLayout(serial_layout)
        
        # Data bits
        self.data_bits = QComboBox()
        self.data_bits.addItems(["5", "6", "7", "8"])
        self.data_bits.setCurrentText("8")
        serial_layout.addRow("Data Bits:", self.data_bits)
        
        # Stop bits
        self.stop_bits = QComboBox()
        self.stop_bits.addItems(["1", "1.5", "2"])
        serial_layout.addRow("Stop Bits:", self.stop_bits)
        
        # Parity
        self.parity = QComboBox()
        self.parity.addItems(["None", "Even", "Odd", "Mark", "Space"])
        serial_layout.addRow("Parity:", self.parity)
        
        # Flow control
        self.flow_control = QComboBox()
        self.flow_control.addItems(["None", "Hardware (RTS/CTS)", "Software (XON/XOFF)"])
        serial_layout.addRow("Flow Control:", self.flow_control)
        
        # Add serial tab to tab widget
        tab_widget.addTab(serial_tab, "Serial Settings")
        
        # Advanced tab
        advanced_tab = QWidget()
        advanced_layout = QFormLayout()
        advanced_tab.setLayout(advanced_layout)
        
        # Timeout
        self.timeout = QSpinBox()
        self.timeout.setMinimum(100)
        self.timeout.setMaximum(10000)
        self.timeout.setValue(1000)
        self.timeout.setSingleStep(100)
        self.timeout.setSuffix(" ms")
        advanced_layout.addRow("Command Timeout:", self.timeout)
        
        # Debug mode
        self.debug_mode = QCheckBox("Enable Debug Output")
        self.debug_mode.setChecked(True)
        self.debug_mode.stateChanged.connect(self.toggle_debug_mode)
        advanced_layout.addRow("", self.debug_mode)
        
        # Raw command input
        self.raw_command = QLineEdit()
        send_raw_button = QPushButton("Send")
        send_raw_button.clicked.connect(self.send_raw_command)
        
        raw_command_layout = QHBoxLayout()
        raw_command_layout.addWidget(self.raw_command)
        raw_command_layout.addWidget(send_raw_button)
        
        advanced_layout.addRow("Raw Command:", raw_command_layout)
        
        # Add advanced tab to tab widget
        tab_widget.addTab(advanced_tab, "Advanced")
        
        # Add tab widget to main layout
        main_layout.addWidget(tab_widget)
        
        # Create a splitter for console output
        splitter = QSplitter(Qt.Vertical)
        splitter.addWidget(console_group)
        
        # Add splitter to main layout
        main_layout.addWidget(splitter)
        
        # Action buttons
        button_layout = QHBoxLayout()
        
        self.save_button = QPushButton("Save Configuration")
        self.save_button.clicked.connect(self.save_configuration)
        self.save_button.setEnabled(False)
        button_layout.addWidget(self.save_button)
        
        self.load_button = QPushButton("Load Configuration")
        self.load_button.clicked.connect(self.load_configuration)
        self.load_button.setEnabled(False)
        button_layout.addWidget(self.load_button)
        
        self.test_button = QPushButton("Test Connection")
        self.test_button.clicked.connect(self.test_connection)
        self.test_button.setEnabled(False)
        button_layout.addWidget(self.test_button)
        
        # Export/Import config buttons
        self.export_button = QPushButton("Export Config")
        self.export_button.clicked.connect(self.export_config)
        self.export_button.setEnabled(False)
        button_layout.addWidget(self.export_button)
        
        self.import_button = QPushButton("Import Config")
        self.import_button.clicked.connect(self.import_config)
        button_layout.addWidget(self.import_button)
        
        main_layout.addLayout(button_layout)
        
        # Status bar
        self.statusBar = QStatusBar()
        self.setStatusBar(self.statusBar)
        self.statusBar.showMessage("Disconnected")
        
        # Now that console is created, we can refresh ports
        self.refresh_ports()
        
        # Log initial message
        self.log_message("ESP32 Protocol Configuration Tool started")
        self.log_message("Please connect to your ESP32 device")
    
    def refresh_ports(self):
        """Refresh the list of available serial ports"""
        self.port_combo.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.port_combo.addItem(port.device)
        
        if self.port_combo.count() == 0:
            self.log_message("No serial ports found")
            self.statusBar.showMessage("No serial ports found")
        else:
            self.log_message(f"Found {self.port_combo.count()} serial ports")
    
    def toggle_connection(self):
        """Connect to or disconnect from the ESP32"""
        if self.serial_thread and self.serial_thread.running:
            # Disconnect
            self.serial_thread.stop()
            self.serial_thread = None
            self.connect_button.setText("Connect")
            self.save_button.setEnabled(False)
            self.load_button.setEnabled(False)
            self.test_button.setEnabled(False)
            self.export_button.setEnabled(False)
            self.statusBar.showMessage("Disconnected")
            self.log_message("Disconnected from ESP32")
        else:
            # Connect
            port = self.port_combo.currentText()
            baudrate = int(self.baud_combo.currentText())
            
            if not port:
                QMessageBox.warning(self, "Error", "No serial port selected")
                return
            
            self.log_message(f"Connecting to {port} at {baudrate} baud...")
            
            self.serial_thread = SerialThread(port, baudrate)
            self.serial_thread.data_received.connect(self.handle_data)
            self.serial_thread.connection_error.connect(self.handle_connection_error)
            self.serial_thread.start()
            
            self.connect_button.setText("Disconnect")
            self.save_button.setEnabled(True)
            self.load_button.setEnabled(True)
            self.test_button.setEnabled(True)
            self.export_button.setEnabled(True)
            self.statusBar.showMessage(f"Connected to {port}")
            self.log_message(f"Connected to {port} at {baudrate} baud")
            
            # Send a help command to show available commands
            self.send_command("HELP:")
    
    def handle_data(self, data):
        """Handle data received from the ESP32"""
        try:
            text = data.decode('utf-8')
            self.log_message(f"RX: {text.strip()}")
            
            # Add to response buffer for parsing
            self.response_buffer += text
            
            # Parse response if we're waiting for one
            if self.waiting_for_response:
                self.parse_response()
                
        except UnicodeDecodeError:
            # If it's not valid UTF-8, display as hex
            hex_data = ' '.join([f"{b:02X}" for b in data])
            self.log_message(f"RX (HEX): {hex_data}")
    
    def parse_response(self):
        """Parse response from ESP32"""
        # Check for specific responses based on what we're waiting for
        # This is a simple example - you might need more sophisticated parsing
        
        # For now, just check if we got a complete response (ends with newline)
        if '\n' in self.response_buffer:
            self.waiting_for_response = False
            self.response_timer.stop()
            self.response_buffer = ""
    
    def handle_response_timeout(self):
        """Handle timeout waiting for response"""
        self.waiting_for_response = False
        self.response_timer.stop()
        self.log_message("Warning: Timeout waiting for response from ESP32")
        self.statusBar.showMessage("Response timeout")
    
    def handle_connection_error(self, error_msg):
        """Handle serial connection errors"""
        QMessageBox.critical(self, "Connection Error", error_msg)
        self.connect_button.setText("Connect")
        self.save_button.setEnabled(False)
        self.load_button.setEnabled(False)
        self.test_button.setEnabled(False)
        self.export_button.setEnabled(False)
        self.statusBar.showMessage("Connection Error")
        self.log_message(f"Connection Error: {error_msg}")
    
    def log_message(self, message):
        """Add a message to the console output"""
        if self.console is None:
            return  # Skip logging if console doesn't exist yet
            
        timestamp = time.strftime("%H:%M:%S")
        self.console.append(f"[{timestamp}] {message}")
        # Scroll to bottom
        self.console.moveCursor(QTextCursor.End)
    
    def clear_console(self):
        """Clear the console output"""
        self.console.clear()
        self.log_message("Console cleared")
    
    def save_console(self):
        """Save console output to a file"""
        filename, _ = QFileDialog.getSaveFileName(
            self, "Save Console Output", "", "Text Files (*.txt);;All Files (*)"
        )
        
        if filename:
            try:
                with open(filename, 'w') as f:
                    f.write(self.console.toPlainText())
                self.log_message(f"Console output saved to {filename}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Failed to save console output: {str(e)}")
    
    def get_selected_protocol(self):
        """Get the currently selected protocol"""
        for i, radio in enumerate(self.protocol_radios):
            if radio.isChecked():
                return i
        return 0  # Default to Lanfeng
    
    def get_protocol_name(self, protocol_index):
        """Get the name of the protocol from its index"""
        protocol_names = ["lanfeng", "huangyang", "redstar", "tasuno"]
        return protocol_names[protocol_index]
    
    def send_command(self, command, wait_for_response=True):
        """Send a command to the ESP32"""
        if not self.serial_thread or not self.serial_thread.running:
            QMessageBox.warning(self, "Error", "Not connected to ESP32")
            return False
        
        # Log the command
        self.log_message(f"TX: {command}")
        
        # Send the command
        self.serial_thread.write(f"{command}\n".encode())
        
        # Set up response waiting if needed
        if wait_for_response:
            self.waiting_for_response = True
            self.response_buffer = ""
            self.response_timer.start(self.timeout.value())
        
        return True
    
    def toggle_debug_mode(self):
        """Toggle debug mode on the ESP32"""
        if not self.serial_thread or not self.serial_thread.running:
            return
        
        debug_state = "ON" if self.debug_mode.isChecked() else "OFF"
        self.send_command(f"DEBUG:{debug_state}")
    
    def send_raw_command(self):
        """Send a raw command to the ESP32"""
        command = self.raw_command.text().strip()
        if not command:
            return
        
        if self.send_command(command):
            self.raw_command.clear()
    
    def save_configuration(self):
        """Save the configuration to the ESP32"""
        if not self.serial_thread or not self.serial_thread.running:
            QMessageBox.warning(self, "Error", "Not connected to ESP32")
            return
        
        # Build configuration data
        protocol_index = self.get_selected_protocol()
        protocol_name = self.get_protocol_name(protocol_index)
        
        config = {
            "protocol": protocol_name,
            "nozzleCount": self.nozzle_count.value(),
            "serial": {
                "baudRate": int(self.baud_combo.currentText()),
                "dataBits": int(self.data_bits.currentText()),
                "stopBits": float(self.stop_bits.currentText()),
                "parity": self.parity.currentText().lower(),
                "flowControl": self.flow_control.currentText().split(" ")[0].lower()
            },
            "advanced": {
                "timeout": self.timeout.value(),
                "debug": self.debug_mode.isChecked()
            }
        }
        
        # Convert to JSON for logging
        config_json = json.dumps(config)
        self.log_message(f"Sending configuration: {config_json}")
        
        # Send command to enter config mode
        self.send_command("x", wait_for_response=False)  # Exit any current mode
        time.sleep(0.5)
        self.send_command("", wait_for_response=False)  # Enter config mode
        time.sleep(0.5)
        
        # Send configuration commands using the new format
        
        # Set protocol
        self.send_command(f"PROTOCOL:{protocol_index + 1}")
        time.sleep(0.2)
        
        # Set nozzle count
        self.send_command(f"NOZZLE:{self.nozzle_count.value()}")
        time.sleep(0.2)
        
        # Set baud rate
        self.send_command(f"BAUD:{self.baud_combo.currentText()}")
        time.sleep(0.2)
        
        # Set debug mode
        debug_state = "ON" if self.debug_mode.isChecked() else "OFF"
        self.send_command(f"DEBUG:{debug_state}")
        time.sleep(0.2)
        
        # Save settings
        self.send_command("SAVE:")
        time.sleep(0.5)
        
        # Exit config mode and start
        self.send_command("EXIT:")
        
        self.log_message("Configuration saved to ESP32")
        QMessageBox.information(self, "Success", "Configuration saved to ESP32")
    
    def load_configuration(self):
        """Load the current configuration from the ESP32"""
        if not self.serial_thread or not self.serial_thread.running:
            QMessageBox.warning(self, "Error", "Not connected to ESP32")
            return
        
        # Send command to enter config mode and print current settings
        self.send_command("x", wait_for_response=False)  # Exit any current mode
        time.sleep(0.5)
        self.send_command("", wait_for_response=False)  # Enter config mode
        time.sleep(0.5)
        
        # Request current settings using new format
        self.send_command("STATUS:")
        
        self.log_message("Requested current configuration from ESP32")
        QMessageBox.information(self, "Info", "Current settings requested. Check the console output for details.")
    
    def test_connection(self):
        """Test the connection to the ESP32"""
        if not self.serial_thread or not self.serial_thread.running:
            QMessageBox.warning(self, "Error", "Not connected to ESP32")
            return
        
        # Send a test command using new format
        self.send_command("STATUS:")
        self.log_message("Sent test command to ESP32")
    
    def export_config(self):
        """Export the current configuration to a JSON file"""
        if not self.serial_thread or not self.serial_thread.running:
            QMessageBox.warning(self, "Error", "Not connected to ESP32")
            return
        
        # Build configuration data
        protocol_index = self.get_selected_protocol()
        protocol_name = self.get_protocol_name(protocol_index)
        
        config = {
            "protocol": protocol_name,
            "nozzleCount": self.nozzle_count.value(),
            "serial": {
                "baudRate": int(self.baud_combo.currentText()),
                "dataBits": int(self.data_bits.currentText()),
                "stopBits": float(self.stop_bits.currentText()),
                "parity": self.parity.currentText().lower(),
                "flowControl": self.flow_control.currentText().split(" ")[0].lower()
            },
            "advanced": {
                "timeout": self.timeout.value(),
                "debug": self.debug_mode.isChecked()
            }
        }
        
        # Ask for file location
        filename, _ = QFileDialog.getSaveFileName(
            self, "Export Configuration", "", "JSON Files (*.json);;All Files (*)"
        )
        
        if filename:
            try:
                with open(filename, 'w') as f:
                    json.dump(config, f, indent=2)
                self.log_message(f"Configuration exported to {filename}")
                QMessageBox.information(self, "Success", f"Configuration exported to {filename}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Failed to export configuration: {str(e)}")
    
    def import_config(self):
        """Import configuration from a JSON file"""
        filename, _ = QFileDialog.getOpenFileName(
            self, "Import Configuration", "", "JSON Files (*.json);;All Files (*)"
        )
        
        if not filename:
            return
        
        try:
            with open(filename, 'r') as f:
                config = json.load(f)
            
            # Validate the configuration
            if not isinstance(config, dict):
                raise ValueError("Invalid configuration format")
            
            # Apply the configuration to the UI
            protocol_name = config.get("protocol", "lanfeng").lower()
            protocol_names = ["lanfeng", "huangyang", "redstar", "tasuno"]
            
            if protocol_name in protocol_names:
                protocol_index = protocol_names.index(protocol_name)
                self.protocol_radios[protocol_index].setChecked(True)
            
            nozzle_count = config.get("nozzleCount", 1)
            if 1 <= nozzle_count <= 10:
                self.nozzle_count.setValue(nozzle_count)
            
            serial_config = config.get("serial", {})
            baud_rate = str(serial_config.get("baudRate", 115200))
            if baud_rate in [item for item in range(self.baud_combo.count())]:
                self.baud_combo.setCurrentText(baud_rate)
            
            data_bits = str(serial_config.get("dataBits", 8))
            if data_bits in ["5", "6", "7", "8"]:
                self.data_bits.setCurrentText(data_bits)
            
            stop_bits = str(serial_config.get("stopBits", 1))
            if stop_bits in ["1", "1.5", "2"]:
                self.stop_bits.setCurrentText(stop_bits)
            
            parity = serial_config.get("parity", "none").capitalize()
            if parity in ["None", "Even", "Odd", "Mark", "Space"]:
                self.parity.setCurrentText(parity)
            
            flow_control = serial_config.get("flowControl", "none").lower()
            flow_control_map = {
                "none": "None",
                "hardware": "Hardware (RTS/CTS)",
                "software": "Software (XON/XOFF)"
            }
            if flow_control in flow_control_map:
                self.flow_control.setCurrentText(flow_control_map[flow_control])
            
            advanced_config = config.get("advanced", {})
            timeout = advanced_config.get("timeout", 1000)
            if 100 <= timeout <= 10000:
                self.timeout.setValue(timeout)
            
            debug = advanced_config.get("debug", True)
            self.debug_mode.setChecked(debug)
            
            self.log_message(f"Configuration imported from {filename}")
            QMessageBox.information(self, "Success", f"Configuration imported from {filename}")
            
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to import configuration: {str(e)}")

def main():
    app = QApplication(sys.argv)
    window = ESP32ProtocolConfig()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()