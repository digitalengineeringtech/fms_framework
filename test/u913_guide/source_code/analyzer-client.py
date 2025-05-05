#!/usr/bin/env python3
"""
Protocol Analyzer Client - Advanced desktop application for ESP32 Protocol Analyzer
This application provides a modern, feature-rich interface to control and monitor
the ESP32-based protocol analyzer (protocol-analyzer.ino).

Features:
- Modern UI with dark/light theme support
- Automated test sequences
- Test reporting and data export
- Real-time data visualization
- Device profiles and configuration management
"""

import sys
import os
import time
import json
import csv
import serial
import serial.tools.list_ports
from datetime import datetime
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
                            QLabel, QPushButton, QComboBox, QTextEdit, QGroupBox, 
                            QGridLayout, QLineEdit, QTabWidget, QSpinBox, QMessageBox,
                            QSplitter, QCheckBox, QFileDialog, QProgressBar, QTableWidget,
                            QTableWidgetItem, QHeaderView, QFrame, QRadioButton, QButtonGroup,
                            QAction, QMenu, QToolBar, QStatusBar, QDialog, QFormLayout,
                            QListWidget, QListWidgetItem, QStackedWidget, QScrollArea,
                            QSystemTrayIcon, QStyle, QDockWidget, QCalendarWidget, QTimeEdit)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QTimer, QSettings, QSize, QDateTime, QDate, QTime
from PyQt5.QtGui import QFont, QTextCursor, QColor, QPalette, QIcon, QPixmap, QKeySequence

# Constants
APP_NAME = "Protocol Analyzer Client"
APP_VERSION = "1.0.0"
CONFIG_FILE = "config.json"
DEFAULT_BAUD_RATE = 115200
DEFAULT_TIMEOUT = 1.0

class SerialThread(QThread):
    """Thread for handling serial communication without blocking the UI"""
    data_received = pyqtSignal(str)
    connection_error = pyqtSignal(str)
    connection_status = pyqtSignal(bool)
    tx_activity = pyqtSignal()
    rx_activity = pyqtSignal()

    def __init__(self, port=None, baudrate=DEFAULT_BAUD_RATE, timeout=DEFAULT_TIMEOUT):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.running = False
        self.serial_port = None
        self.buffer = ""

    def run(self):
        if not self.port:
            self.connection_error.emit("No port specified")
            return
            
        try:
            self.serial_port = serial.Serial(self.port, self.baudrate, timeout=self.timeout)
            self.running = True
            self.connection_status.emit(True)
            
            while self.running:
                if self.serial_port.in_waiting:
                    data = self.serial_port.readline().decode('utf-8', errors='replace')
                    if data:
                        self.rx_activity.emit()
                        self.data_received.emit(data)
                time.sleep(0.01)  # Small delay to prevent CPU hogging
                
        except serial.SerialException as e:
            self.connection_error.emit(f"Serial Error: {str(e)}")
            self.connection_status.emit(False)
            self.running = False
        except Exception as e:
            self.connection_error.emit(f"Error: {str(e)}")
            self.connection_status.emit(False)
            self.running = False
        finally:
            if self.serial_port and self.serial_port.is_open:
                self.serial_port.close()
            self.connection_status.emit(False)

    def send_command(self, command):
        if self.serial_port and self.serial_port.is_open:
            try:
                self.serial_port.write((command + '\n').encode('utf-8'))
                self.tx_activity.emit()
                return True
            except Exception as e:
                self.connection_error.emit(f"Send Error: {str(e)}")
                return False
        return False

    def stop(self):
        self.running = False
        self.wait()

class MatplotlibCanvas(FigureCanvas):
    """Canvas for displaying matplotlib plots in Qt"""
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        self.fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = self.fig.add_subplot(111)
        super(MatplotlibCanvas, self).__init__(self.fig)

class DeviceProfile:
    """Class to manage device profiles"""
    def __init__(self, name="Default", device_type="Generic", settings=None):
        self.name = name
        self.device_type = device_type
        self.settings = settings or {}
        
    def to_dict(self):
        return {
            "name": self.name,
            "device_type": self.device_type,
            "settings": self.settings
        }
        
    @classmethod
    def from_dict(cls, data):
        return cls(
            name=data.get("name", "Default"),
            device_type=data.get("device_type", "Generic"),
            settings=data.get("settings", {})
        )

class TestResult:
    """Class to store test results"""
    def __init__(self, test_name, protocol, result="PENDING", details="", timestamp=None):
        self.test_name = test_name
        self.protocol = protocol
        self.result = result
        self.details = details
        self.timestamp = timestamp or datetime.now()
        
    def to_dict(self):
        return {
            "test_name": self.test_name,
            "protocol": self.protocol,
            "result": self.result,
            "details": self.details,
            "timestamp": self.timestamp.isoformat()
        }
        
    @classmethod
    def from_dict(cls, data):
        return cls(
            test_name=data.get("test_name", "Unknown"),
            protocol=data.get("protocol", "Unknown"),
            result=data.get("result", "UNKNOWN"),
            details=data.get("details", ""),
            timestamp=datetime.fromisoformat(data.get("timestamp", datetime.now().isoformat()))
        )

class TestReport:
    """Class to manage test reports"""
    def __init__(self, name="Test Report", device_profile=None):
        self.name = name
        self.device_profile = device_profile or DeviceProfile()
        self.results = []
        self.start_time = datetime.now()
        self.end_time = None
        
    def add_result(self, result):
        self.results.append(result)
        
    def complete(self):
        self.end_time = datetime.now()
        
    def to_dict(self):
        return {
            "name": self.name,
            "device_profile": self.device_profile.to_dict(),
            "results": [r.to_dict() for r in self.results],
            "start_time": self.start_time.isoformat(),
            "end_time": self.end_time.isoformat() if self.end_time else None
        }
        
    @classmethod
    def from_dict(cls, data):
        report = cls(
            name=data.get("name", "Test Report"),
            device_profile=DeviceProfile.from_dict(data.get("device_profile", {}))
        )
        report.results = [TestResult.from_dict(r) for r in data.get("results", [])]
        report.start_time = datetime.fromisoformat(data.get("start_time", datetime.now().isoformat()))
        if data.get("end_time"):
            report.end_time = datetime.fromisoformat(data.get("end_time"))
        return report
        
    def export_csv(self, filename):
        with open(filename, 'w', newline='') as csvfile:
            fieldnames = ['test_name', 'protocol', 'result', 'details', 'timestamp']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            
            writer.writeheader()
            for result in self.results:
                writer.writerow({
                    'test_name': result.test_name,
                    'protocol': result.protocol,
                    'result': result.result,
                    'details': result.details,
                    'timestamp': result.timestamp.isoformat()
                })

class ProfileDialog(QDialog):
    """Dialog for creating/editing device profiles"""
    def __init__(self, profile=None, parent=None):
        super().__init__(parent)
        self.profile = profile or DeviceProfile()
        self.init_ui()
        
    def init_ui(self):
        self.setWindowTitle("Device Profile")
        self.setMinimumWidth(400)
        
        layout = QFormLayout()
        
        # Profile name
        self.name_input = QLineEdit(self.profile.name)
        layout.addRow("Profile Name:", self.name_input)
        
        # Device type
        self.type_combo = QComboBox()
        self.type_combo.addItems(["Generic", "U913", "RS485 Device", "RS232 Device", "Custom"])
        self.type_combo.setCurrentText(self.profile.device_type)
        layout.addRow("Device Type:", self.type_combo)
        
        # Settings section
        settings_group = QGroupBox("Device Settings")
        settings_layout = QFormLayout()
        
        # RS232 settings
        self.rs232_baud = QComboBox()
        self.rs232_baud.addItems(["2400", "4800", "9600", "19200", "38400", "57600", "115200"])
        self.rs232_baud.setCurrentText(str(self.profile.settings.get("rs232_baud", "2400")))
        settings_layout.addRow("RS232 Baud Rate:", self.rs232_baud)
        
        # RS485 settings
        self.rs485_baud = QComboBox()
        self.rs485_baud.addItems(["2400", "4800", "9600", "19200", "38400", "57600", "115200"])
        self.rs485_baud.setCurrentText(str(self.profile.settings.get("rs485_baud", "9600")))
        settings_layout.addRow("RS485 Baud Rate:", self.rs485_baud)
        
        # U913 settings
        self.u913_channel = QComboBox()
        self.u913_channel.addItems(["0", "1", "2", "3"])
        self.u913_channel.setCurrentText(str(self.profile.settings.get("u913_channel", "0")))
        settings_layout.addRow("U913 Channel:", self.u913_channel)
        
        # Custom settings
        self.custom_settings = QTextEdit()
        if "custom" in self.profile.settings:
            self.custom_settings.setText(json.dumps(self.profile.settings["custom"], indent=2))
        settings_layout.addRow("Custom Settings (JSON):", self.custom_settings)
        
        settings_group.setLayout(settings_layout)
        layout.addRow(settings_group)
        
        # Buttons
        button_layout = QHBoxLayout()
        save_button = QPushButton("Save")
        save_button.clicked.connect(self.save_profile)
        cancel_button = QPushButton("Cancel")
        cancel_button.clicked.connect(self.reject)
        
        button_layout.addWidget(save_button)
        button_layout.addWidget(cancel_button)
        layout.addRow(button_layout)
        
        self.setLayout(layout)
        
    def save_profile(self):
        self.profile.name = self.name_input.text()
        self.profile.device_type = self.type_combo.currentText()
        
        # Update settings
        self.profile.settings = {
            "rs232_baud": self.rs232_baud.currentText(),
            "rs485_baud": self.rs485_baud.currentText(),
            "u913_channel": self.u913_channel.currentText()
        }
        
        # Parse custom settings
        try:
            custom_text = self.custom_settings.toPlainText().strip()
            if custom_text:
                self.profile.settings["custom"] = json.loads(custom_text)
        except json.JSONDecodeError:
            QMessageBox.warning(self, "Invalid JSON", "Custom settings contain invalid JSON. Please correct and try again.")
            return
            
        self.accept()

class ScheduleDialog(QDialog):
    """Dialog for scheduling automated tests"""
    def __init__(self, parent=None):
        super().__init__(parent)
        self.init_ui()
        
    def init_ui(self):
        self.setWindowTitle("Schedule Test")
        self.setMinimumWidth(400)
        
        layout = QVBoxLayout()
        
        # Date selection
        date_group = QGroupBox("Test Date")
        date_layout = QVBoxLayout()
        
        self.calendar = QCalendarWidget()
        self.calendar.setMinimumDate(QDate.currentDate())
        date_layout.addWidget(self.calendar)
        
        date_group.setLayout(date_layout)
        layout.addWidget(date_group)
        
        # Time selection
        time_group = QGroupBox("Test Time")
        time_layout = QHBoxLayout()
        
        self.time_edit = QTimeEdit()
        self.time_edit.setTime(QTime.currentTime().addSecs(60))  # Default to 1 minute from now
        self.time_edit.setDisplayFormat("HH:mm:ss")
        time_layout.addWidget(self.time_edit)
        
        time_group.setLayout(time_layout)
        layout.addWidget(time_group)
        
        # Repeat options
        repeat_group = QGroupBox("Repeat")
        repeat_layout = QVBoxLayout()
        
        self.repeat_none = QRadioButton("No Repeat")
        self.repeat_daily = QRadioButton("Daily")
        self.repeat_weekly = QRadioButton("Weekly")
        self.repeat_none.setChecked(True)
        
        repeat_layout.addWidget(self.repeat_none)
        repeat_layout.addWidget(self.repeat_daily)
        repeat_layout.addWidget(self.repeat_weekly)
        
        repeat_group.setLayout(repeat_layout)
        layout.addWidget(repeat_group)
        
        # Buttons
        button_layout = QHBoxLayout()
        schedule_button = QPushButton("Schedule")
        schedule_button.clicked.connect(self.accept)
        cancel_button = QPushButton("Cancel")
        cancel_button.clicked.connect(self.reject)
        
        button_layout.addWidget(schedule_button)
        button_layout.addWidget(cancel_button)
        layout.addLayout(button_layout)
        
        self.setLayout(layout)
        
    def get_schedule_time(self):
        date = self.calendar.selectedDate()
        time = self.time_edit.time()
        return QDateTime(date, time)
        
    def get_repeat_type(self):
        if self.repeat_daily.isChecked():
            return "daily"
        elif self.repeat_weekly.isChecked():
            return "weekly"
        else:
            return "none"

class ProtocolAnalyzerClient(QMainWindow):
    """Main window for the Protocol Analyzer Client"""
    
    def __init__(self):
        super().__init__()
        self.serial_thread = None
        self.log_file = None
        self.logging_enabled = False
        self.test_results = []
        self.device_profiles = []
        self.current_profile = None
        self.current_report = None
        self.scheduled_tests = []
        
        # Load settings
        self.settings = QSettings("ProtocolAnalyzer", "Client")
        self.load_config()
        
        # Initialize UI
        self.init_ui()
        
        # Set up timers
        self.activity_timer = QTimer()
        self.activity_timer.timeout.connect(self.reset_activity_indicators)
        self.activity_timer.start(200)  # Reset indicators after 200ms
        
        self.schedule_timer = QTimer()
        self.schedule_timer.timeout.connect(self.check_scheduled_tests)
        self.schedule_timer.start(10000)  # Check every 10 seconds
        
    def init_ui(self):
        self.setWindowTitle(f"{APP_NAME} v{APP_VERSION}")
        self.setGeometry(100, 100, 1280, 800)
        
        # Set up the central widget with main layout
        central_widget = QWidget()
        main_layout = QVBoxLayout()
        central_widget.setLayout(main_layout)
        self.setCentralWidget(central_widget)
        
        # Create menu bar
        self.create_menu_bar()
        
        # Create toolbar
        self.create_toolbar()
        
        # Connection panel
        connection_panel = self.create_connection_panel()
        main_layout.addLayout(connection_panel)
        
        # Main content area with splitter
        main_splitter = QSplitter(Qt.Horizontal)
        
        # Left side: Control tabs
        self.tab_widget = QTabWidget()
        self.tab_widget.addTab(self.create_dashboard_tab(), "Dashboard")
        self.tab_widget.addTab(self.create_monitor_tab(), "Monitor")
        self.tab_widget.addTab(self.create_rs232_tab(), "RS232")
        self.tab_widget.addTab(self.create_rs485_tab(), "RS485/Modbus")
        self.tab_widget.addTab(self.create_u913_tab(), "U913 Device")
        self.tab_widget.addTab(self.create_test_suite_tab(), "Test Suite")
        self.tab_widget.addTab(self.create_reports_tab(), "Reports")
        main_splitter.addWidget(self.tab_widget)
        
        # Right side: Log display and visualization
        right_panel = QSplitter(Qt.Vertical)
        
        # Log display
        log_widget = self.create_log_widget()
        right_panel.addWidget(log_widget)
        
        # Visualization panel
        viz_widget = self.create_visualization_widget()
        right_panel.addWidget(viz_widget)
        
        # Set initial sizes for right panel (70% log, 30% visualization)
        right_panel.setSizes([700, 300])
        
        main_splitter.addWidget(right_panel)
        
        # Set initial sizes for main splitter (40% controls, 60% display)
        main_splitter.setSizes([500, 780])
        
        main_layout.addWidget(main_splitter)
        
        # Status bar
        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        
        # Connection status
        self.connection_status = QLabel("Disconnected")
        self.connection_status.setStyleSheet("color: red;")
        self.status_bar.addPermanentWidget(self.connection_status)
        
        # TX/RX indicators
        self.tx_indicator = QLabel("TX")
        self.tx_indicator.setStyleSheet("background-color: gray; color: white; padding: 2px 5px; border-radius: 3px;")
        self.status_bar.addPermanentWidget(self.tx_indicator)
        
        self.rx_indicator = QLabel("RX")
        self.rx_indicator.setStyleSheet("background-color: gray; color: white; padding: 2px 5px; border-radius: 3px;")
        self.status_bar.addPermanentWidget(self.rx_indicator)
        
        # Apply theme
        self.apply_theme()
        
    def create_menu_bar(self):
        menubar = self.menuBar()
        
        # File menu
        file_menu = menubar.addMenu("&File")
        
        new_profile_action = QAction("New Device Profile", self)
        new_profile_action.setShortcut(QKeySequence.New)
        new_profile_action.triggered.connect(self.create_new_profile)
        file_menu.addAction(new_profile_action)
        
        load_profile_action = QAction("Load Profile", self)
        load_profile_action.triggered.connect(self.load_profile)
        file_menu.addAction(load_profile_action)
        
        save_profile_action = QAction("Save Profile", self)
        save_profile_action.setShortcut(QKeySequence.Save)
        save_profile_action.triggered.connect(self.save_profile)
        file_menu.addAction(save_profile_action)
        
        file_menu.addSeparator()
        
        export_results_action = QAction("Export Results", self)
        export_results_action.triggered.connect(self.export_test_results)
        file_menu.addAction(export_results_action)
        
        file_menu.addSeparator()
        
        exit_action = QAction("Exit", self)
        exit_action.setShortcut(QKeySequence.Quit)
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)
        
        # Connection menu
        connection_menu = menubar.addMenu("&Connection")
        
        connect_action = QAction("Connect", self)
        connect_action.triggered.connect(self.connect_device)
        connection_menu.addAction(connect_action)
        
        disconnect_action = QAction("Disconnect", self)
        disconnect_action.triggered.connect(self.disconnect_device)
        connection_menu.addAction(disconnect_action)
        
        connection_menu.addSeparator()
        
        refresh_ports_action = QAction("Refresh Ports", self)
        refresh_ports_action.triggered.connect(self.refresh_ports)
        connection_menu.addAction(refresh_ports_action)
        
        # Tests menu
        tests_menu = menubar.addMenu("&Tests")
        
        run_all_tests_action = QAction("Run All Tests", self)
        run_all_tests_action.triggered.connect(self.run_all_tests)
        tests_menu.addAction(run_all_tests_action)
        
        schedule_test_action = QAction("Schedule Test", self)
        schedule_test_action.triggered.connect(self.schedule_test)
        tests_menu.addAction(schedule_test_action)
        
        tests_menu.addSeparator()
        
        rs232_test_menu = tests_menu.addMenu("RS232 Tests")
        rs232_test_action = QAction("RS232 Connection Test", self)
        rs232_test_action.triggered.connect(self.run_rs232_test)
        rs232_test_menu.addAction(rs232_test_action)
        
        rs232_loopback_action = QAction("RS232 Loopback Test", self)
        rs232_loopback_action.triggered.connect(self.run_loopback_test)
        rs232_test_menu.addAction(rs232_loopback_action)
        
        rs485_test_menu = tests_menu.addMenu("RS485 Tests")
        rs485_test_action = QAction("RS485 Simple Test", self)
        rs485_test_action.triggered.connect(self.run_rs485_simple_test)
        rs485_test_menu.addAction(rs485_test_action)
        
        rs485_scan_action = QAction("Scan for Modbus Devices", self)
        rs485_scan_action.triggered.connect(self.run_rs485_scan)
        rs485_test_menu.addAction(rs485_scan_action)
        
        u913_test_menu = tests_menu.addMenu("U913 Tests")
        u913_test_action = QAction("U913 Device Test", self)
        u913_test_action.triggered.connect(self.run_u913_test)
        u913_test_menu.addAction(u913_test_action)
        
        # View menu
        view_menu = menubar.addMenu("&View")
        
        theme_menu = view_menu.addMenu("Theme")
        
        light_theme_action = QAction("Light", self)
        light_theme_action.triggered.connect(lambda: self.set_theme("light"))
        theme_menu.addAction(light_theme_action)
        
        dark_theme_action = QAction("Dark", self)
        dark_theme_action.triggered.connect(lambda: self.set_theme("dark"))
        theme_menu.addAction(dark_theme_action)
        
        system_theme_action = QAction("System", self)
        system_theme_action.triggered.connect(lambda: self.set_theme("system"))
        theme_menu.addAction(system_theme_action)
        
        view_menu.addSeparator()
        
        clear_log_action = QAction("Clear Log", self)
        clear_log_action.triggered.connect(self.clear_log)
        view_menu.addAction(clear_log_action)
        
        # Help menu
        help_menu = menubar.addMenu("&Help")
        
        about_action = QAction("About", self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)
        
    def create_toolbar(self):
        toolbar = QToolBar("Main Toolbar")
        toolbar.setIconSize(QSize(24, 24))
        self.addToolBar(toolbar)
        
        # Connect/Disconnect
        connect_action = QAction("Connect", self)
        connect_action.triggered.connect(self.connect_device)
        toolbar.addAction(connect_action)
        
        disconnect_action = QAction("Disconnect", self)
        disconnect_action.triggered.connect(self.disconnect_device)
        toolbar.addAction(disconnect_action)
        
        toolbar.addSeparator()
        
        # Run tests
        run_tests_action = QAction("Run Tests", self)
        run_tests_action.triggered.connect(self.run_all_tests)
        toolbar.addAction(run_tests_action)
        
        # Clear log
        clear_log_action = QAction("Clear Log", self)
        clear_log_action.triggered.connect(self.clear_log)
        toolbar.addAction(clear_log_action)
        
        toolbar.addSeparator()
        
        # Profile selector
        toolbar.addWidget(QLabel("Profile:"))
        self.profile_combo = QComboBox()
        self.profile_combo.setMinimumWidth(150)
        self.update_profile_list()
        self.profile_combo.currentIndexChanged.connect(self.profile_selected)
        toolbar.addWidget(self.profile_combo)
        
    def create_connection_panel(self):
        """Create the connection panel"""
        layout = QHBoxLayout()
        
        # Port selection
        layout.addWidget(QLabel("Port:"))
        self.port_combo = QComboBox()
        self.refresh_ports()
        layout.addWidget(self.port_combo)
        
        # Baud rate selection
        layout.addWidget(QLabel("Baud Rate:"))
        self.baud_combo = QComboBox()
        self.baud_combo.addItems(["9600", "19200", "38400", "57600", "115200"])
        self.baud_combo.setCurrentText("115200")  # Default for ESP32 monitor
        layout.addWidget(self.baud_combo)
        
        # Connect/Disconnect button
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        layout.addWidget(self.connect_button)
        
        # Refresh ports button
        refresh_button = QPushButton("Refresh Ports")
        refresh_button.clicked.connect(self.refresh_ports)
        layout.addWidget(refresh_button)
        
        # Add stretch to push everything to the left
        layout.addStretch()
        
        # Protocol status indicators
        for protocol in ["RS232", "RS485", "U913"]:
            status_frame = QFrame()
            status_frame.setFrameShape(QFrame.Box)
            status_frame.setFixedSize(80, 30)
            status_frame.setStyleSheet("background-color: gray; color: white; border-radius: 3px;")
            
            status_layout = QHBoxLayout(status_frame)
            status_layout.setContentsMargins(5, 2, 5, 2)
            
            status_label = QLabel(protocol)
            status_label.setAlignment(Qt.AlignCenter)
            status_layout.addWidget(status_label)
            
            layout.addWidget(status_frame)
            setattr(self, f"{protocol.lower()}_status_frame", status_frame)
        
        return layout
        
    def create_log_widget(self):
        """Create the log display widget"""
        group = QGroupBox("Communication Log")
        layout = QVBoxLayout()
        
        # Log controls
        controls_layout = QHBoxLayout()
        
        clear_button = QPushButton("Clear Log")
        clear_button.clicked.connect(self.clear_log)
        controls_layout.addWidget(clear_button)
        
        self.auto_scroll_check = QCheckBox("Auto-scroll")
        self.auto_scroll_check.setChecked(True)
        controls_layout.addWidget(self.auto_scroll_check)
        
        self.log_checkbox = QCheckBox("Log to File")
        self.log_checkbox.toggled.connect(self.toggle_logging)
        controls_layout.addWidget(self.log_checkbox)
        
        # Filter options
        controls_layout.addWidget(QLabel("Filter:"))
        self.filter_combo = QComboBox()
        self.filter_combo.addItems(["All", "TX Only", "RX Only", "RS232", "RS485", "U913"])
        self.filter_combo.currentIndexChanged.connect(self.apply_log_filter)
        controls_layout.addWidget(self.filter_combo)
        
        # Search
        controls_layout.addWidget(QLabel("Search:"))
        self.search_input = QLineEdit()
        self.search_input.setPlaceholderText("Search log...")
        self.search_input.returnPressed.connect(self.search_log)
        controls_layout.addWidget(self.search_input)
        
        search_button = QPushButton("Search")
        search_button.clicked.connect(self.search_log)
        controls_layout.addWidget(search_button)
        
        layout.addLayout(controls_layout)
        
        # Log display
        self.log_display = QTextEdit()
        self.log_display.setReadOnly(True)
        self.log_display.setFont(QFont("Courier New", 12))
        layout.addWidget(self.log_display)
        
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
        
        layout.addLayout(cmd_layout)
        
        group.setLayout(layout)
        return group
        
    def create_visualization_widget(self):
        """Create the visualization widget"""
        group = QGroupBox("Data Visualization")
        layout = QVBoxLayout()
        
        # Visualization controls
        controls_layout = QHBoxLayout()
        
        controls_layout.addWidget(QLabel("Visualization Type:"))
        self.viz_type_combo = QComboBox()
        self.viz_type_combo.addItems(["Time Series", "Histogram", "Protocol Distribution", "Test Results"])
        self.viz_type_combo.currentIndexChanged.connect(self.update_visualization)
        controls_layout.addWidget(self.viz_type_combo)
        
        refresh_viz_button = QPushButton("Refresh")
        refresh_viz_button.clicked.connect(self.update_visualization)
        controls_layout.addWidget(refresh_viz_button)
        
        layout.addLayout(controls_layout)
        
        # Matplotlib canvas for visualization
        self.viz_canvas = MatplotlibCanvas(self, width=5, height=4, dpi=100)
        layout.addWidget(self.viz_canvas)
        
        group.setLayout(layout)
        return group
        
    def create_dashboard_tab(self):
        """Create the dashboard tab"""
        tab = QWidget()
        layout = QVBoxLayout()
        
        # Welcome message
        welcome_label = QLabel(f"Welcome to {APP_NAME} v{APP_VERSION}")
        welcome_label.setStyleSheet("font-size: 18pt; font-weight: bold;")
        welcome_label.setAlignment(Qt.AlignCenter)
        layout.addWidget(welcome_label)
        
        # Quick stats
        stats_group = QGroupBox("System Status")
        stats_layout = QGridLayout()
        
        # Connection status
        stats_layout.addWidget(QLabel("Connection:"), 0, 0)
        self.dashboard_conn_status = QLabel("Disconnected")
        self.dashboard_conn_status.setStyleSheet("color: red; font-weight: bold;")
        stats_layout.addWidget(self.dashboard_conn_status, 0, 1)
        
        # Protocol statuses
        for i, protocol in enumerate(["RS232", "RS485", "U913"]):
            stats_layout.addWidget(QLabel(f"{protocol} Status:"), i+1, 0)
            status_label = QLabel("Not Tested")
            status_label.setStyleSheet("color: gray; font-weight: bold;")
            stats_layout.addWidget(status_label, i+1, 1)
            setattr(self, f"dashboard_{protocol.lower()}_status", status_label)
        
        # Test statistics
        stats_layout.addWidget(QLabel("Tests Run:"), 4, 0)
        self.dashboard_tests_run = QLabel("0")
        stats_layout.addWidget(self.dashboard_tests_run, 4, 1)
        
        stats_layout.addWidget(QLabel("Tests Passed:"), 5, 0)
        self.dashboard_tests_passed = QLabel("0")
        stats_layout.addWidget(self.dashboard_tests_passed, 5, 1)
        
        stats_layout.addWidget(QLabel("Tests Failed:"), 6, 0)
        self.dashboard_tests_failed = QLabel("0")
        stats_layout.addWidget(self.dashboard_tests_failed, 6, 1)
        
        stats_group.setLayout(stats_layout)
        layout.addWidget(stats_group)
        
        # Quick actions
        actions_group = QGroupBox("Quick Actions")
        actions_layout = QGridLayout()
        
        # Connection actions
        connect_button = QPushButton("Connect")
        connect_button.clicked.connect(self.connect_device)
        actions_layout.addWidget(connect_button, 0, 0)
        
        disconnect_button = QPushButton("Disconnect")
        disconnect_button.clicked.connect(self.disconnect_device)
        actions_layout.addWidget(disconnect_button, 0, 1)
        
        # Test actions
        run_tests_button = QPushButton("Run All Tests")
        run_tests_button.clicked.connect(self.run_all_tests)
        actions_layout.addWidget(run_tests_button, 1, 0)
        
        clear_log_button = QPushButton("Clear Log")
        clear_log_button.clicked.connect(self.clear_log)
        actions_layout.addWidget(clear_log_button, 1, 1)
        
        # Protocol specific tests
        rs232_test_button = QPushButton("Test RS232")
        rs232_test_button.clicked.connect(self.run_rs232_test)
        actions_layout.addWidget(rs232_test_button, 2, 0)
        
        rs485_test_button = QPushButton("Test RS485")
        rs485_test_button.clicked.connect(self.run_rs485_simple_test)
        actions_layout.addWidget(rs485_test_button, 2, 1)
        
        u913_test_button = QPushButton("Test U913")
        u913_test_button.clicked.connect(self.run_u913_test)
        actions_layout.addWidget(u913_test_button, 3, 0)
        
        export_button = QPushButton("Export Results")
        export_button.clicked.connect(self.export_test_results)
        actions_layout.addWidget(export_button, 3, 1)
        
        actions_group.setLayout(actions_layout)
        layout.addWidget(actions_group)
        
        # Recent activity
        activity_group = QGroupBox("Recent Activity")
        activity_layout = QVBoxLayout()
        
        self.activity_list = QListWidget()
        activity_layout.addWidget(self.activity_list)
        
        activity_group.setLayout(activity_layout)
        layout.addWidget(activity_group)
        
        tab.setLayout(layout)
        return tab
        
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
    
    def create_reports_tab(self):
        """Create the reports tab"""
        tab = QWidget()
        layout = QVBoxLayout()
        
        # Reports list
        reports_group = QGroupBox("Test Reports")
        reports_layout = QVBoxLayout()
        
        self.reports_list = QListWidget()
        self.reports_list.itemClicked.connect(self.show_report)
        reports_layout.addWidget(self.reports_list)
        
        reports_group.setLayout(reports_layout)
        layout.addWidget(reports_group)
        
        # Report details
        details_group = QGroupBox("Report Details")
        details_layout = QVBoxLayout()
        
        self.report_details = QTextEdit()
        self.report_details.setReadOnly(True)
        details_layout.addWidget(self.report_details)
        
        # Export buttons
        export_layout = QHBoxLayout()
        
        export_csv_button = QPushButton("Export to CSV")
        export_csv_button.clicked.connect(self.export_report_csv)
        export_layout.addWidget(export_csv_button)
        
        export_pdf_button = QPushButton("Export to PDF")
        export_pdf_button.clicked.connect(self.export_report_pdf)
        export_layout.addWidget(export_pdf_button)
        
        details_layout.addLayout(export_layout)
        
        details_group.setLayout(details_layout)
        layout.addWidget(details_group)
        
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
            self.disconnect_device()
        else:
            self.connect_device()
    
    def connect_device(self):
        """Connect to the device"""
        port = self.port_combo.currentText()
        baudrate = int(self.baud_combo.currentText())
        
        if port == "No ports available":
            QMessageBox.warning(self, "Connection Error", "No serial ports available")
            return
        
        # Stop any existing thread
        if self.serial_thread and self.serial_thread.running:
            self.serial_thread.stop()
        
        # Create and start new thread
        self.serial_thread = SerialThread(port, baudrate)
        self.serial_thread.data_received.connect(self.handle_received_data)
        self.serial_thread.connection_error.connect(self.handle_connection_error)
        self.serial_thread.connection_status.connect(self.update_connection_status)
        self.serial_thread.tx_activity.connect(self.show_tx_activity)
        self.serial_thread.rx_activity.connect(self.show_rx_activity)
        self.serial_thread.start()
        
        self.connect_button.setText("Disconnect")
        self.log_message(f"Connecting to {port} at {baudrate} baud...", "blue")
        
        # Add to activity list
        self.add_activity(f"Connected to {port} at {baudrate} baud")
    
    def disconnect_device(self):
        """Disconnect from the device"""
        if self.serial_thread and self.serial_thread.running:
            self.serial_thread.stop()
            self.serial_thread = None
            self.connect_button.setText("Connect")
            self.update_connection_status(False)
            self.log_message("Disconnected from device", "red")
            
            # Add to activity list
            self.add_activity("Disconnected from device")
    
    def update_connection_status(self, connected):
        """Update the connection status indicators"""
        if connected:
            self.connection_status.setText("Connected")
            self.connection_status.setStyleSheet("color: green;")
            self.dashboard_conn_status.setText("Connected")
            self.dashboard_conn_status.setStyleSheet("color: green; font-weight: bold;")
        else:
            self.connection_status.setText("Disconnected")
            self.connection_status.setStyleSheet("color: red;")
            self.dashboard_conn_status.setText("Disconnected")
            self.dashboard_conn_status.setStyleSheet("color: red; font-weight: bold;")
    
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
        self.update_connection_status(False)
        QMessageBox.critical(self, "Connection Error", error_msg)
        
        # Add to activity list
        self.add_activity(f"Connection Error: {error_msg}")
    
    def send_command(self, command):
        """Send a command to the device"""
        if not self.serial_thread or not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return False
        
        success = self.serial_thread.send_command(command)
        if success:
            self.log_message(f"Sent: {command}", "blue")
            
        return success
    
    def show_tx_activity(self):
        """Show TX activity indicator"""
        self.tx_indicator.setStyleSheet("background-color: red; color: white; padding: 2px 5px; border-radius: 3px;")
    
    def show_rx_activity(self):
        """Show RX activity indicator"""
        self.rx_indicator.setStyleSheet("background-color: green; color: white; padding: 2px 5px; border-radius: 3px;")
    
    def reset_activity_indicators(self):
        """Reset the TX/RX activity indicators"""
        self.tx_indicator.setStyleSheet("background-color: gray; color: white; padding: 2px 5px; border-radius: 3px;")
        self.rx_indicator.setStyleSheet("background-color: gray; color: white; padding: 2px 5px; border-radius: 3px;")
    
    def send_manual_command(self):
        """Send a command from the command line input"""
        command = self.cmd_input.text().strip()
        if command:
            if self.send_command(command):
                self.cmd_input.clear()
                
                # Add to activity list
                self.add_activity(f"Sent command: {command}")
    
    def send_hex_data(self):
        """Send hex data to the device"""
        hex_data = self.hex_input.text().strip()
        if hex_data:
            self.send_command(f"SEND {hex_data}")
            
            # Add to activity list
            self.add_activity(f"Sent hex data: {hex_data}")
    
    def send_rs232_command(self):
        """Send a custom RS232 command"""
        cmd_bytes = self.rs232_cmd_input.text().strip()
        
        if not cmd_bytes:
            QMessageBox.warning(self, "Input Error", "Please enter command bytes")
            return
        
        self.send_command(f"SEND {cmd_bytes}")
        
        # Add to activity list
        self.add_activity(f"Sent RS232 command: {cmd_bytes}")
    
    def read_modbus_register(self):
        """Read a Modbus register"""
        slave_id = self.slave_id_input.value()
        register = self.register_input.text().strip()
        
        if not register:
            QMessageBox.warning(self, "Input Error", "Please enter a register address")
            return
        
        self.send_command(f"RS485READ {slave_id} {register}")
        
        # Add to activity list
        self.add_activity(f"Read Modbus register {register} from device {slave_id}")
    
    def write_modbus_register(self):
        """Write to a Modbus register"""
        slave_id = self.slave_id_input.value()
        register = self.register_input.text().strip()
        value = self.value_input.value()
        
        if not register:
            QMessageBox.warning(self, "Input Error", "Please enter a register address")
            return
        
        self.send_command(f"RS485WRITE {slave_id} {register} {value}")
        
        # Add to activity list
        self.add_activity(f"Wrote value {value} to Modbus register {register} on device {slave_id}")
    
    def test_u913_on_channel(self):
        """Test U913 on the selected channel"""
        channel = self.u913_channel_combo.currentIndex()
        self.set_test_status("RUNNING")
        self.set_protocol_status("u913", "RUNNING")
        self.send_command(f"CHANNEL {channel}")
        time.sleep(0.2)  # Small delay
        self.send_command(f"U913")
        
        # Add to activity list
        self.add_activity(f"Testing U913 on channel {channel}")
    
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
        
        # Add to activity list
        self.add_activity(f"Sent price change command: {price}")
    
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
        
        # Add to activity list
        self.add_activity("Sent read shift total command")
    
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
        
        # Add to activity list
        self.add_activity("Sent clear shift total command")
    
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
        
        # Add to activity list
        self.add_activity("Sent stop pump command")
    
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
        
        # Add to activity list
        self.add_activity("Sent real-time data request")
    
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
                    
                    # Add to activity list
                    self.add_activity(f"Started logging to {file_path}")
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
                    
                    # Add to activity list
                    self.add_activity("Stopped logging to file")
                except Exception:
                    pass
                self.log_file = None
    
    def clear_log(self):
        """Clear the log display"""
        self.log_display.clear()
        # Also send CLEAR command to the device
        self.send_command("CLEAR")
        
        # Add to activity list
        self.add_activity("Cleared log")
    
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
    
    def apply_log_filter(self):
        """Apply filter to log display"""
        filter_type = self.filter_combo.currentText()
        
        # TODO: Implement log filtering based on filter_type
        self.log_message(f"Filter applied: {filter_type}", "blue")
    
    def search_log(self):
        """Search the log for the specified text"""
        search_text = self.search_input.text().strip()
        if not search_text:
            return
            
        # Find the text in the log
        cursor = self.log_display.textCursor()
        cursor.movePosition(QTextCursor.Start)
        self.log_display.setTextCursor(cursor)
        
        found = self.log_display.find(search_text)
        if not found:
            self.log_message(f"Search text not found: {search_text}", "red")
    
    def set_test_status(self, status, message=""):
        """Set the test status indicator"""
        if status == "PASS":
            self.status_label.setText("PASS")
            self.status_label.setStyleSheet("font-size: 24pt; font-weight: bold; background-color: #4CAF50; color: white; border-radius: 5px;")
            self.log_message(f"TEST PASSED: {message}", "green")
            # Play sound for pass
            QApplication.beep()
            
            # Update dashboard
            self.dashboard_tests_run.setText(str(int(self.dashboard_tests_run.text()) + 1))
            self.dashboard_tests_passed.setText(str(int(self.dashboard_tests_passed.text()) + 1))
            
            # Add to activity list
            self.add_activity(f"Test PASSED: {message}")
            
        elif status == "FAIL":
            self.status_label.setText("FAIL")
            self.status_label.setStyleSheet("font-size: 24pt; font-weight: bold; background-color: #F44336; color: white; border-radius: 5px;")
            self.log_message(f"TEST FAILED: {message}", "red")
            # Play sound for fail (two beeps)
            QApplication.beep()
            QTimer.singleShot(300, QApplication.beep)
            
            # Update dashboard
            self.dashboard_tests_run.setText(str(int(self.dashboard_tests_run.text()) + 1))
            self.dashboard_tests_failed.setText(str(int(self.dashboard_tests_failed.text()) + 1))
            
            # Add to activity list
            self.add_activity(f"Test FAILED: {message}")
            
        elif status == "RUNNING":
            self.status_label.setText("TESTING...")
            self.status_label.setStyleSheet("font-size: 24pt; font-weight: bold; background-color: #2196F3; color: white; border-radius: 5px;")
            
            # Add to activity list
            self.add_activity("Test started")
            
        else:
            self.status_label.setText("NOT TESTED")
            self.status_label.setStyleSheet("font-size: 24pt; font-weight: bold; color: gray;")
    
    def set_protocol_status(self, protocol, status):
        """Set the status for a specific protocol"""
        # Update status frame
        frame = getattr(self, f"{protocol}_status_frame")
        
        # Update dashboard label
        dashboard_label = getattr(self, f"dashboard_{protocol}_status")
        
        if status == "PASS":
            frame.setStyleSheet("background-color: #4CAF50; color: white; border-radius: 3px;")
            dashboard_label.setText("PASS")
            dashboard_label.setStyleSheet("color: green; font-weight: bold;")
        elif status == "FAIL":
            frame.setStyleSheet("background-color: #F44336; color: white; border-radius: 3px;")
            dashboard_label.setText("FAIL")
            dashboard_label.setStyleSheet("color: red; font-weight: bold;")
        elif status == "RUNNING":
            frame.setStyleSheet("background-color: #2196F3; color: white; border-radius: 3px;")
            dashboard_label.setText("Testing...")
            dashboard_label.setStyleSheet("color: blue; font-weight: bold;")
        else:
            frame.setStyleSheet("background-color: gray; color: white; border-radius: 3px;")
            dashboard_label.setText("Not Tested")
            dashboard_label.setStyleSheet("color: gray; font-weight: bold;")
    
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
        
        # Create a new test report
        self.current_report = TestReport(f"Test Report {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        if self.current_profile:
            self.current_report.device_profile = self.current_profile
        
        # Clear previous results
        self.results_table.setRowCount(0)
        
        # Run tests in sequence
        self.log_message("Starting Test Suite", "blue")
        self.test_progress.setValue(0)
        
        # Add to activity list
        self.add_activity("Started test suite")
        
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
            
            # Complete the report
            if self.current_report:
                self.current_report.complete()
                self.save_report(self.current_report)
                self.update_reports_list()
            
            # Add to activity list
            self.add_activity("Completed test suite")
    
    def add_test_result(self, test_name, protocol, result="PENDING"):
        """Add a test result to the results table"""
        timestamp = datetime.now()
        
        # Create test result object
        test_result = TestResult(
            test_name=test_name,
            protocol=protocol,
            result=result,
            timestamp=timestamp
        )
        
        # Add to current report if available
        if self.current_report:
            self.current_report.add_result(test_result)
        
        # Add to table
        row = self.results_table.rowCount()
        self.results_table.insertRow(row)
        self.results_table.setItem(row, 0, QTableWidgetItem(test_name))
        self.results_table.setItem(row, 1, QTableWidgetItem(protocol))
        self.results_table.setItem(row, 2, QTableWidgetItem(result))
        self.results_table.setItem(row, 3, QTableWidgetItem(timestamp.strftime("%Y-%m-%d %H:%M:%S")))
        
        # Color code result
        if result == "PASS":
            self.results_table.item(row, 2).setBackground(QColor("#4CAF50"))
            self.results_table.item(row, 2).setForeground(QColor("white"))
        elif result == "FAIL":
            self.results_table.item(row, 2).setBackground(QColor("#F44336"))
            self.results_table.item(row, 2).setForeground(QColor("white"))
    
    def export_test_results(self):
        """Export test results to CSV file"""
        if self.results_table.rowCount() == 0:
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
                    for row in range(self.results_table.rowCount()):
                        writer.writerow({
                            'test': self.results_table.item(row, 0).text(),
                            'protocol': self.results_table.item(row, 1).text(),
                            'result': self.results_table.item(row, 2).text(),
                            'time': self.results_table.item(row, 3).text()
                        })
                
                self.log_message(f"Test results exported to {file_path}", "green")
                
                # Add to activity list
                self.add_activity(f"Exported test results to {file_path}")
            except Exception as e:
                self.log_message(f"Error exporting results: {str(e)}", "red")
                QMessageBox.critical(self, "Export Error", f"Error exporting results: {str(e)}")
    
    def run_all_tests(self):
        """Run all tests"""
        # Select all tests
        self.select_all_tests()
        
        # Run test suite
        self.run_test_suite()
    
    def add_activity(self, activity):
        """Add an activity to the activity list"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        item = QListWidgetItem(f"[{timestamp}] {activity}")
        self.activity_list.insertItem(0, item)  # Add at the top
        
        # Limit to 100 items
        while self.activity_list.count() > 100:
            self.activity_list.takeItem(self.activity_list.count() - 1)
    
    def update_visualization(self):
        """Update the visualization based on the selected type"""
        viz_type = self.viz_type_combo.currentText()
        
        # Clear the current plot
        self.viz_canvas.axes.clear()
        
        if viz_type == "Time Series":
            self.plot_time_series()
        elif viz_type == "Histogram":
            self.plot_histogram()
        elif viz_type == "Protocol Distribution":
            self.plot_protocol_distribution()
        elif viz_type == "Test Results":
            self.plot_test_results()
        
        self.viz_canvas.draw()
    
    def plot_time_series(self):
        """Plot time series data"""
        # Example time series data
        times = [1, 2, 3, 4, 5]
        values = [5, 7, 2, 8, 4]
        
        self.viz_canvas.axes.plot(times, values, 'b-')
        self.viz_canvas.axes.set_title('Time Series Data')
        self.viz_canvas.axes.set_xlabel('Time')
        self.viz_canvas.axes.set_ylabel('Value')
        self.viz_canvas.axes.grid(True)
    
    def plot_histogram(self):
        """Plot histogram data"""
        # Example histogram data
        data = [1, 1, 2, 2, 2, 3, 3, 4, 5, 5, 5, 5]
        
        self.viz_canvas.axes.hist(data, bins=5, color='green', alpha=0.7)
        self.viz_canvas.axes.set_title('Data Histogram')
        self.viz_canvas.axes.set_xlabel('Value')
        self.viz_canvas.axes.set_ylabel('Frequency')
        self.viz_canvas.axes.grid(True)
    
    def plot_protocol_distribution(self):
        """Plot protocol distribution"""
        # Example protocol distribution
        protocols = ['RS232', 'RS485', 'U913']
        counts = [15, 8, 12]
        
        self.viz_canvas.axes.bar(protocols, counts, color=['blue', 'green', 'red'])
        self.viz_canvas.axes.set_title('Protocol Distribution')
        self.viz_canvas.axes.set_xlabel('Protocol')
        self.viz_canvas.axes.set_ylabel('Count')
        
        # Add count labels on top of bars
        for i, count in enumerate(counts):
            self.viz_canvas.axes.text(i, count + 0.1, str(count), ha='center')
    
    def plot_test_results(self):
        """Plot test results"""
        # Count test results from the table
        pass_count = 0
        fail_count = 0
        pending_count = 0
        
        for row in range(self.results_table.rowCount()):
            result = self.results_table.item(row, 2).text()
            if result == "PASS":
                pass_count += 1
            elif result == "FAIL":
                fail_count += 1
            else:
                pending_count += 1
        
        # Create pie chart
        labels = ['Pass', 'Fail', 'Pending']
        sizes = [pass_count, fail_count, pending_count]
        colors = ['#4CAF50', '#F44336', '#FFC107']
        explode = (0.1, 0, 0)  # Explode the first slice (Pass)
        
        # Only plot non-zero values
        plot_labels = []
        plot_sizes = []
        plot_colors = []
        plot_explode = []
        
        for i, size in enumerate(sizes):
            if size > 0:
                plot_labels.append(labels[i])
                plot_sizes.append(size)
                plot_colors.append(colors[i])
                plot_explode.append(explode[i] if i < len(explode) else 0)
        
        if sum(plot_sizes) > 0:
            self.viz_canvas.axes.pie(
                plot_sizes, 
                explode=plot_explode, 
                labels=plot_labels, 
                colors=plot_colors,
                autopct='%1.1f%%', 
                shadow=True, 
                startangle=90
            )
            self.viz_canvas.axes.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle
            self.viz_canvas.axes.set_title('Test Results')
        else:
            self.viz_canvas.axes.text(0.5, 0.5, "No test results available", 
                                     horizontalalignment='center',
                                     verticalalignment='center',
                                     transform=self.viz_canvas.axes.transAxes)
    
    def create_new_profile(self):
        """Create a new device profile"""
        dialog = ProfileDialog(parent=self)
        if dialog.exec_():
            self.current_profile = dialog.profile
            self.device_profiles.append(self.current_profile)
            self.update_profile_list()
            self.save_config()
            
            # Add to activity list
            self.add_activity(f"Created new profile: {self.current_profile.name}")
    
    def load_profile(self):
        """Load a device profile from file"""
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Load Profile", "", "JSON Files (*.json);;All Files (*)"
        )
        
        if file_path:
            try:
                with open(file_path, 'r') as f:
                    profile_data = json.load(f)
                
                profile = DeviceProfile.from_dict(profile_data)
                self.device_profiles.append(profile)
                self.current_profile = profile
                self.update_profile_list()
                self.save_config()
                
                self.log_message(f"Loaded profile: {profile.name}", "green")
                
                # Add to activity list
                self.add_activity(f"Loaded profile: {profile.name}")
            except Exception as e:
                self.log_message(f"Error loading profile: {str(e)}", "red")
                QMessageBox.critical(self, "Load Error", f"Error loading profile: {str(e)}")
    
    def save_profile(self):
        """Save the current device profile to file"""
        if not self.current_profile:
            QMessageBox.warning(self, "No Profile", "No current profile to save")
            return
            
        file_path, _ = QFileDialog.getSaveFileName(
            self, "Save Profile", "", "JSON Files (*.json);;All Files (*)"
        )
        
        if file_path:
            try:
                with open(file_path, 'w') as f:
                    json.dump(self.current_profile.to_dict(), f, indent=2)
                
                self.log_message(f"Saved profile to {file_path}", "green")
                
                # Add to activity list
                self.add_activity(f"Saved profile: {self.current_profile.name}")
            except Exception as e:
                self.log_message(f"Error saving profile: {str(e)}", "red")
                QMessageBox.critical(self, "Save Error", f"Error saving profile: {str(e)}")
    
    def update_profile_list(self):
        """Update the profile list in the UI"""
        self.profile_combo.clear()
        
        for profile in self.device_profiles:
            self.profile_combo.addItem(profile.name)
            
        # Select current profile if available
        if self.current_profile:
            index = self.profile_combo.findText(self.current_profile.name)
            if index >= 0:
                self.profile_combo.setCurrentIndex(index)
    
    def profile_selected(self, index):
        """Handle profile selection"""
        if index >= 0 and index < len(self.device_profiles):
            self.current_profile = self.device_profiles[index]
            self.log_message(f"Selected profile: {self.current_profile.name}", "blue")
            
            # Apply profile settings
            self.apply_profile_settings()
    
    def apply_profile_settings(self):
        """Apply the current profile settings"""
        if not self.current_profile:
            return
            
        # Apply settings based on device type
        if self.current_profile.device_type == "U913":
            # Set U913 channel
            channel = int(self.current_profile.settings.get("u913_channel", "0"))
            self.u913_channel_combo.setCurrentIndex(channel)
            self.test_u913_channel.setCurrentIndex(channel)
            
        # Apply baud rate settings
        rs232_baud = self.current_profile.settings.get("rs232_baud", "2400")
        rs485_baud = self.current_profile.settings.get("rs485_baud", "9600")
        
        # Log applied settings
        self.log_message(f"Applied profile settings: RS232 Baud={rs232_baud}, RS485 Baud={rs485_baud}", "blue")
    
    def save_report(self, report):
        """Save a test report"""
        # Create reports directory if it doesn't exist
        os.makedirs("reports", exist_ok=True)
        
        # Generate filename
        filename = f"reports/report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        try:
            with open(filename, 'w') as f:
                json.dump(report.to_dict(), f, indent=2)
            
            self.log_message(f"Saved report to {filename}", "green")
        except Exception as e:
            self.log_message(f"Error saving report: {str(e)}", "red")
    
    def update_reports_list(self):
        """Update the list of available reports"""
        self.reports_list.clear()
        
        # Check if reports directory exists
        if not os.path.exists("reports"):
            return
            
        # List report files
        report_files = [f for f in os.listdir("reports") if f.endswith(".json")]
        
        for file in sorted(report_files, reverse=True):
            self.reports_list.addItem(file)
    
    def show_report(self, item):
        """Show the selected report"""
        filename = item.text()
        filepath = os.path.join("reports", filename)
        
        try:
            with open(filepath, 'r') as f:
                report_data = json.load(f)
            
            report = TestReport.from_dict(report_data)
            
            # Display report details
            html = f"<h2>{report.name}</h2>"
            html += f"<p><b>Start Time:</b> {report.start_time}</p>"
            html += f"<p><b>End Time:</b> {report.end_time}</p>"
            html += f"<p><b>Device Profile:</b> {report.device_profile.name} ({report.device_profile.device_type})</p>"
            
            html += "<h3>Test Results</h3>"
            html += "<table border='1' cellpadding='5' style='border-collapse: collapse;'>"
            html += "<tr><th>Test</th><th>Protocol</th><th>Result</th><th>Time</th></tr>"
            
            for result in report.results:
                result_color = "#4CAF50" if result.result == "PASS" else "#F44336" if result.result == "FAIL" else "#FFC107"
                html += f"<tr>"
                html += f"<td>{result.test_name}</td>"
                html += f"<td>{result.protocol}</td>"
                html += f"<td style='background-color: {result_color}; color: white;'>{result.result}</td>"
                html += f"<td>{result.timestamp}</td>"
                html += f"</tr>"
            
            html += "</table>"
            
            self.report_details.setHtml(html)
            
        except Exception as e:
            self.log_message(f"Error loading report: {str(e)}", "red")
            self.report_details.setPlainText(f"Error loading report: {str(e)}")
    
    def export_report_csv(self):
        """Export the current report to CSV"""
        if not self.reports_list.currentItem():
            QMessageBox.warning(self, "No Report Selected", "Please select a report to export")
            return
            
        filename = self.reports_list.currentItem().text()
        filepath = os.path.join("reports", filename)
        
        try:
            with open(filepath, 'r') as f:
                report_data = json.load(f)
            
            report = TestReport.from_dict(report_data)
            
            # Get export filename
            export_path, _ = QFileDialog.getSaveFileName(
                self, "Export Report", "", "CSV Files (*.csv);;All Files (*)"
            )
            
            if export_path:
                report.export_csv(export_path)
                self.log_message(f"Exported report to {export_path}", "green")
                
                # Add to activity list
                self.add_activity(f"Exported report to CSV: {os.path.basename(export_path)}")
                
        except Exception as e:
            self.log_message(f"Error exporting report: {str(e)}", "red")
            QMessageBox.critical(self, "Export Error", f"Error exporting report: {str(e)}")
    
    def export_report_pdf(self):
        """Export the current report to PDF"""
        if not self.reports_list.currentItem():
            QMessageBox.warning(self, "No Report Selected", "Please select a report to export")
            return
            
        # Show a message that PDF export is not implemented yet
        QMessageBox.information(self, "Not Implemented", "PDF export is not implemented yet")
    
    def schedule_test(self):
        """Schedule a test to run at a specific time"""
        dialog = ScheduleDialog(self)
        if dialog.exec_():
            schedule_time = dialog.get_schedule_time()
            repeat_type = dialog.get_repeat_type()
            
            # Add to scheduled tests
            self.scheduled_tests.append({
                "time": schedule_time,
                "repeat": repeat_type,
                "last_run": None
            })
            
            self.log_message(f"Test scheduled for {schedule_time.toString()}", "blue")
            
            # Add to activity list
            self.add_activity(f"Scheduled test for {schedule_time.toString()}")
    
    def check_scheduled_tests(self):
        """Check if any scheduled tests need to be run"""
        current_time = QDateTime.currentDateTime()
        
        for test in self.scheduled_tests:
            schedule_time = test["time"]
            
            # Check if it's time to run the test
            if current_time >= schedule_time and (test["last_run"] is None or test["last_run"] < schedule_time):
                self.log_message(f"Running scheduled test", "blue")
                
                # Run the test
                self.run_all_tests()
                
                # Update last run time
                test["last_run"] = current_time
                
                # Handle repeat
                if test["repeat"] == "daily":
                    # Schedule for tomorrow same time
                    test["time"] = QDateTime(
                        schedule_time.date().addDays(1),
                        schedule_time.time()
                    )
                elif test["repeat"] == "weekly":
                    # Schedule for next week same time
                    test["time"] = QDateTime(
                        schedule_time.date().addDays(7),
                        schedule_time.time()
                    )
    
    def set_theme(self, theme):
        """Set the application theme"""
        self.settings.setValue("theme", theme)
        self.apply_theme()
        
        # Add to activity list
        self.add_activity(f"Changed theme to {theme}")
    
    def apply_theme(self):
        """Apply the selected theme"""
        theme = self.settings.value("theme", "system")
        
        if theme == "dark":
            # Apply dark theme
            palette = QPalette()
            palette.setColor(QPalette.Window, QColor(53, 53, 53))
            palette.setColor(QPalette.WindowText, Qt.white)
            palette.setColor(QPalette.Base, QColor(25, 25, 25))
            palette.setColor(QPalette.AlternateBase, QColor(53, 53, 53))
            palette.setColor(QPalette.ToolTipBase, Qt.white)
            palette.setColor(QPalette.ToolTipText, Qt.white)
            palette.setColor(QPalette.Text, Qt.white)
            palette.setColor(QPalette.Button, QColor(53, 53, 53))
            palette.setColor(QPalette.ButtonText, Qt.white)
            palette.setColor(QPalette.BrightText, Qt.red)
            palette.setColor(QPalette.Link, QColor(42, 130, 218))
            palette.setColor(QPalette.Highlight, QColor(42, 130, 218))
            palette.setColor(QPalette.HighlightedText, Qt.black)
            QApplication.setPalette(palette)
        elif theme == "light":
            # Apply light theme
            QApplication.setPalette(QApplication.style().standardPalette())
        else:
            # Use system theme
            QApplication.setPalette(QApplication.style().standardPalette())
    
    def show_about(self):
        """Show the about dialog"""
        QMessageBox.about(
            self,
            f"About {APP_NAME}",
            f"""<h1>{APP_NAME} v{APP_VERSION}</h1>
            <p>A comprehensive client application for the ESP32 Protocol Analyzer.</p>
            <p>Features:</p>
            <ul>
                <li>Modern UI with dark/light theme support</li>
                <li>Automated test sequences</li>
                <li>Test reporting and data export</li>
                <li>Real-time data visualization</li>
                <li>Device profiles and configuration management</li>
            </ul>
            <p>&copy; 2023 Protocol Analyzer Team</p>"""
        )
    
    def load_config(self):
        """Load configuration from file"""
        try:
            if os.path.exists(CONFIG_FILE):
                with open(CONFIG_FILE, 'r') as f:
                    config = json.load(f)
                
                # Load device profiles
                if "profiles" in config:
                    self.device_profiles = [DeviceProfile.from_dict(p) for p in config["profiles"]]
                    
                    if self.device_profiles:
                        self.current_profile = self.device_profiles[0]
                
                # Load theme
                if "theme" in config:
                    self.settings.setValue("theme", config["theme"])
        except Exception as e:
            print(f"Error loading config: {str(e)}")
    
    def save_config(self):
        """Save configuration to file"""
        try:
            config = {
                "profiles": [p.to_dict() for p in self.device_profiles],
                "theme": self.settings.value("theme", "system")
            }
            
            with open(CONFIG_FILE, 'w') as f:
                json.dump(config, f, indent=2)
        except Exception as e:
            print(f"Error saving config: {str(e)}")
    
    def closeEvent(self, event):
        """Handle application close event"""
        # Stop serial thread if running
        if self.serial_thread and self.serial_thread.running:
            self.serial_thread.stop()
        
        # Save configuration
        self.save_config()
        
        # Accept the close event
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setStyle('Fusion')  # Use Fusion style for a consistent look
    
    window = ProtocolAnalyzerClient()
    window.show()
    sys.exit(app.exec_())

