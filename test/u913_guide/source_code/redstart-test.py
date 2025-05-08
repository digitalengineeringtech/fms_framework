import sys
import serial
import serial.tools.list_ports
import json
import time
import re
from PyQt5.QtWidgets import (QApplication, QMainWindow, QTabWidget, QWidget, QVBoxLayout, 
                            QHBoxLayout, QLabel, QLineEdit, QComboBox, QPushButton, 
                            QGroupBox, QFormLayout, QSpinBox, QCheckBox, QTextEdit, 
                            QTableWidget, QTableWidgetItem, QHeaderView, QMessageBox,
                            QProgressBar, QSplitter, QFrame, QGridLayout, QRadioButton,
                            QDialog, QFileDialog)
from PyQt5.QtCore import Qt, QTimer, pyqtSignal, QThread, QDateTime
from PyQt5.QtGui import QIcon, QFont, QTextCursor, QColor

class SerialThread(QThread):
    """Thread for handling serial communication without blocking the UI"""
    data_received = pyqtSignal(str)
    hex_received = pyqtSignal(bytes)
    connection_status = pyqtSignal(bool)
    
    def __init__(self, port=None, baudrate=115200):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.running = False
        
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
            self.connection_status.emit(False)
            print(f"Serial connection error: {e}")
            return False
    
    def disconnect_serial(self):
        self.running = False
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.connection_status.emit(False)
    
    def send_data(self, data):
        if self.ser and self.ser.is_open:
            try:
                if isinstance(data, str):
                    self.ser.write(data.encode())
                else:
                    self.ser.write(data)
                return True
            except Exception as e:
                print(f"Error sending data: {e}")
        return False
    
    def run(self):
        while self.running:
            if self.ser and self.ser.is_open:
                try:
                    if self.ser.in_waiting:
                        data = self.ser.read(self.ser.in_waiting)
                        if data:
                            try:
                                text_data = data.decode('utf-8', errors='replace')
                                self.data_received.emit(text_data)
                            except:
                                pass
                            self.hex_received.emit(data)
                except Exception as e:
                    print(f"Error reading data: {e}")
                    self.running = False
                    self.connection_status.emit(False)
            time.sleep(0.1)

class MQTTSettingsDialog(QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowTitle("MQTT Settings")
        self.setMinimumWidth(400)
        
        layout = QVBoxLayout()
        
        form_layout = QFormLayout()
        
        self.server_edit = QLineEdit("192.168.1.124")
        form_layout.addRow("MQTT Server:", self.server_edit)
        
        self.port_spin = QSpinBox()
        self.port_spin.setRange(1, 65535)
        self.port_spin.setValue(1883)
        form_layout.addRow("MQTT Port:", self.port_spin)
        
        self.username_edit = QLineEdit("detpos")
        form_layout.addRow("Username:", self.username_edit)
        
        self.password_edit = QLineEdit("asdffdsa")
        self.password_edit.setEchoMode(QLineEdit.Password)
        form_layout.addRow("Password:", self.password_edit)
        
        self.topic_prefix_edit = QLineEdit("detpos")
        form_layout.addRow("Topic Prefix:", self.topic_prefix_edit)
        
        layout.addLayout(form_layout)
        
        button_layout = QHBoxLayout()
        self.save_button = QPushButton("Save")
        self.save_button.clicked.connect(self.accept)
        self.cancel_button = QPushButton("Cancel")
        self.cancel_button.clicked.connect(self.reject)
        
        button_layout.addWidget(self.save_button)
        button_layout.addWidget(self.cancel_button)
        
        layout.addLayout(button_layout)
        
        self.setLayout(layout)
    
    def get_settings(self):
        return {
            "server": self.server_edit.text(),
            "port": self.port_spin.value(),
            "username": self.username_edit.text(),
            "password": self.password_edit.text(),
            "topic_prefix": self.topic_prefix_edit.text()
        }

class PumpControllerApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Fuel Pump Controller Configuration")
        self.setMinimumSize(900, 700)
        
        # Initialize serial communication thread
        self.serial_thread = SerialThread()
        self.serial_thread.data_received.connect(self.handle_serial_data)
        self.serial_thread.hex_received.connect(self.handle_hex_data)
        self.serial_thread.connection_status.connect(self.update_connection_status)
        
        # Initialize data structures
        self.pump_status = {1: "Idle", 2: "Idle"}
        self.pump_data = {
            1: {"volume": "0.00", "amount": "0.00", "price": "0.00", "last_transaction": "-"},
            2: {"volume": "0.00", "amount": "0.00", "price": "0.00", "last_transaction": "-"}
        }
        
        # Setup UI
        self.setup_ui()
        
        # Start with a scan of available ports
        self.scan_ports()
        
        # Setup timer for periodic status updates
        self.status_timer = QTimer(self)
        self.status_timer.timeout.connect(self.update_status)
        self.status_timer.start(5000)  # Update every 5 seconds
        
    def setup_ui(self):
        # Main widget and layout
        main_widget = QWidget()
        main_layout = QVBoxLayout()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)
        
        # Connection status bar at the top
        status_layout = QHBoxLayout()
        
        # Device info
        device_info_layout = QHBoxLayout()
        device_info_layout.addWidget(QLabel("Device ID:"))
        self.device_id_spin = QSpinBox()
        self.device_id_spin.setRange(1, 8)
        self.device_id_spin.setValue(1)
        device_info_layout.addWidget(self.device_id_spin)
        
        device_info_layout.addWidget(QLabel("Nozzles:"))
        self.nozzle_count_spin = QSpinBox()
        self.nozzle_count_spin.setRange(1, 8)
        self.nozzle_count_spin.setValue(2)
        device_info_layout.addWidget(self.nozzle_count_spin)
        
        status_layout.addLayout(device_info_layout)
        status_layout.addStretch()
        
        # Connection status
        self.connection_status = QLabel("Disconnected")
        self.connection_status.setStyleSheet("color: red; font-weight: bold;")
        status_layout.addWidget(QLabel("Status:"))
        status_layout.addWidget(self.connection_status)
        
        # Port selection
        status_layout.addWidget(QLabel("Port:"))
        self.port_combo = QComboBox()
        status_layout.addWidget(self.port_combo)
        
        # Baud rate selection
        status_layout.addWidget(QLabel("Baud Rate:"))
        self.baud_combo = QComboBox()
        self.baud_combo.addItems(["9600", "19200", "38400", "57600", "115200"])
        self.baud_combo.setCurrentText("115200")
        status_layout.addWidget(self.baud_combo)
        
        # Connect/Disconnect button
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.toggle_connection)
        status_layout.addWidget(self.connect_button)
        
        # Scan ports button
        scan_button = QPushButton("Scan Ports")
        scan_button.clicked.connect(self.scan_ports)
        status_layout.addWidget(scan_button)
        
        main_layout.addLayout(status_layout)
        
        # Tab widget for different settings
        self.tabs = QTabWidget()
        self.setup_pump_tab()
        self.setup_network_tab()
        self.setup_mqtt_tab()
        self.setup_monitor_tab()
        self.setup_advanced_tab()
        
        main_layout.addWidget(self.tabs)
        
        # Bottom status bar with save/load buttons
        bottom_layout = QHBoxLayout()
        
        # Save configuration button
        save_button = QPushButton("Save Configuration")
        save_button.clicked.connect(self.save_configuration)
        bottom_layout.addWidget(save_button)
        
        # Load configuration button
        load_button = QPushButton("Load Configuration")
        load_button.clicked.connect(self.load_configuration)
        bottom_layout.addWidget(load_button)
        
        # Upload to device button
        upload_button = QPushButton("Upload to Device")
        upload_button.clicked.connect(self.upload_to_device)
        bottom_layout.addWidget(upload_button)
        
        # Read from device button
        read_button = QPushButton("Read from Device")
        read_button.clicked.connect(self.read_from_device)
        bottom_layout.addWidget(read_button)
        
        main_layout.addLayout(bottom_layout)
    
    def setup_pump_tab(self):
        pump_tab = QWidget()
        pump_layout = QVBoxLayout()
        pump_tab.setLayout(pump_layout)
        
        # Pump ID Mapping
        pump_group = QGroupBox("Pump ID Mapping")
        pump_grid = QGridLayout()
        
        # Headers
        pump_grid.addWidget(QLabel("Nozzle"), 0, 0)
        pump_grid.addWidget(QLabel("Enabled"), 0, 1)
        pump_grid.addWidget(QLabel("Pump ID"), 0, 2)
        pump_grid.addWidget(QLabel("Fuel Type"), 0, 3)
        pump_grid.addWidget(QLabel("Price"), 0, 4)
        
        # Pump mapping rows
        self.pump_mapping_widgets = []
        for i in range(8):  # Support up to 8 nozzles
            row = i + 1
            
            # Nozzle label
            nozzle_label = QLabel(f"Nozzle {i+1}")
            pump_grid.addWidget(nozzle_label, row, 0)
            
            # Enabled checkbox
            enabled_check = QCheckBox()
            enabled_check.setChecked(i < 2)  # First two are enabled by default
            pump_grid.addWidget(enabled_check, row, 1)
            
            # Pump ID spinbox
            pump_id_spin = QSpinBox()
            pump_id_spin.setRange(1, 32)
            pump_id_spin.setValue(i + 1)
            pump_grid.addWidget(pump_id_spin, row, 2)
            
            # Fuel type combo
            fuel_type_combo = QComboBox()
            fuel_type_combo.addItems(["Diesel", "Petrol", "Premium", "E85"])
            pump_grid.addWidget(fuel_type_combo, row, 3)
            
            # Price input
            price_edit = QLineEdit("0.00")
            pump_grid.addWidget(price_edit, row, 4)
            
            # Store widgets for later access
            self.pump_mapping_widgets.append({
                "nozzle_label": nozzle_label,
                "enabled_check": enabled_check,
                "pump_id_spin": pump_id_spin,
                "fuel_type_combo": fuel_type_combo,
                "price_edit": price_edit
            })
            
            # Hide rows beyond the current nozzle count
            if i >= self.nozzle_count_spin.value():
                nozzle_label.setVisible(False)
                enabled_check.setVisible(False)
                pump_id_spin.setVisible(False)
                fuel_type_combo.setVisible(False)
                price_edit.setVisible(False)
        
        # Connect nozzle count change to update visible rows
        self.nozzle_count_spin.valueChanged.connect(self.update_nozzle_count)
        
        pump_group.setLayout(pump_grid)
        pump_layout.addWidget(pump_group)
        
        # Pump Control
        control_group = QGroupBox("Pump Control")
        control_layout = QGridLayout()
        
        # Pump selection
        control_layout.addWidget(QLabel("Select Pump:"), 0, 0)
        self.control_pump_combo = QComboBox()
        self.control_pump_combo.addItems([f"Pump {i+1}" for i in range(2)])
        control_layout.addWidget(self.control_pump_combo, 0, 1)
        
        # Action buttons
        self.authorize_button = QPushButton("Authorize")
        self.authorize_button.clicked.connect(self.authorize_pump)
        control_layout.addWidget(self.authorize_button, 1, 0)
        
        self.stop_button = QPushButton("Stop")
        self.stop_button.clicked.connect(self.stop_pump)
        control_layout.addWidget(self.stop_button, 1, 1)
        
        # Preset section
        control_layout.addWidget(QLabel("Preset Type:"), 2, 0)
        self.preset_type_combo = QComboBox()
        self.preset_type_combo.addItems(["Amount", "Volume"])
        control_layout.addWidget(self.preset_type_combo, 2, 1)
        
        control_layout.addWidget(QLabel("Preset Value:"), 3, 0)
        self.preset_value_edit = QLineEdit("0.00")
        control_layout.addWidget(self.preset_value_edit, 3, 1)
        
        self.preset_button = QPushButton("Set Preset")
        self.preset_button.clicked.connect(self.set_preset)
        control_layout.addWidget(self.preset_button, 4, 0, 1, 2)
        
        control_group.setLayout(control_layout)
        pump_layout.addWidget(control_group)
        
        # Price Change
        price_group = QGroupBox("Price Change")
        price_layout = QGridLayout()
        
        price_layout.addWidget(QLabel("Select Pump:"), 0, 0)
        self.price_pump_combo = QComboBox()
        self.price_pump_combo.addItems([f"Pump {i+1}" for i in range(2)])
        price_layout.addWidget(self.price_pump_combo, 0, 1)
        
        price_layout.addWidget(QLabel("New Price:"), 1, 0)
        self.new_price_edit = QLineEdit("0.00")
        price_layout.addWidget(self.new_price_edit, 1, 1)
        
        self.change_price_button = QPushButton("Change Price")
        self.change_price_button.clicked.connect(self.change_price)
        price_layout.addWidget(self.change_price_button, 2, 0, 1, 2)
        
        price_group.setLayout(price_layout)
        pump_layout.addWidget(price_group)
        
        pump_layout.addStretch()
        
        self.tabs.addTab(pump_tab, "Pump Settings")
    
    def update_nozzle_count(self):
        count = self.nozzle_count_spin.value()
        
        # Update control pump combo boxes
        self.control_pump_combo.clear()
        self.price_pump_combo.clear()
        
        for i in range(count):
            self.control_pump_combo.addItem(f"Pump {i+1}")
            self.price_pump_combo.addItem(f"Pump {i+1}")
        
        # Show/hide nozzle rows
        for i, widgets in enumerate(self.pump_mapping_widgets):
            visible = i < count
            widgets["nozzle_label"].setVisible(visible)
            widgets["enabled_check"].setVisible(visible)
            widgets["pump_id_spin"].setVisible(visible)
            widgets["fuel_type_combo"].setVisible(visible)
            widgets["price_edit"].setVisible(visible)
    
    def setup_network_tab(self):
        network_tab = QWidget()
        network_layout = QVBoxLayout()
        network_tab.setLayout(network_layout)
        
        # WiFi Settings
        wifi_group = QGroupBox("WiFi Configuration")
        wifi_layout = QFormLayout()
        
        # WiFi Enable
        self.wifi_enabled = QCheckBox("Enable WiFi")
        self.wifi_enabled.setChecked(True)
        wifi_layout.addRow("", self.wifi_enabled)
        
        # SSID
        self.ssid_edit = QLineEdit("Innovatic IOTHouse")
        wifi_layout.addRow("SSID:", self.ssid_edit)
        
        # Password
        self.wifi_password = QLineEdit("saiIIH2024")
        self.wifi_password.setEchoMode(QLineEdit.Password)
        wifi_layout.addRow("Password:", self.wifi_password)
        
        # IP Configuration
        self.ip_config = QComboBox()
        self.ip_config.addItems(["DHCP (Automatic)", "Static IP"])
        wifi_layout.addRow("IP Configuration:", self.ip_config)
        
        # Static IP settings (initially hidden)
        self.static_ip_widget = QWidget()
        static_ip_layout = QFormLayout()
        self.static_ip_widget.setLayout(static_ip_layout)
        
        self.ip_address = QLineEdit()
        static_ip_layout.addRow("IP Address:", self.ip_address)
        
        self.subnet_mask = QLineEdit()
        static_ip_layout.addRow("Subnet Mask:", self.subnet_mask)
        
        self.gateway = QLineEdit()
        static_ip_layout.addRow("Gateway:", self.gateway)
        
        self.dns_server = QLineEdit()
        static_ip_layout.addRow("DNS Server:", self.dns_server)
        
        self.static_ip_widget.setVisible(False)
        
        # Connect IP config change to show/hide static IP settings
        self.ip_config.currentIndexChanged.connect(self.toggle_static_ip)
        
        wifi_group.setLayout(wifi_layout)
        network_layout.addWidget(wifi_group)
        network_layout.addWidget(self.static_ip_widget)
        
        # Advanced Network Settings
        advanced_group = QGroupBox("Advanced Network Settings")
        advanced_layout = QFormLayout()
        
        # Hostname
        self.hostname_edit = QLineEdit("pump-controller")
        advanced_layout.addRow("Hostname:", self.hostname_edit)
        
        # MAC Address (read-only)
        self.mac_address = QLineEdit()
        self.mac_address.setReadOnly(True)
        self.mac_address.setPlaceholderText("Will be read from device")
        advanced_layout.addRow("MAC Address:", self.mac_address)
        
        # Network scan button
        scan_network_button = QPushButton("Scan Networks")
        scan_network_button.clicked.connect(self.scan_networks)
        advanced_layout.addRow("", scan_network_button)
        
        advanced_group.setLayout(advanced_layout)
        network_layout.addWidget(advanced_group)
        
        # Network scan results
        self.network_scan_results = QTextEdit()
        self.network_scan_results.setReadOnly(True)
        self.network_scan_results.setPlaceholderText("Network scan results will appear here")
        network_layout.addWidget(self.network_scan_results)
        
        # Add stretch to push everything to the top
        network_layout.addStretch()
        
        self.tabs.addTab(network_tab, "Network Settings")
    
    def toggle_static_ip(self, index):
        self.static_ip_widget.setVisible(index == 1)  # Show if "Static IP" is selected
    
    def setup_mqtt_tab(self):
        mqtt_tab = QWidget()
        mqtt_layout = QVBoxLayout()
        mqtt_tab.setLayout(mqtt_layout)
        
        # MQTT Broker Settings
        broker_group = QGroupBox("MQTT Broker Configuration")
        broker_layout = QFormLayout()
        
        # MQTT Enable
        self.mqtt_enabled = QCheckBox("Enable MQTT")
        self.mqtt_enabled.setChecked(True)
        broker_layout.addRow("", self.mqtt_enabled)
        
        # Broker Address
        self.broker_address = QLineEdit("192.168.1.124")
        broker_layout.addRow("Broker Address:", self.broker_address)
        
        # Broker Port
        self.broker_port = QSpinBox()
        self.broker_port.setRange(1, 65535)
        self.broker_port.setValue(1883)
        broker_layout.addRow("Broker Port:", self.broker_port)
        
        # Authentication
        self.auth_enabled = QCheckBox("Enable Authentication")
        broker_layout.addRow("", self.auth_enabled)
        
        # Username and Password (initially hidden)
        self.auth_widget = QWidget()
        auth_layout = QFormLayout()
        self.auth_widget.setLayout(auth_layout)
        
        self.mqtt_username = QLineEdit("detpos")
        auth_layout.addRow("Username:", self.mqtt_username)
        
        self.mqtt_password = QLineEdit("asdffdsa")
        self.mqtt_password.setEchoMode(QLineEdit.Password)
        auth_layout.addRow("Password:", self.mqtt_password)
        
        self.auth_widget.setVisible(False)
        
        # Connect auth checkbox to show/hide auth settings
        self.auth_enabled.stateChanged.connect(lambda state: self.auth_widget.setVisible(state == Qt.Checked))
        
        # TLS/SSL
        self.tls_enabled = QCheckBox("Enable TLS/SSL")
        broker_layout.addRow("", self.tls_enabled)
        
        # Client ID
        self.client_id = QLineEdit("pump_controller_1")
        broker_layout.addRow("Client ID:", self.client_id)
        
        # Keep Alive
        self.keep_alive = QSpinBox()
        self.keep_alive.setRange(5, 300)
        self.keep_alive.setValue(60)
        broker_layout.addRow("Keep Alive (seconds):", self.keep_alive)
        
        broker_group.setLayout(broker_layout)
        mqtt_layout.addWidget(broker_group)
        mqtt_layout.addWidget(self.auth_widget)
        
        # MQTT Topics
        topics_group = QGroupBox("MQTT Topics")
        topics_layout = QFormLayout()
        
        # Topic Prefix
        self.topic_prefix = QLineEdit("detpos")
        topics_layout.addRow("Topic Prefix:", self.topic_prefix)
        
        # Topic Preview
        topics_preview = QGroupBox("Topic Structure Preview")
        preview_layout = QFormLayout()
        
        preview_layout.addRow("Pump Approval:", QLabel("detpos/local_server/1"))
        preview_layout.addRow("Pump Request:", QLabel("detpos/device/permit/1"))
        preview_layout.addRow("Pump Preset:", QLabel("detpos/local_server/preset"))
        preview_layout.addRow("Live Data:", QLabel("detpos/device/livedata/1"))
        preview_layout.addRow("Final Data:", QLabel("detpos/device/Final/1"))
        preview_layout.addRow("Price Change:", QLabel("detpos/local_server/price"))
        
        topics_preview.setLayout(preview_layout)
        
        topics_group.setLayout(topics_layout)
        mqtt_layout.addWidget(topics_group)
        mqtt_layout.addWidget(topics_preview)
        
        # MQTT Test
        test_group = QGroupBox("MQTT Connection Test")
        test_layout = QHBoxLayout()
        
        test_button = QPushButton("Test MQTT Connection")
        test_button.clicked.connect(self.test_mqtt_connection)
        test_layout.addWidget(test_button)
        
        self.mqtt_status = QLabel("Not Tested")
        test_layout.addWidget(self.mqtt_status)
        
        test_group.setLayout(test_layout)
        mqtt_layout.addWidget(test_group)
        
        # Add stretch to push everything to the top
        mqtt_layout.addStretch()
        
        self.tabs.addTab(mqtt_tab, "MQTT Settings")
    
    def setup_monitor_tab(self):
        monitor_tab = QWidget()
        monitor_layout = QVBoxLayout()
        monitor_tab.setLayout(monitor_layout)
        
        # Split the tab into two sections
        splitter = QSplitter(Qt.Vertical)
        
        # Pump Status section
        pump_status_widget = QWidget()
        pump_status_layout = QVBoxLayout()
        pump_status_widget.setLayout(pump_status_layout)
        
        pump_status_label = QLabel("Pump Status")
        pump_status_label.setFont(QFont("Arial", 12, QFont.Bold))
        pump_status_layout.addWidget(pump_status_label)
        
        # Pump status table
        self.pump_status_table = QTableWidget(2, 5)  # 2 rows (pumps), 5 columns
        self.pump_status_table.setHorizontalHeaderLabels(["Pump ID", "Status", "Last Transaction", "Volume", "Amount"])
        self.pump_status_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        
        # Initialize with some default values
        for row in range(2):
            self.pump_status_table.setItem(row, 0, QTableWidgetItem(f"{row+1}"))
            self.pump_status_table.setItem(row, 1, QTableWidgetItem("Idle"))
            self.pump_status_table.setItem(row, 2, QTableWidgetItem("-"))
            self.pump_status_table.setItem(row, 3, QTableWidgetItem("0.00 L"))
            self.pump_status_table.setItem(row, 4, QTableWidgetItem("$0.00"))
        
        pump_status_layout.addWidget(self.pump_status_table)
        
        # Refresh button for pump status
        refresh_button = QPushButton("Refresh Status")
        refresh_button.clicked.connect(self.refresh_pump_status)
        pump_status_layout.addWidget(refresh_button)
        
        splitter.addWidget(pump_status_widget)
        
        # Logs section
        logs_widget = QWidget()
        logs_layout = QVBoxLayout()
        logs_widget.setLayout(logs_layout)
        
        logs_label = QLabel("Device Logs")
        logs_label.setFont(QFont("Arial", 12, QFont.Bold))
        logs_layout.addWidget(logs_label)
        
        # Log display
        self.log_display = QTextEdit()
        self.log_display.setReadOnly(True)
        self.log_display.setFont(QFont("Courier New", 9))
        logs_layout.addWidget(self.log_display)
        
        # Log control buttons
        log_buttons_layout = QHBoxLayout()
        
        clear_logs_button = QPushButton("Clear Logs")
        clear_logs_button.clicked.connect(self.clear_logs)
        log_buttons_layout.addWidget(clear_logs_button)
        
        save_logs_button = QPushButton("Save Logs")
        save_logs_button.clicked.connect(self.save_logs)
        log_buttons_layout.addWidget(save_logs_button)
        
        logs_layout.addLayout(log_buttons_layout)
        
        splitter.addWidget(logs_widget)
        
        # Set initial sizes
        splitter.setSizes([300, 300])
        
        monitor_layout.addWidget(splitter)
        
        self.tabs.addTab(monitor_tab, "Monitor")
    
    def setup_advanced_tab(self):
        advanced_tab = QWidget()
        advanced_layout = QVBoxLayout()
        advanced_tab.setLayout(advanced_layout)
        
        # Firmware Update
        firmware_group = QGroupBox("Firmware Update")
        firmware_layout = QVBoxLayout()
        
        firmware_file_layout = QHBoxLayout()
        self.firmware_path = QLineEdit()
        self.firmware_path.setReadOnly(True)
        self.firmware_path.setPlaceholderText("Select firmware file...")
        firmware_file_layout.addWidget(self.firmware_path)
        
        browse_button = QPushButton("Browse")
        browse_button.clicked.connect(self.browse_firmware)
        firmware_file_layout.addWidget(browse_button)
        
        firmware_layout.addLayout(firmware_file_layout)
        
        upload_firmware_button = QPushButton("Upload Firmware")
        upload_firmware_button.clicked.connect(self.upload_firmware)
        firmware_layout.addWidget(upload_firmware_button)
        
        self.firmware_progress = QProgressBar()
        self.firmware_progress.setRange(0, 100)
        self.firmware_progress.setValue(0)
        firmware_layout.addWidget(self.firmware_progress)
        
        firmware_group.setLayout(firmware_layout)
        advanced_layout.addWidget(firmware_group)
        
        # Serial Communication Settings
        serial_group = QGroupBox("Serial Communication Settings")
        serial_layout = QFormLayout()
        
        # Data Bits
        self.data_bits_combo = QComboBox()
        self.data_bits_combo.addItems(["7", "8"])
        self.data_bits_combo.setCurrentText("8")
        serial_layout.addRow("Data Bits:", self.data_bits_combo)
        
        # Parity
        self.parity_combo = QComboBox()
        self.parity_combo.addItems(["None", "Even", "Odd"])
        serial_layout.addRow("Parity:", self.parity_combo)
        
        # Stop Bits
        self.stop_bits_combo = QComboBox()
        self.stop_bits_combo.addItems(["1", "2"])
        serial_layout.addRow("Stop Bits:", self.stop_bits_combo)
        
        serial_group.setLayout(serial_layout)
        advanced_layout.addWidget(serial_group)
        
        # Raw Command Interface
        command_group = QGroupBox("Raw Command Interface")
        command_layout = QVBoxLayout()
        
        command_layout.addWidget(QLabel("Send raw hex commands to the device:"))
        
        command_input_layout = QHBoxLayout()
        self.command_input = QLineEdit()
        self.command_input.setPlaceholderText("Enter hex bytes (e.g., 01 03 12 EA)")
        command_input_layout.addWidget(self.command_input)
        
        send_command_button = QPushButton("Send")
        send_command_button.clicked.connect(self.send_raw_command)
        command_input_layout.addWidget(send_command_button)
        
        command_layout.addLayout(command_input_layout)
        
        # Raw response display
        command_layout.addWidget(QLabel("Response:"))
        self.command_response = QTextEdit()
        self.command_response.setReadOnly(True)
        self.command_response.setFont(QFont("Courier New", 9))
        self.command_response.setMaximumHeight(100)
        command_layout.addWidget(self.command_response)
        
        command_group.setLayout(command_layout)
        advanced_layout.addWidget(command_group)
        
        # Device Reset
        reset_group = QGroupBox("Device Reset")
        reset_layout = QHBoxLayout()
        
        reset_button = QPushButton("Reset Device")
        reset_button.clicked.connect(self.reset_device)
        reset_layout.addWidget(reset_button)
        
        factory_reset_button = QPushButton("Factory Reset")
        factory_reset_button.clicked.connect(self.factory_reset)
        reset_layout.addWidget(factory_reset_button)
        
        reset_group.setLayout(reset_layout)
        advanced_layout.addWidget(reset_group)
        
        advanced_layout.addStretch()
        
        self.tabs.addTab(advanced_tab, "Advanced")
    
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
                    self.log_message(f"Connected to {port} at {baudrate} baud")
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
        """Handle text data received from the serial port"""
        self.log_message(f"RX: {data.strip()}")
        
        # Parse data for pump status updates
        self.parse_pump_data(data)
    
    def handle_hex_data(self, data):
        """Handle binary data received from the serial port"""
        hex_str = ' '.join(f'{b:02X}' for b in data)
        self.command_response.append(f"RX: {hex_str}")
    
    def parse_pump_data(self, data):
        """Parse incoming data for pump status information"""
        # Look for volume and price data
        volume_match = re.search(r"liter is => (\d+\.?\d*)", data)
        price_match = re.search(r"price is => (\d+\.?\d*)", data)
        
        if volume_match and price_match:
            pump_id = 1  # Default to pump 1 if not specified
            
            # Try to determine which pump the data is for
            pump_match = re.search(r"Pump (\d+)", data)
            if pump_match:
                pump_id = int(pump_match.group(1))
            
            volume = volume_match.group(1)
            price = price_match.group(1)
            
            # Update pump data
            if pump_id in self.pump_data:
                self.pump_data[pump_id]["volume"] = volume
                self.pump_data[pump_id]["amount"] = price
                self.pump_data[pump_id]["last_transaction"] = QDateTime.currentDateTime().toString("hh:mm:ss")
                
                # Update status table
                self.update_pump_status_table()
    
    def update_pump_status_table(self):
        """Update the pump status table with current data"""
        for row, pump_id in enumerate(sorted(self.pump_data.keys())):
            if row < self.pump_status_table.rowCount():
                data = self.pump_data[pump_id]
                self.pump_status_table.setItem(row, 0, QTableWidgetItem(f"{pump_id}"))
                self.pump_status_table.setItem(row, 1, QTableWidgetItem(self.pump_status.get(pump_id, "Idle")))
                self.pump_status_table.setItem(row, 2, QTableWidgetItem(data["last_transaction"]))
                self.pump_status_table.setItem(row, 3, QTableWidgetItem(f"{data['volume']} L"))
                self.pump_status_table.setItem(row, 4, QTableWidgetItem(f"${data['amount']}"))
    
    def log_message(self, message):
        """Add a message to the log display"""
        timestamp = QDateTime.currentDateTime().toString("hh:mm:ss.zzz")
        self.log_display.append(f"[{timestamp}] {message}")
        # Auto-scroll to the bottom
        self.log_display.moveCursor(QTextCursor.End)
    
    def clear_logs(self):
        """Clear the log display"""
        self.log_display.clear()
    
    def save_logs(self):
        """Save the logs to a file"""
        filename, _ = QFileDialog.getSaveFileName(self, "Save Logs", "", "Text Files (*.txt);;All Files (*)")
        if filename:
            try:
                with open(filename, 'w') as f:
                    f.write(self.log_display.toPlainText())
                self.log_message(f"Logs saved to {filename}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Failed to save logs: {e}")
    
    def refresh_pump_status(self):
        """Refresh the pump status display"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        # Send a command to request pump status
        self.serial_thread.send_data(b'\x01\x03\x12\xEA')  # Read state for pump 1
        self.log_message("Requesting pump status...")
    
    def update_status(self):
        """Periodic status update"""
        if self.serial_thread.running:
            # Alternate between pumps
            if hasattr(self, 'last_status_pump') and self.last_status_pump == 1:
                self.serial_thread.send_data(b'\x02\x03\x12\xE9')  # Read state for pump 2
                self.last_status_pump = 2
            else:
                self.serial_thread.send_data(b'\x01\x03\x12\xEA')  # Read state for pump 1
                self.last_status_pump = 1
    
    def authorize_pump(self):
        """Authorize the selected pump"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        pump_index = self.control_pump_combo.currentIndex()
        pump_id = pump_index + 1
        
        # Send authorize command
        if pump_id == 1:
            self.serial_thread.send_data(b'\x01\x03\x08\xF4')  # Authorize pump 1
        elif pump_id == 2:
            self.serial_thread.send_data(b'\x02\x03\x08\xF3')  # Authorize pump 2
        
        self.log_message(f"Authorizing pump {pump_id}")
        self.pump_status[pump_id] = "Authorized"
        self.update_pump_status_table()
    
    def stop_pump(self):
        """Stop the selected pump"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        pump_index = self.control_pump_combo.currentIndex()
        pump_id = pump_index + 1
        
        # There's no direct stop command in the provided code, so we'll log it
        self.log_message(f"Stop command sent to pump {pump_id}")
        self.pump_status[pump_id] = "Idle"
        self.update_pump_status_table()
    
    def set_preset(self):
        """Set a preset for the selected pump"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        pump_index = self.control_pump_combo.currentIndex()
        pump_id = pump_index + 1
        preset_type = self.preset_type_combo.currentText()
        preset_value = self.preset_value_edit.text()
        
        try:
            value = float(preset_value)
            if value <= 0:
                raise ValueError("Value must be greater than zero")
        except ValueError as e:
            QMessageBox.warning(self, "Invalid Value", f"Please enter a valid number: {e}")
            return
        
        # Construct preset command based on the ESP32 code
        preset_data = bytearray(8)
        preset_data[0] = pump_id  # Pump ID
        preset_data[1] = 0x07     # Command
        
        if preset_type == "Amount":
            preset_data[2] = 0x89  # Price preset
            int_value = int(value * 100)  # Convert to cents
        else:  # Volume
            preset_data[2] = 0x8B  # Volume preset
            int_value = int(value * 1000)  # Convert to milliliters
        
        # Set the value bytes (big endian)
        preset_data[3] = (int_value >> 24) & 0xFF
        preset_data[4] = (int_value >> 16) & 0xFF
        preset_data[5] = (int_value >> 8) & 0xFF
        preset_data[6] = int_value & 0xFF
        
        # Calculate checksum (two's complement of sum)
        checksum = 0
        for i in range(7):
            checksum += preset_data[i]
        preset_data[7] = (~checksum + 1) & 0xFF
        
        # Send the command
        self.serial_thread.send_data(preset_data)
        
        preset_type_str = "amount" if preset_type == "Amount" else "volume"
        self.log_message(f"Setting {preset_type_str} preset of {preset_value} for pump {pump_id}")
    
    def change_price(self):
        """Change the price for the selected pump"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        pump_index = self.price_pump_combo.currentIndex()
        pump_id = pump_index + 1
        new_price = self.new_price_edit.text()
        
        try:
            price = float(new_price)
            if price < 0:
                raise ValueError("Price cannot be negative")
        except ValueError as e:
            QMessageBox.warning(self, "Invalid Price", f"Please enter a valid price: {e}")
            return
        
        # Construct price change command based on the ESP32 code
        price_data = bytearray(8)
        price_data[0] = pump_id  # Pump ID
        price_data[1] = 0x07     # Command
        price_data[2] = 0x80     # Price change
        
        int_price = int(price * 100)  # Convert to cents
        
        # Set the price bytes (big endian)
        price_data[3] = (int_price >> 24) & 0xFF
        price_data[4] = (int_price >> 16) & 0xFF
        price_data[5] = (int_price >> 8) & 0xFF
        price_data[6] = int_price & 0xFF
        
        # Calculate checksum (two's complement of sum)
        checksum = 0
        for i in range(7):
            checksum += price_data[i]
        price_data[7] = (~checksum + 1) & 0xFF
        
        # Send the command
        self.serial_thread.send_data(price_data)
        
        self.log_message(f"Changing price to {new_price} for pump {pump_id}")
    
    def scan_networks(self):
        """Scan for available WiFi networks"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        self.network_scan_results.clear()
        self.network_scan_results.append("Scanning for networks...")
        
        # This would normally send a command to the ESP32 to scan networks
        # For now, we'll simulate it
        self.serial_thread.send_data("scan_networks\n")
        
        # Simulate network scan results
        QTimer.singleShot(2000, self.simulate_network_scan)
    
    def simulate_network_scan(self):
        """Simulate WiFi network scan results"""
        networks = [
            {"ssid": "Innovatic IOTHouse", "signal": 90, "secured": True},
            {"ssid": "DETPOS_NETWORK", "signal": 75, "secured": True},
            {"ssid": "Guest WiFi", "signal": 60, "secured": False},
        ]
        
        self.network_scan_results.clear()
        self.network_scan_results.append("Available Networks:\n")
        
        for network in networks:
            security = "Secured" if network["secured"] else "Open"
            self.network_scan_results.append(f"{network['ssid']} - Signal: {network['signal']}% - {security}")
    
    def test_mqtt_connection(self):
        """Test the MQTT connection"""
        # This would normally test the MQTT connection on the ESP32
        # For now, we'll simulate it
        
        dialog = MQTTSettingsDialog(self)
        if dialog.exec_():
            settings = dialog.get_settings()
            self.broker_address.setText(settings["server"])
            self.broker_port.setValue(settings["port"])
            self.mqtt_username.setText(settings["username"])
            self.mqtt_password.setText(settings["password"])
            self.topic_prefix.setText(settings["topic_prefix"])
            
            # Simulate successful connection
            self.mqtt_status.setText("Connected")
            self.mqtt_status.setStyleSheet("color: green; font-weight: bold;")
            self.log_message(f"MQTT connected to {settings['server']}:{settings['port']}")
    
    def browse_firmware(self):
        """Browse for firmware file"""
        filename, _ = QFileDialog.getOpenFileName(self, "Select Firmware File", "", "Binary Files (*.bin);;All Files (*)")
        if filename:
            self.firmware_path.setText(filename)
    
    def upload_firmware(self):
        """Upload firmware to the device"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        if not self.firmware_path.text():
            QMessageBox.warning(self, "No File Selected", "Please select a firmware file")
            return
        
        # This would normally upload the firmware to the ESP32
        # For now, we'll simulate it
        self.log_message(f"Starting firmware upload: {self.firmware_path.text()}")
        
        # Simulate upload progress
        self.firmware_progress.setValue(0)
        
        for i in range(101):
            QApplication.processEvents()
            self.firmware_progress.setValue(i)
            time.sleep(0.05)
        
        self.log_message("Firmware upload complete")
        QMessageBox.information(self, "Upload Complete", "Firmware has been successfully uploaded")
    
    def send_raw_command(self):
        """Send a raw command to the device"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        command = self.command_input.text().strip()
        if not command:
            return
        
        try:
            # Convert hex string to bytes
            hex_bytes = bytes.fromhex(command)
            self.serial_thread.send_data(hex_bytes)
            self.command_response.append(f"TX: {command}")
            self.log_message(f"Sent raw command: {command}")
        except ValueError:
            QMessageBox.warning(self, "Invalid Command", "Please enter valid hexadecimal values (e.g., 01 03 12 EA)")
    
    def reset_device(self):
        """Reset the device"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        reply = QMessageBox.question(self, "Reset Device", 
                                    "Are you sure you want to reset the device?",
                                    QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
        
        if reply == QMessageBox.Yes:
            # Send reset command
            self.serial_thread.send_data("reset\n")
            self.log_message("Device reset command sent")
    
    def factory_reset(self):
        """Perform a factory reset on the device"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        reply = QMessageBox.question(self, "Factory Reset", 
                                    "Are you sure you want to perform a factory reset? This will erase all settings.",
                                    QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
        
        if reply == QMessageBox.Yes:
            # Send factory reset command
            self.serial_thread.send_data("factory_reset\n")
            self.log_message("Factory reset command sent")
    
    def save_configuration(self):
        """Save the current configuration to a file"""
        filename, _ = QFileDialog.getSaveFileName(self, "Save Configuration", "", "JSON Files (*.json);;All Files (*)")
        if not filename:
            return
        
        config = self.get_current_config()
        
        try:
            with open(filename, 'w') as f:
                json.dump(config, f, indent=2)
            self.log_message(f"Configuration saved to {filename}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to save configuration: {e}")
    
    def load_configuration(self):
        """Load configuration from a file"""
        filename, _ = QFileDialog.getOpenFileName(self, "Load Configuration", "", "JSON Files (*.json);;All Files (*)")
        if not filename:
            return
        
        try:
            with open(filename, 'r') as f:
                config = json.load(f)
            
            self.apply_config(config)
            self.log_message(f"Configuration loaded from {filename}")
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Failed to load configuration: {e}")
    
    def upload_to_device(self):
        """Upload the current configuration to the device"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        # Collect all settings
        config = self.get_current_config()
        
        # Convert to JSON
        config_json = json.dumps(config)
        
        # In a real implementation, you would send this to the device
        # using an appropriate protocol
        self.log_message("Uploading configuration to device...")
        self.log_message(f"Config: {config_json}")
        
        # Simulate upload progress
        progress_dialog = QMessageBox(self)
        progress_dialog.setWindowTitle("Uploading")
        progress_dialog.setText("Uploading configuration to device...")
        progress_dialog.setStandardButtons(QMessageBox.NoButton)
        progress_bar = QProgressBar(progress_dialog)
        progress_bar.setRange(0, 100)
        progress_bar.setValue(0)
        
        layout = progress_dialog.layout()
        layout.addWidget(progress_bar, 1, 0, 1, layout.columnCount())
        progress_dialog.show()
        
        # Simulate progress
        for i in range(101):
            progress_bar.setValue(i)
            QApplication.processEvents()
            time.sleep(0.02)
        
        progress_dialog.close()
        
        QMessageBox.information(self, "Upload Complete", "Configuration uploaded successfully")
    
    def read_from_device(self):
        """Read the current configuration from the device"""
        if not self.serial_thread.running:
            QMessageBox.warning(self, "Not Connected", "Please connect to the device first")
            return
        
        # Send a command to request configuration
        self.serial_thread.send_data("get_config\n")
        self.log_message("Requesting device configuration...")
        
        # In a real implementation, you would wait for and parse the response
        # For now, we'll simulate it
        QTimer.singleShot(1000, self.simulate_config_response)
    
    def simulate_config_response(self):
        """Simulate a configuration response from the device"""
        # Create a sample configuration
        config = {
            "device": {
                "id": 1,
                "nozzle_count": 2,
                "serial": {
                    "data_bits": 8,
                    "parity": "none",
                    "stop_bits": 1
                },
                "pumps": [
                    {
                        "nozzle": 1,
                        "enabled": True,
                        "pump_id": 1,
                        "fuel_type": "diesel"
                    },
                    {
                        "nozzle": 2,
                        "enabled": True,
                        "pump_id": 2,
                        "fuel_type": "petrol"
                    }
                ]
            },
            "network": {
                "wifi_enabled": True,
                "ssid": "Innovatic IOTHouse",
                "password": "saiIIH2024",
                "ip_config": "dhcp",
                "hostname": "pump-controller"
            },
            "mqtt": {
                "enabled": True,
                "broker": "192.168.1.124",
                "port": 1883,
                "auth_enabled": False,
                "username": "detpos",
                "password": "asdffdsa",
                "tls_enabled": False,
                "client_id": "pump_controller_1",
                "keep_alive": 60,
                "topic_prefix": "detpos"
            }
        }
        
        # Apply the configuration
        self.apply_config(config)
        self.log_message("Received configuration from device")
    
    def get_current_config(self):
        """Get the current configuration from the UI"""
        config = {
            "device": {
                "id": self.device_id_spin.value(),
                "nozzle_count": self.nozzle_count_spin.value(),
                "serial": {
                    "data_bits": int(self.data_bits_combo.currentText()),
                    "parity": self.parity_combo.currentText().lower(),
                    "stop_bits": int(self.stop_bits_combo.currentText())
                },
                "pumps": []
            },
            "network": {
                "wifi_enabled": self.wifi_enabled.isChecked(),
                "ssid": self.ssid_edit.text(),
                "password": self.wifi_password.text(),
                "ip_config": "dhcp" if self.ip_config.currentIndex() == 0 else "static",
                "hostname": self.hostname_edit.text()
            },
            "mqtt": {
                "enabled": self.mqtt_enabled.isChecked(),
                "broker": self.broker_address.text(),
                "port": self.broker_port.value(),
                "auth_enabled": self.auth_enabled.isChecked(),
                "username": self.mqtt_username.text(),
                "password": self.mqtt_password.text(),
                "tls_enabled": self.tls_enabled.isChecked(),
                "client_id": self.client_id.text(),
                "keep_alive": self.keep_alive.value(),
                "topic_prefix": self.topic_prefix.text()
            }
        }
        
        # Add static IP settings if enabled
        if config["network"]["ip_config"] == "static":
            config["network"]["static_ip"] = {
                "ip_address": self.ip_address.text(),
                "subnet_mask": self.subnet_mask.text(),
                "gateway": self.gateway.text(),
                "dns_server": self.dns_server.text()
            }
        
        # Add pump mapping
        for i, widgets in enumerate(self.pump_mapping_widgets):
            if i < self.nozzle_count_spin.value():
                config["device"]["pumps"].append({
                    "nozzle": i + 1,
                    "enabled": widgets["enabled_check"].isChecked(),
                    "pump_id": widgets["pump_id_spin"].value(),
                    "fuel_type": widgets["fuel_type_combo"].currentText().lower(),
                    "price": widgets["price_edit"].text()
                })
        
        return config
    
    def apply_config(self, config):
        """Apply a configuration to the UI"""
        # Device settings
        self.device_id_spin.setValue(config["device"]["id"])
        self.nozzle_count_spin.setValue(config["device"]["nozzle_count"])
        
        # Serial settings
        self.data_bits_combo.setCurrentText(str(config["device"]["serial"]["data_bits"]))
        self.parity_combo.setCurrentText(config["device"]["serial"]["parity"].capitalize())
        self.stop_bits_combo.setCurrentText(str(config["device"]["serial"]["stop_bits"]))
        
        # Network settings
        self.wifi_enabled.setChecked(config["network"]["wifi_enabled"])
        self.ssid_edit.setText(config["network"]["ssid"])
        self.wifi_password.setText(config["network"]["password"])
        self.ip_config.setCurrentIndex(0 if config["network"]["ip_config"] == "dhcp" else 1)
        self.hostname_edit.setText(config["network"]["hostname"])
        
        # Static IP settings if available
        if "static_ip" in config["network"]:
            self.ip_address.setText(config["network"]["static_ip"]["ip_address"])
            self.subnet_mask.setText(config["network"]["static_ip"]["subnet_mask"])
            self.gateway.setText(config["network"]["static_ip"]["gateway"])
            self.dns_server.setText(config["network"]["static_ip"]["dns_server"])
        
        # MQTT settings
        self.mqtt_enabled.setChecked(config["mqtt"]["enabled"])
        self.broker_address.setText(config["mqtt"]["broker"])
        self.broker_port.setValue(config["mqtt"]["port"])
        self.auth_enabled.setChecked(config["mqtt"]["auth_enabled"])
        self.mqtt_username.setText(config["mqtt"]["username"])
        self.mqtt_password.setText(config["mqtt"]["password"])
        self.tls_enabled.setChecked(config["mqtt"]["tls_enabled"])
        self.client_id.setText(config["mqtt"]["client_id"])
        self.keep_alive.setValue(config["mqtt"]["keep_alive"])
        self.topic_prefix.setText(config["mqtt"]["topic_prefix"])
        
        # Pump mapping
        for i, pump in enumerate(config["device"]["pumps"]):
            if i < len(self.pump_mapping_widgets):
                widgets = self.pump_mapping_widgets[i]
                widgets["enabled_check"].setChecked(pump["enabled"])
                widgets["pump_id_spin"].setValue(pump["pump_id"])
                widgets["fuel_type_combo"].setCurrentText(pump["fuel_type"].capitalize())
                if "price" in pump:
                    widgets["price_edit"].setText(pump["price"])
        
        # Update UI based on new settings
        self.update_nozzle_count()
        self.auth_widget.setVisible(self.auth_enabled.isChecked())
        self.static_ip_widget.setVisible(self.ip_config.currentIndex() == 1)

def main():
    app = QApplication(sys.argv  == 1)

def main():
    app = QApplication(sys.argv)
    window = PumpControllerApp()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()