#!/usr/bin/env python3
import sys
import time
import re
import json
from datetime import datetime
from PyQt5.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
    QLabel, QPushButton, QComboBox, QCheckBox, QLineEdit, 
    QTextEdit, QTabWidget, QGroupBox, QFormLayout, QSpinBox,
    QTableWidget, QTableWidgetItem, QHeaderView, QSplitter,
    QFileDialog, QMessageBox, QDialog, QGridLayout, QInputDialog,
    QDialogButtonBox, QFrame
)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QTimer, QSettings
from PyQt5.QtGui import QColor, QTextCharFormat, QFont, QIcon, QSyntaxHighlighter, QTextCursor
import serial
import serial.tools.list_ports

# Log level mapping
LOG_LEVELS = {
    "ERROR": 0,
    "WARN": 1, 
    "WARNING": 1,
    "INFO": 2,
    "DEBUG": 3,
    "VERB": 4,
    "VERBOSE": 4,
    "TASK": 5,
    "NONE": 6
}

# Colors for different log levels
LOG_COLORS = {
    "ERROR": QColor(255, 80, 80),
    "WARN": QColor(255, 180, 0),
    "WARNING": QColor(255, 180, 0),
    "INFO": QColor(0, 120, 215),
    "DEBUG": QColor(128, 0, 128),
    "VERB": QColor(0, 160, 0),
    "VERBOSE": QColor(0, 160, 0),
    "TASK": QColor(0, 160, 160),
    "NONE": QColor(128, 128, 128)
}

# CLI Commands
CLI_COMMANDS = {
    "wifi": {
        "description": "Configure WiFi settings",
        "usage": "wifi <ssid> <password>",
        "min_args": 2,
        "max_args": 2
    },
    "wifi_connect": {
        "description": "Connect to WiFi network",
        "usage": "wifi_connect <ssid> <password>",
        "min_args": 2,
        "max_args": 2
    },
    "restart": {
        "description": "Restart the system",
        "usage": "restart",
        "min_args": 0,
        "max_args": 0
    },
    "wifiscan_safe": {
        "description": "Scan for WiFi networks (safe mode)",
        "usage": "wifiscan_safe",
        "min_args": 0,
        "max_args": 0
    },
    "wifiread": {
        "description": "Read current WiFi status",
        "usage": "wifiread",
        "min_args": 0,
        "max_args": 0
    },
    "wifi_test": {
        "description": "Test WiFi connection",
        "usage": "wifi_test",
        "min_args": 0,
        "max_args": 0
    },
    "uuid_change": {
        "description": "Change Your Device Id unique address",
        "usage": "uuid_change <uuid>",
        "min_args": 1,
        "max_args": 1
    },
    "help": {
        "description": "Help command",
        "usage": "help",
        "min_args": 0,
        "max_args": 0
    },
    "login": {
        "description": "Login to CLI with password",
        "usage": "login <password>",
        "min_args": 1,
        "max_args": 1
    },
    "echo": {
        "description": "Toggle command echo",
        "usage": "echo [on|off]",
        "min_args": 0,
        "max_args": 1
    },
    "logout": {
        "description": "Logout from CLI",
        "usage": "logout",
        "min_args": 0,
        "max_args": 0
    }
}

class JsonSyntaxHighlighter(QSyntaxHighlighter):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.highlighting_rules = []

        # Define formats for different parts of JSON
        key_format = QTextCharFormat()
        key_format.setForeground(QColor(0, 120, 215))  # Blue for keys
        key_format.setFontWeight(QFont.Bold)

        string_format = QTextCharFormat()
        string_format.setForeground(QColor(0, 160, 0))  # Green for strings

        number_format = QTextCharFormat()
        number_format.setForeground(QColor(128, 0, 128))  # Purple for numbers

        boolean_format = QTextCharFormat()
        boolean_format.setForeground(QColor(255, 140, 0))  # Orange for booleans

        # Add rules for highlighting
        # Keys
        self.highlighting_rules.append((re.compile(r'"([^"]+)"\s*:'), key_format))
        
        # Strings
        self.highlighting_rules.append((re.compile(r':\s*"([^"]+)"'), string_format))
        
        # Numbers
        self.highlighting_rules.append((re.compile(r':\s*(-?\d+(\.\d+)?)'), number_format))
        
        # Booleans
        self.highlighting_rules.append((re.compile(r':\s*(true|false|null)'), boolean_format))

    def highlightBlock(self, text):
        for pattern, format in self.highlighting_rules:
            for match in pattern.finditer(text):
                self.setFormat(match.start(), match.end() - match.start(), format)

class LogEntry:
    def __init__(self, timestamp, level, message, raw=None):
        self.timestamp = timestamp
        self.level = level
        self.message = message
        self.raw = raw or f"[{timestamp}] [{level}] {message}"
    
    def __str__(self):
        return self.raw

class SerialReaderThread(QThread):
    log_received = pyqtSignal(str)
    connection_error = pyqtSignal(str)
    
    def __init__(self, port, baud_rate):
        super().__init__()
        self.port = port
        self.baud_rate = baud_rate
        self.running = False
        self.serial_port = None
    
    def run(self):
        try:
            self.serial_port = serial.Serial(
                port=self.port,
                baudrate=self.baud_rate,
                timeout=0.1
            )
            self.running = True
            
            buffer = ""
            while self.running:
                if self.serial_port.in_waiting:
                    data = self.serial_port.read(self.serial_port.in_waiting).decode('utf-8', errors='replace')
                    buffer += data
                    
                    # Process complete lines
                    # Handle both \n and \r\n line endings
                    lines = re.split(r'\r?\n', buffer)
                    buffer = lines.pop()  # Keep the last incomplete line in the buffer
                    
                    for line in lines:
                        if line.strip():  # Skip empty lines
                            self.log_received.emit(line.strip())
                
                # Small delay to prevent high CPU usage
                time.sleep(0.01)
                
        except serial.SerialException as e:
            self.connection_error.emit(f"Serial connection error: {str(e)}")
        finally:
            if self.serial_port and self.serial_port.is_open:
                self.serial_port.close()
    
    def stop(self):
        self.running = False
        self.wait()
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
    
    def send_command(self, command):
        if not self.running or not self.serial_port or not self.serial_port.is_open:
            raise Exception("Serial port not connected")
        
        # Ensure command ends with both carriage return and newline
        # This is important for the FMS CLI implementation
        if not command.endswith('\r\n'):
            if command.endswith('\n'):
                command = command[:-1] + '\r\n'
            elif command.endswith('\r'):
                command = command + '\n'
            else:
                command += '\r\n'
        
        self.serial_port.write(command.encode('utf-8'))
        self.serial_port.flush()
        
        # Small delay to ensure command is processed
        time.sleep(0.05)

class LogParser:
    @staticmethod
    def parse_log_line(line):
        # Try to match the format: [timestamp] [level] message
        pattern = r'\[(.*?)\]\s*\[(.*?)\]\s*(.*)'
        match = re.match(pattern, line)
        
        if match:
            timestamp = match.group(1)
            level = match.group(2).upper()
            message = match.group(3)
            
            # Normalize log level
            if level not in LOG_LEVELS:
                if "WARN" in level:
                    level = "WARN"
                elif "ERR" in level:
                    level = "ERROR"
                elif "INFO" in level:
                    level = "INFO"
                elif "DEBUG" in level:
                    level = "DEBUG"
                elif "VERB" in level:
                    level = "VERB"
                elif "TASK" in level:
                    level = "TASK"
                else:
                    level = "NONE"
            
            return LogEntry(timestamp, level, message, line)
        
        # Try to parse JSON response
        try:
            # Check if the line looks like JSON
            if line.strip().startswith('{') and line.strip().endswith('}'):
                data = json.loads(line)
                if isinstance(data, dict):
                    now = datetime.now().strftime("%H:%M:%S.%f")[:-3]
                    command = data.get("command", "unknown")
                    success = data.get("success", True)
                    result = data.get("result", "")
                    
                    # Format message based on JSON content
                    message = f"Command '{command}': {result}"
                    level = "INFO" if success else "ERROR"
                    
                    return LogEntry(now, level, message, line)
        except json.JSONDecodeError:
            # Not valid JSON, continue with other parsing
            pass
        except Exception as e:
            # Other parsing errors
            pass
        
        # Check for special patterns in the output
        if "interruptSection" in line:
            now = datetime.now().strftime("%H:%M:%S.%f")[:-3]
            return LogEntry(now, "DEBUG", f"Interrupt: {line}", line)
        
        # If no match, treat the whole line as a message
        now = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        return LogEntry(now, "NONE", line, line)

class WiFiConfigDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("WiFi Configuration")
        self.resize(400, 150)
        
        layout = QFormLayout()
        
        self.ssid_input = QLineEdit()
        self.password_input = QLineEdit()
        self.password_input.setEchoMode(QLineEdit.Password)
        
        layout.addRow("SSID:", self.ssid_input)
        layout.addRow("Password:", self.password_input)
        
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        
        layout.addRow(buttons)
        self.setLayout(layout)
    
    def get_values(self):
        return self.ssid_input.text(), self.password_input.text()

class UUIDChangeDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Change Device UUID")
        self.resize(400, 100)
        
        layout = QFormLayout()
        
        self.uuid_input = QLineEdit()
        
        layout.addRow("New UUID:", self.uuid_input)
        
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        
        layout.addRow(buttons)
        self.setLayout(layout)
    
    def get_value(self):
        return self.uuid_input.text()

class LoginDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Login to FMS CLI")
        self.resize(400, 100)
        
        layout = QFormLayout()
        
        self.password_input = QLineEdit()
        self.password_input.setEchoMode(QLineEdit.Password)
        
        layout.addRow("Password:", self.password_input)
        
        buttons = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel)
        buttons.accepted.connect(self.accept)
        buttons.rejected.connect(self.reject)
        
        layout.addRow(buttons)
        self.setLayout(layout)
    
    def get_password(self):
        return self.password_input.text()

class CommandDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("Send Command")
        self.resize(500, 400)
        
        layout = QVBoxLayout()
        
        # Command input
        input_layout = QHBoxLayout()
        self.command_input = QLineEdit()
        self.command_input.setPlaceholderText("Enter command...")
        self.send_button = QPushButton("Send")
        self.send_button.clicked.connect(self.accept)
        
        input_layout.addWidget(self.command_input)
        input_layout.addWidget(self.send_button)
        
        # Command list
        command_group = QGroupBox("Available Commands")
        command_layout = QVBoxLayout()
        
        self.command_table = QTableWidget(len(CLI_COMMANDS), 2)
        self.command_table.setHorizontalHeaderLabels(["Command", "Description"])
        self.command_table.horizontalHeader().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.command_table.horizontalHeader().setSectionResizeMode(1, QHeaderView.Stretch)
        self.command_table.setSelectionBehavior(QTableWidget.SelectRows)
        self.command_table.setEditTriggers(QTableWidget.NoEditTriggers)
        self.command_table.itemDoubleClicked.connect(self.command_selected)
        
        row = 0
        for cmd, info in CLI_COMMANDS.items():
            self.command_table.setItem(row, 0, QTableWidgetItem(cmd))
            self.command_table.setItem(row, 1, QTableWidgetItem(info["description"]))
            row += 1
        
        command_layout.addWidget(self.command_table)
        command_group.setLayout(command_layout)
        
        # Command details
        details_group = QGroupBox("Command Details")
        details_layout = QVBoxLayout()
        self.command_details = QTextEdit()
        self.command_details.setReadOnly(True)
        details_layout.addWidget(self.command_details)
        details_group.setLayout(details_layout)
        
        # Add to main layout
        layout.addLayout(input_layout)
        layout.addWidget(command_group)
        layout.addWidget(details_group)
        
        self.setLayout(layout)
        
        # Connect signals
        self.command_table.itemSelectionChanged.connect(self.update_command_details)
        self.command_input.returnPressed.connect(self.accept)
    
    def update_command_details(self):
        selected_items = self.command_table.selectedItems()
        if not selected_items:
            return
        
        row = selected_items[0].row()
        command = self.command_table.item(row, 0).text()
        
        if command in CLI_COMMANDS:
            info = CLI_COMMANDS[command]
            details = f"Command: {command}\n"
            details += f"Description: {info['description']}\n"
            details += f"Usage: {info['usage']}\n"
            details += f"Arguments: {info['min_args']} to {info['max_args']}"
            
            self.command_details.setText(details)
    
    def command_selected(self, item):
        row = item.row()
        command = self.command_table.item(row, 0).text()
        self.command_input.setText(command)
    
    def get_command(self):
        return self.command_input.text()

class FMSDebugUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("FMS Debug Logger")
        self.resize(1000, 700)
        
        # Initialize variables
        self.serial_thread = None
        self.logs = []
        self.current_filter_level = "INFO"
        self.filter_text = ""
        self.auto_scroll = True
        self.max_logs = 10000  # Maximum number of logs to keep
        self.command_history = []
        self.command_history_index = -1
        self.is_authenticated = False  # Authentication state
        self.all_logs_auto_scroll = True
        
        # Load settings
        self.settings = QSettings("FMSDebug", "LoggerUI")
        
        # Setup UI
        self.setup_ui()
        self.load_settings()
        self.refresh_ports()
        
        # Setup auto-refresh for ports
        self.port_refresh_timer = QTimer(self)
        self.port_refresh_timer.timeout.connect(self.refresh_ports)
        self.port_refresh_timer.start(5000)  # Refresh every 5 seconds
    
    def setup_ui(self):
        # Main widget and layout
        main_widget = QWidget()
        main_layout = QVBoxLayout()
        
        # Create tabs
        self.tabs = QTabWidget()
        
        # Log tab
        log_tab = QWidget()
        log_layout = QVBoxLayout()
        
        # Top controls for log tab
        top_layout = QHBoxLayout()
        
        # Connection group
        connection_group = QGroupBox("Connection")
        connection_layout = QFormLayout()
        
        self.port_combo = QComboBox()
        self.baud_combo = QComboBox()
        for rate in ["9600", "19200", "38400", "57600", "115200"]:
            self.baud_combo.addItem(rate)
        self.baud_combo.setCurrentText("115200")
        
        self.refresh_button = QPushButton("Refresh")
        self.refresh_button.clicked.connect(self.refresh_ports)
        
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        
        port_layout = QHBoxLayout()
        port_layout.addWidget(self.port_combo)
        port_layout.addWidget(self.refresh_button)
        
        connection_layout.addRow("Port:", port_layout)
        connection_layout.addRow("Baud Rate:", self.baud_combo)
        connection_layout.addRow("", self.connect_button)
        
        connection_group.setLayout(connection_layout)
        
        # Log settings group
        log_settings_group = QGroupBox("Log Settings")
        log_settings_layout = QFormLayout()
        
        self.log_level_combo = QComboBox()
        for level in ["ERROR", "WARN", "INFO", "DEBUG", "VERB", "TASK"]:
            self.log_level_combo.addItem(level)
        self.log_level_combo.setCurrentText("INFO")
        self.log_level_combo.currentTextChanged.connect(self.set_log_level)
        
        self.auto_scroll_check = QCheckBox("Auto-scroll")
        self.auto_scroll_check.setChecked(True)
        self.auto_scroll_check.stateChanged.connect(self.toggle_auto_scroll)
        
        self.filter_edit = QLineEdit()
        self.filter_edit.setPlaceholderText("Filter logs...")
        self.filter_edit.textChanged.connect(self.set_filter_text)
        
        log_settings_layout.addRow("Log Level:", self.log_level_combo)
        log_settings_layout.addRow("Filter:", self.filter_edit)
        log_settings_layout.addRow("", self.auto_scroll_check)
        
        log_settings_group.setLayout(log_settings_layout)
        
        # Actions group
        actions_group = QGroupBox("Actions")
        actions_layout = QVBoxLayout()
        
        self.clear_button = QPushButton("Clear Logs")
        self.clear_button.clicked.connect(self.clear_logs)
        
        self.save_button = QPushButton("Save Logs")
        self.save_button.clicked.connect(self.save_logs)
        
        self.command_button = QPushButton("Send Command")
        self.command_button.clicked.connect(self.show_command_dialog)
        
        actions_layout.addWidget(self.clear_button)
        actions_layout.addWidget(self.save_button)
        actions_layout.addWidget(self.command_button)
        
        actions_group.setLayout(actions_layout)
        
        # Add groups to top layout
        top_layout.addWidget(connection_group)
        top_layout.addWidget(log_settings_group)
        top_layout.addWidget(actions_group)
        
        # Log display
        log_group = QGroupBox("Log Output")
        log_layout_inner = QVBoxLayout()
        
        # Create a splitter for resizable panels
        splitter = QSplitter(Qt.Vertical)
        
        # Log table
        self.log_table = QTableWidget(0, 3)
        self.log_table.setHorizontalHeaderLabels(["Timestamp", "Level", "Message"])
        self.log_table.horizontalHeader().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.log_table.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.log_table.horizontalHeader().setSectionResizeMode(2, QHeaderView.Stretch)
        self.log_table.setSelectionBehavior(QTableWidget.SelectRows)
        self.log_table.setEditTriggers(QTableWidget.NoEditTriggers)
        
        # Log details
        self.log_details = QTextEdit()
        self.log_details.setReadOnly(True)
        self.log_details.setFont(QFont("Monospace", 9))
        
        # Add widgets to splitter
        splitter.addWidget(self.log_table)
        splitter.addWidget(self.log_details)
        splitter.setSizes([500, 200])
        
        log_layout_inner.addWidget(splitter)
        log_group.setLayout(log_layout_inner)
        
        # Add everything to log tab layout
        log_layout.addLayout(top_layout)
        log_layout.addWidget(log_group)
        log_tab.setLayout(log_layout)
        
        # Commands tab
        commands_tab = QWidget()
        commands_layout = QVBoxLayout()
        
        # Quick commands section
        quick_commands_group = QGroupBox("Quick Commands")
        quick_commands_layout = QGridLayout()
        
        # WiFi commands
        wifi_config_button = QPushButton("WiFi Config")
        wifi_config_button.clicked.connect(self.show_wifi_config_dialog)
        
        wifi_connect_button = QPushButton("WiFi Connect")
        wifi_connect_button.clicked.connect(self.show_wifi_connect_dialog)
        
        wifi_scan_button = QPushButton("WiFi Scan")
        wifi_scan_button.clicked.connect(lambda: self.send_cli_command("wifiscan_safe"))
        
        wifi_status_button = QPushButton("WiFi Status")
        wifi_status_button.clicked.connect(lambda: self.send_cli_command("wifiread"))
        
        wifi_test_button = QPushButton("WiFi Test")
        wifi_test_button.clicked.connect(lambda: self.send_cli_command("wifi_test"))
        
        # System commands
        restart_button = QPushButton("Restart System")
        restart_button.clicked.connect(self.confirm_restart)
        
        uuid_button = QPushButton("Change UUID")
        uuid_button.clicked.connect(self.show_uuid_dialog)
        
        help_button = QPushButton("Help")
        help_button.clicked.connect(lambda: self.send_cli_command("help"))
        
        login_button = QPushButton("Login")
        login_button.clicked.connect(self.show_login_dialog)
        
        echo_on_button = QPushButton("Echo On")
        echo_on_button.clicked.connect(lambda: self.send_cli_command("echo on"))
        
        echo_off_button = QPushButton("Echo Off")
        echo_off_button.clicked.connect(lambda: self.send_cli_command("echo off"))
        
        # Add buttons to grid
        quick_commands_layout.addWidget(QLabel("<b>WiFi Commands</b>"), 0, 0, 1, 2)
        quick_commands_layout.addWidget(wifi_config_button, 1, 0)
        quick_commands_layout.addWidget(wifi_connect_button, 1, 1)
        quick_commands_layout.addWidget(wifi_scan_button, 2, 0)
        quick_commands_layout.addWidget(wifi_status_button, 2, 1)
        quick_commands_layout.addWidget(wifi_test_button, 3, 0)
        
        quick_commands_layout.addWidget(QLabel("<b>System Commands</b>"), 4, 0, 1, 2)
        quick_commands_layout.addWidget(restart_button, 5, 0)
        quick_commands_layout.addWidget(uuid_button, 5, 1)
        quick_commands_layout.addWidget(help_button, 6, 0)
        quick_commands_layout.addWidget(login_button, 6, 1)
        quick_commands_layout.addWidget(echo_on_button, 7, 0)
        quick_commands_layout.addWidget(echo_off_button, 7, 1)
        
        quick_commands_group.setLayout(quick_commands_layout)
        
        # Custom command section
        custom_command_group = QGroupBox("Custom Command")
        custom_command_layout = QHBoxLayout()
        
        self.custom_command_input = QLineEdit()
        self.custom_command_input.setPlaceholderText("Enter command...")
        self.custom_command_input.returnPressed.connect(self.send_custom_command)
        
        # Add command history navigation
        self.custom_command_input.installEventFilter(self)
        
        send_custom_button = QPushButton("Send")
        send_custom_button.clicked.connect(self.send_custom_command)
        
        custom_command_layout.addWidget(self.custom_command_input)
        custom_command_layout.addWidget(send_custom_button)
        
        custom_command_group.setLayout(custom_command_layout)
        
        # Command response section
        response_group = QGroupBox("Command Response")
        response_layout = QVBoxLayout()
        
        self.command_response = QTextEdit()
        self.command_response.setReadOnly(True)
        self.command_response.setFont(QFont("Monospace", 9))
        
        # Add syntax highlighting for JSON responses
        self.json_highlighter = JsonSyntaxHighlighter(self.command_response.document())
        
        response_layout.addWidget(self.command_response)
        response_group.setLayout(response_layout)
        
        # Command history section
        history_group = QGroupBox("Command History")
        history_layout = QVBoxLayout()
        
        self.history_list = QTableWidget(0, 2)
        self.history_list.setHorizontalHeaderLabels(["Command", "Time"])
        self.history_list.horizontalHeader().setSectionResizeMode(0, QHeaderView.Stretch)
        self.history_list.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.history_list.setSelectionBehavior(QTableWidget.SelectRows)
        self.history_list.setEditTriggers(QTableWidget.NoEditTriggers)
        self.history_list.itemDoubleClicked.connect(self.use_history_command)
        
        clear_history_button = QPushButton("Clear History")
        clear_history_button.clicked.connect(self.clear_command_history)
        
        history_layout.addWidget(self.history_list)
        history_layout.addWidget(clear_history_button)
        history_group.setLayout(history_layout)
        
        # Create a horizontal splitter for command response and history
        command_splitter = QSplitter(Qt.Horizontal)
        
        # Create containers for the splitter
        response_container = QWidget()
        response_container.setLayout(response_layout)
        
        history_container = QWidget()
        history_container.setLayout(history_layout)
        
        command_splitter.addWidget(response_container)
        command_splitter.addWidget(history_container)
        command_splitter.setSizes([600, 400])
        
        # Add everything to commands tab layout
        commands_layout.addWidget(quick_commands_group)
        commands_layout.addWidget(custom_command_group)
        commands_layout.addWidget(command_splitter)
        commands_tab.setLayout(commands_layout)
        
        # All Logs tab
        all_logs_tab = QWidget()
        all_logs_layout = QVBoxLayout()
        
        # All logs display
        all_logs_group = QGroupBox("Complete Log Output")
        all_logs_inner_layout = QVBoxLayout()
        
        # Create a splitter for resizable panels
        all_logs_splitter = QSplitter(Qt.Vertical)
        
        # All logs table
        self.all_logs_table = QTableWidget(0, 3)
        self.all_logs_table.setHorizontalHeaderLabels(["Timestamp", "Level", "Message"])
        self.all_logs_table.horizontalHeader().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.all_logs_table.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.all_logs_table.horizontalHeader().setSectionResizeMode(2, QHeaderView.Stretch)
        self.all_logs_table.setSelectionBehavior(QTableWidget.SelectRows)
        self.all_logs_table.setEditTriggers(QTableWidget.NoEditTriggers)
        
        # All logs details
        self.all_logs_details = QTextEdit()
        self.all_logs_details.setReadOnly(True)
        self.all_logs_details.setFont(QFont("Monospace", 9))
        
        # Add widgets to splitter
        all_logs_splitter.addWidget(self.all_logs_table)
        all_logs_splitter.addWidget(self.all_logs_details)
        all_logs_splitter.setSizes([500, 200])
        
        # Controls for all logs tab
        all_logs_controls_layout = QHBoxLayout()
        
        # Auto-scroll checkbox
        self.all_logs_auto_scroll_check = QCheckBox("Auto-scroll")
        self.all_logs_auto_scroll_check.setChecked(True)
        self.all_logs_auto_scroll_check.stateChanged.connect(self.toggle_all_logs_auto_scroll)
        
        # Clear button
        self.all_logs_clear_button = QPushButton("Clear All Logs")
        self.all_logs_clear_button.clicked.connect(self.clear_all_logs)
        
        # Save button
        self.all_logs_save_button = QPushButton("Save All Logs")
        self.all_logs_save_button.clicked.connect(self.save_all_logs)
        
        # Add controls to layout
        all_logs_controls_layout.addWidget(self.all_logs_auto_scroll_check)
        all_logs_controls_layout.addStretch()
        all_logs_controls_layout.addWidget(self.all_logs_clear_button)
        all_logs_controls_layout.addWidget(self.all_logs_save_button)
        
        all_logs_inner_layout.addLayout(all_logs_controls_layout)
        all_logs_inner_layout.addWidget(all_logs_splitter)
        all_logs_group.setLayout(all_logs_inner_layout)
        
        all_logs_layout.addWidget(all_logs_group)
        all_logs_tab.setLayout(all_logs_layout)
        
        # Connect signals for all logs tab
        self.all_logs_table.itemSelectionChanged.connect(self.show_all_logs_details)
        
        # find a device 


        # Add tabs to tab widget
        self.tabs.addTab(log_tab, "Logs")
        self.tabs.addTab(commands_tab, "Commands")
        self.tabs.addTab(all_logs_tab, "All Logs")
        self.tabs.addTab(,"Find Device")
        
        # Add tab widget to main layout
        main_layout.addWidget(self.tabs)
        
        # Status bar
        self.statusBar().showMessage("Ready")
        
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)
        
        # Connect signals
        self.log_table.itemSelectionChanged.connect(self.show_log_details)
    
    def eventFilter(self, obj, event):
        if obj is self.custom_command_input and event.type() == event.KeyPress:
            if event.key() == Qt.Key_Up:
                self.navigate_command_history(-1)
                return True
            elif event.key() == Qt.Key_Down:
                self.navigate_command_history(1)
                return True
        return super().eventFilter(obj, event)
    
    def navigate_command_history(self, direction):
        if not self.command_history:
            return
        
        # Update index based on direction
        self.command_history_index += direction
        
        # Ensure index is within bounds
        if self.command_history_index < 0:
            self.command_history_index = 0
        elif self.command_history_index >= len(self.command_history):
            self.command_history_index = len(self.command_history) - 1
        
        # Set the command from history
        self.custom_command_input.setText(self.command_history[self.command_history_index])
        
        # Move cursor to end of text
        self.custom_command_input.setCursorPosition(len(self.custom_command_input.text()))
    
    def add_to_command_history(self, command):
        # Don't add empty commands or duplicates of the last command
        if not command or (self.command_history and self.command_history[0] == command):
            return
        
        # Add to beginning of list
        self.command_history.insert(0, command)
        
        # Limit history size
        if len(self.command_history) > 50:
            self.command_history = self.command_history[:50]
        
        # Reset index
        self.command_history_index = -1
        
        # Update history display
        self.update_command_history_display()
    
    def update_command_history_display(self):
        self.history_list.setRowCount(0)
        
        for i, command in enumerate(self.command_history):
            row = self.history_list.rowCount()
            self.history_list.insertRow(row)
            
            # Command
            command_item = QTableWidgetItem(command)
            self.history_list.setItem(row, 0, command_item)
            
            # Time
            time_item = QTableWidgetItem(datetime.now().strftime("%H:%M:%S"))
            self.history_list.setItem(row, 1, time_item)
    
    def use_history_command(self, item):
        row = item.row()
        if 0 <= row < len(self.command_history):
            command = self.command_history[row]
            self.custom_command_input.setText(command)
            self.custom_command_input.setFocus()
    
    def clear_command_history(self):
        self.command_history = []
        self.command_history_index = -1
        self.history_list.setRowCount(0)
    
    def load_settings(self):
        # Load last used port and baud rate
        last_port = self.settings.value("last_port", "")
        last_baud = self.settings.value("last_baud", "115200")
        
        if last_baud:
            index = self.baud_combo.findText(last_baud)
            if index >= 0:
                self.baud_combo.setCurrentIndex(index)
        
        # We'll set the port after refreshing the port list
    
    def save_settings(self):
        self.settings.setValue("last_port", self.port_combo.currentText())
        self.settings.setValue("last_baud", self.baud_combo.currentText())
    
    def refresh_ports(self):
        current_port = self.port_combo.currentText()
        self.port_combo.clear()
        
        ports = [port.device for port in serial.tools.list_ports.comports()]
        for port in ports:
            self.port_combo.addItem(port)
        
        # Try to restore the previously selected port
        if current_port:
            index = self.port_combo.findText(current_port)
            if index >= 0:
                self.port_combo.setCurrentIndex(index)
        
        # If no port was selected or the previous port is gone, try to use the saved port
        if self.port_combo.currentText() == "" and ports:
            last_port = self.settings.value("last_port", "")
            index = self.port_combo.findText(last_port)
            if index >= 0:
                self.port_combo.setCurrentIndex(index)
            else:
                # Just select the first available port
                self.port_combo.setCurrentIndex(0)
    
    def toggle_connection(self):
        if self.serial_thread and self.serial_thread.running:
            self.disconnect_serial()
        else:
            self.connect_serial()
    
    def connect_serial(self):
        port = self.port_combo.currentText()
        baud_rate = int(self.baud_combo.currentText())
        
        if not port:
            QMessageBox.warning(self, "Connection Error", "No serial port selected")
            return
        
        self.serial_thread = SerialReaderThread(port, baud_rate)
        self.serial_thread.log_received.connect(self.process_log)
        self.serial_thread.connection_error.connect(self.handle_connection_error)
        self.serial_thread.start()
        
        self.connect_button.setText("Disconnect")
        self.statusBar().showMessage(f"Connected to {port} at {baud_rate} baud")
        
        # Save settings
        self.save_settings()
    
    def disconnect_serial(self):
        if self.serial_thread:
            self.serial_thread.stop()
            self.serial_thread = None
        
        self.connect_button.setText("Connect")
        self.statusBar().showMessage("Disconnected")
    
    def handle_connection_error(self, error_message):
        self.disconnect_serial()
        QMessageBox.critical(self, "Connection Error", error_message)
    
    def process_log(self, log_line):
        log_entry = LogParser.parse_log_line(log_line)
        self.logs.append(log_entry)
        
        # Check for authentication responses
        if "Login successful" in log_line:
            self.is_authenticated = True
            self.statusBar().showMessage("Authenticated")
        elif "Invalid password" in log_line:
            self.is_authenticated = False
            self.statusBar().showMessage("Authentication failed")
        
        # Limit the number of logs
        if len(self.logs) > self.max_logs:
            self.logs = self.logs[-self.max_logs:]
        
        # Update the display if the log passes the current filter
        if self.should_display_log(log_entry):
            self.add_log_to_table(log_entry)
        
        # Always add to the all logs table regardless of filters
        self.add_log_to_all_logs_table(log_entry)
        
        # Add to command response if it looks like a command response
        if "command" in log_line.lower() or "{" in log_line or "interruptSection" in log_line:
            self.command_response.append(log_line)
            self.command_response.append("\n")
            
            # Auto-scroll command response
            cursor = self.command_response.textCursor()
            cursor.movePosition(QTextCursor.End)
            self.command_response.setTextCursor(cursor)
            
            # Limit the size of the command response
            if self.command_response.document().blockCount() > 500:
                cursor = self.command_response.textCursor()
                cursor.movePosition(cursor.Start)
                cursor.movePosition(cursor.Down, cursor.KeepAnchor, 1)
                cursor.removeSelectedText()
    
    def should_display_log(self, log_entry):
        # Check log level filter
        if LOG_LEVELS.get(log_entry.level, 6) > LOG_LEVELS.get(self.current_filter_level, 2):
            return False
        
        # Check text filter
        if self.filter_text and self.filter_text.lower() not in log_entry.raw.lower():
            return False
        
        return True
    
    def add_log_to_table(self, log_entry):
        row = self.log_table.rowCount()
        self.log_table.insertRow(row)
        
        # Timestamp
        timestamp_item = QTableWidgetItem(log_entry.timestamp)
        self.log_table.setItem(row, 0, timestamp_item)
        
        # Level
        level_item = QTableWidgetItem(log_entry.level)
        level_item.setForeground(LOG_COLORS.get(log_entry.level, QColor(0, 0, 0)))
        self.log_table.setItem(row, 1, level_item)
        
        # Message
        message_item = QTableWidgetItem(log_entry.message)
        self.log_table.setItem(row, 2, message_item)
        
        # Auto-scroll
        if self.auto_scroll:
            self.log_table.scrollToBottom()
    
    def show_log_details(self):
        selected_rows = self.log_table.selectedItems()
        if not selected_rows:
            return
        
        row = selected_rows[0].row()
        if 0 <= row < len(self.logs):
            log_entry = self.logs[row]
            self.log_details.setText(log_entry.raw)
    
    def set_log_level(self, level):
        self.current_filter_level = level
        self.refresh_log_display()
    
    def set_filter_text(self, text):
        self.filter_text = text
        self.refresh_log_display()
    
    def toggle_auto_scroll(self, state):
        self.auto_scroll = state == Qt.Checked
    
    def refresh_log_display(self):
        self.log_table.setRowCount(0)
        for log_entry in self.logs:
            if self.should_display_log(log_entry):
                self.add_log_to_table(log_entry)
    
    def clear_logs(self):
        self.logs = []
        self.log_table.setRowCount(0)
        self.all_logs_table.setRowCount(0)
        self.log_details.clear()
        self.all_logs_details.clear()
    
    def save_logs(self):
        if not self.logs:
            QMessageBox.information(self, "Save Logs", "No logs to save")
            return
        
        filename, _ = QFileDialog.getSaveFileName(
            self, "Save Logs", "", "Text Files (*.txt);;CSV Files (*.csv);;All Files (*)"
        )
        
        if not filename:
            return
        
        try:
            with open(filename, 'w') as f:
                if filename.endswith('.csv'):
                    # CSV format
                    f.write("Timestamp,Level,Message\n")
                    for log in self.logs:
                        f.write(f'"{log.timestamp}","{log.level}","{log.message}"\n')
                else:
                    # Text format
                    for log in self.logs:
                        f.write(f"{log.raw}\n")
            
            QMessageBox.information(self, "Save Logs", f"Logs saved to {filename}")
        
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to save logs: {str(e)}")
    
    def show_command_dialog(self):
        dialog = CommandDialog(self)
        if dialog.exec_() == QDialog.Accepted:
            command = dialog.get_command()
            if command:
                self.send_cli_command(command)
    
    def send_custom_command(self):
        command = self.custom_command_input.text()
        if command:
            self.send_cli_command(command)
            self.add_to_command_history(command)
            self.custom_command_input.clear()
    
    def send_cli_command(self, command):
        if not self.serial_thread or not self.serial_thread.running:
            QMessageBox.warning(self, "Send Command", "Not connected to a serial port")
            return
        
        try:
            # Add command to response display
            self.command_response.append(f"> {command}\n")
            
            # Send command with proper line endings
            self.serial_thread.send_command(command)
            self.statusBar().showMessage(f"Command sent: {command}")
            
            # Switch to commands tab
            self.tabs.setCurrentIndex(1)
            
            # Add to history if not already there
            self.add_to_command_history(command)
            
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to send command: {str(e)}")
    
    def show_wifi_config_dialog(self):
        dialog = WiFiConfigDialog(self)
        if dialog.exec_() == QDialog.Accepted:
            ssid, password = dialog.get_values()
            if ssid and password:
                self.send_cli_command(f'wifi "{ssid}" "{password}"')
    
    def show_wifi_connect_dialog(self):
        dialog = WiFiConfigDialog(self)
        dialog.setWindowTitle("WiFi Connect")
        if dialog.exec_() == QDialog.Accepted:
            ssid, password = dialog.get_values()
            if ssid and password:
                self.send_cli_command(f'wifi_connect "{ssid}" "{password}"')
    
    def show_uuid_dialog(self):
        dialog = UUIDChangeDialog(self)
        if dialog.exec_() == QDialog.Accepted:
            uuid = dialog.get_value()
            if uuid:
                self.send_cli_command(f'uuid_change "{uuid}"')
    
    def show_login_dialog(self):
        dialog = LoginDialog(self)
        if dialog.exec_() == QDialog.Accepted:
            password = dialog.get_password()
            if password:
                self.send_cli_command(f'login {password}')
                # Authentication status will be updated based on response

    def confirm_restart(self):
        reply = QMessageBox.question(
            self, "Confirm Restart", 
            "Are you sure you want to restart the system?",
            QMessageBox.Yes | QMessageBox.No, 
            QMessageBox.No
        )
        
        if reply == QMessageBox.Yes:
            self.send_cli_command("restart")
    
    def closeEvent(self, event):
        # Disconnect serial port when closing
        self.disconnect_serial()
        self.save_settings()
        event.accept()

    def toggle_all_logs_auto_scroll(self, state):
        self.all_logs_auto_scroll = state == Qt.Checked
    
    def show_all_logs_details(self):
        selected_rows = self.all_logs_table.selectedItems()
        if not selected_rows:
            return
        
        row = selected_rows[0].row()
        if 0 <= row < len(self.logs):
            log_entry = self.logs[row]
            self.all_logs_details.setText(log_entry.raw)
    
    def clear_all_logs(self):
        self.logs = []
        self.log_table.setRowCount(0)
        self.all_logs_table.setRowCount(0)
        self.log_details.clear()
        self.all_logs_details.clear()
    
    def save_all_logs(self):
        if not self.logs:
            QMessageBox.information(self, "Save Logs", "No logs to save")
            return
        
        filename, _ = QFileDialog.getSaveFileName(
            self, "Save All Logs", "", "Text Files (*.txt);;CSV Files (*.csv);;All Files (*)"
        )
        
        if not filename:
            return
        
        try:
            with open(filename, 'w') as f:
                if filename.endswith('.csv'):
                    # CSV format
                    f.write("Timestamp,Level,Message\n")
                    for log in self.logs:
                        f.write(f'"{log.timestamp}","{log.level}","{log.message}"\n')
                else:
                    # Text format
                    for log in self.logs:
                        f.write(f"{log.raw}\n")
            
            QMessageBox.information(self, "Save Logs", f"All logs saved to {filename}")
        
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to save logs: {str(e)}")

    def add_log_to_all_logs_table(self, log_entry):
        row = self.all_logs_table.rowCount()
        self.all_logs_table.insertRow(row)
        
        # Timestamp
        timestamp_item = QTableWidgetItem(log_entry.timestamp)
        self.all_logs_table.setItem(row, 0, timestamp_item)
        
        # Level
        level_item = QTableWidgetItem(log_entry.level)
        level_item.setForeground(LOG_COLORS.get(log_entry.level, QColor(0, 0, 0)))
        self.all_logs_table.setItem(row, 1, level_item)
        
        # Message
        message_item = QTableWidgetItem(log_entry.message)
        self.all_logs_table.setItem(row, 2, message_item)
        
        # Auto-scroll
        if self.all_logs_auto_scroll:
            self.all_logs_table.scrollToBottom()

def main():
    app = QApplication(sys.argv)
    window = FMSDebugUI()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
