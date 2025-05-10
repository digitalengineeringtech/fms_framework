import sys
import serial
import serial.tools.list_ports
import time
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QLabel, QRadioButton, QComboBox, 
                             QPushButton, QGroupBox, QButtonGroup, QTextEdit,
                             QSpinBox, QSlider, QTabWidget, QMessageBox,
                             QSplitter, QFrame, QGridLayout, QSpacerItem,
                             QSizePolicy, QStatusBar, QToolBar, QAction)
from PyQt5.QtCore import Qt, QTimer, pyqtSignal, QThread
from PyQt5.QtGui import QFont, QIcon, QColor, QPalette

class SerialThread(QThread):
    """Thread for handling serial communication without blocking the UI"""
    message_received = pyqtSignal(str)
    connection_error = pyqtSignal(str)
    
    def __init__(self, serial_port):
        super().__init__()
        self.serial_port = serial_port
        self.running = True
        
    def run(self):
        try:
            while self.running:
                if self.serial_port.in_waiting > 0:
                    line = self.serial_port.readline().decode('utf-8').strip()
                    if line:
                        self.message_received.emit(line)
                time.sleep(0.01)
        except Exception as e:
            self.connection_error.emit(str(e))
            
    def stop(self):
        self.running = False
        self.wait()

class ProtocolChooserUI(QMainWindow):
    def __init__(self):
        super().__init__()
        self.serial_port = None
        self.serial_thread = None
        self.protocol_params = {
            "REDSTAR": {"PULSEWIDTH": 100, "PULSEPERIOD": 10, "BURSTCOUNT": 3, "BURSTINTERVAL": 50},
            "LANFENG": {"PULSEWIDTH": 200, "PULSEPERIOD": 20, "BURSTCOUNT": 5, "BURSTINTERVAL": 100},
            "TASUNO": {"PULSEWIDTH": 150, "PULSEPERIOD": 15, "BURSTCOUNT": 4, "BURSTINTERVAL": 75}
        }
        self.active_protocol = "REDSTAR"
        self.active_nozzles = 1
        self.initUI()
        
    def initUI(self):
        self.setWindowTitle('ESP32 Protocol Chooser')
        self.setGeometry(100, 100, 800, 600)
        
        # Create main widget and layout
        main_widget = QWidget()
        main_layout = QVBoxLayout()
        main_widget.setLayout(main_layout)
        self.setCentralWidget(main_widget)
        
        # Create toolbar
        toolbar = QToolBar("Main Toolbar")
        self.addToolBar(toolbar)
        
        # Add connect action
        connect_action = QAction("Connect", self)
        connect_action.triggered.connect(self.show_connection_dialog)
        toolbar.addAction(connect_action)
        
        # Add refresh action
        refresh_action = QAction("Refresh Status", self)
        refresh_action.triggered.connect(self.request_status)
        toolbar.addAction(refresh_action)
        
        # Create status bar
        self.statusBar = QStatusBar()
        self.setStatusBar(self.statusBar)
        self.statusBar.showMessage("Not connected to ESP32")
        
        # Create splitter for main layout
        splitter = QSplitter(Qt.Vertical)
        main_layout.addWidget(splitter)
        
        # Top section - Protocol configuration
        top_widget = QWidget()
        top_layout = QVBoxLayout()
        top_widget.setLayout(top_layout)
        
        # Title
        title_label = QLabel('ESP32 Protocol Chooser')
        title_font = QFont()
        title_font.setPointSize(16)
        title_font.setBold(True)
        title_label.setFont(title_font)
        title_label.setAlignment(Qt.AlignCenter)
        top_layout.addWidget(title_label)
        
        # Create horizontal layout for protocol and nozzle selection
        selection_layout = QHBoxLayout()
        
        # Protocol selection group
        protocol_group = QGroupBox('Select Protocol')
        protocol_layout = QVBoxLayout()
        protocol_group.setLayout(protocol_layout)
        
        # Protocol radio buttons
        self.protocol_button_group = QButtonGroup()
        
        self.redstar_radio = QRadioButton('Redstar Protocol')
        self.lanfeng_radio = QRadioButton('Lanfeng Protocol')
        self.tasuno_radio = QRadioButton('Tasuno Protocol')
        
        self.protocol_button_group.addButton(self.redstar_radio, 1)
        self.protocol_button_group.addButton(self.lanfeng_radio, 2)
        self.protocol_button_group.addButton(self.tasuno_radio, 3)
        
        protocol_layout.addWidget(self.redstar_radio)
        protocol_layout.addWidget(self.lanfeng_radio)
        protocol_layout.addWidget(self.tasuno_radio)
        
        selection_layout.addWidget(protocol_group)
        
        # Nozzle selection group
        nozzle_group = QGroupBox('Select Number of Nozzles')
        nozzle_layout = QVBoxLayout()
        nozzle_group.setLayout(nozzle_layout)
        
        nozzle_label = QLabel('Number of Nozzles:')
        self.nozzle_combo = QComboBox()
        self.nozzle_combo.addItems(['1', '2', '3', '4'])
        
        nozzle_layout.addWidget(nozzle_label)
        nozzle_layout.addWidget(self.nozzle_combo)
        nozzle_layout.addStretch()
        
        selection_layout.addWidget(nozzle_group)
        
        top_layout.addLayout(selection_layout)
        
        # Create tab widget for protocol parameters
        self.param_tabs = QTabWidget()
        
        # Create tabs for each protocol
        for protocol in ["REDSTAR", "LANFENG", "TASUNO"]:
            tab = QWidget()
            tab_layout = QGridLayout()
            tab.setLayout(tab_layout)
            
            # Create parameter controls
            row = 0
            for param, default_value in self.protocol_params[protocol].items():
                # Parameter label
                label = QLabel(f"{param}:")
                tab_layout.addWidget(label, row, 0)
                
                # Parameter value spinbox
                spinbox = QSpinBox()
                spinbox.setObjectName(f"{protocol}_{param}_spinbox")
                
                # Set range and value based on parameter
                if param == "PULSEWIDTH":
                    spinbox.setRange(10, 1000)
                    spinbox.setSuffix(" µs")
                elif param == "PULSEPERIOD":
                    spinbox.setRange(1, 1000)
                    spinbox.setSuffix(" ms")
                elif param == "BURSTCOUNT":
                    spinbox.setRange(1, 20)
                    spinbox.setSuffix(" pulses")
                elif param == "BURSTINTERVAL":
                    spinbox.setRange(10, 5000)
                    spinbox.setSuffix(" ms")
                
                spinbox.setValue(default_value)
                tab_layout.addWidget(spinbox, row, 1)
                
                # Parameter slider
                slider = QSlider(Qt.Horizontal)
                slider.setObjectName(f"{protocol}_{param}_slider")
                
                # Set range based on parameter
                if param == "PULSEWIDTH":
                    slider.setRange(10, 1000)
                elif param == "PULSEPERIOD":
                    slider.setRange(1, 1000)
                elif param == "BURSTCOUNT":
                    slider.setRange(1, 20)
                elif param == "BURSTINTERVAL":
                    slider.setRange(10, 5000)
                
                slider.setValue(default_value)
                tab_layout.addWidget(slider, row, 2)
                
                # Connect slider and spinbox
                slider.valueChanged.connect(spinbox.setValue)
                spinbox.valueChanged.connect(slider.setValue)
                
                row += 1
            
            # Add apply button
            apply_button = QPushButton(f"Apply {protocol} Settings")
            apply_button.setObjectName(f"{protocol}_apply_button")
            apply_button.clicked.connect(lambda checked, p=protocol: self.apply_protocol_settings(p))
            tab_layout.addWidget(apply_button, row, 0, 1, 3)
            
            # Add tab
            self.param_tabs.addTab(tab, protocol)
        
        top_layout.addWidget(self.param_tabs)
        
        # Add apply protocol button
        self.apply_protocol_button = QPushButton("Apply Protocol Selection")
        self.apply_protocol_button.clicked.connect(self.apply_protocol_selection)
        top_layout.addWidget(self.apply_protocol_button)
        
        splitter.addWidget(top_widget)
        
        # Bottom section - Log
        bottom_widget = QWidget()
        bottom_layout = QVBoxLayout()
        bottom_widget.setLayout(bottom_layout)
        
        log_label = QLabel("Communication Log")
        log_label.setAlignment(Qt.AlignCenter)
        bottom_layout.addWidget(log_label)
        
        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        bottom_layout.addWidget(self.log_text)
        
        splitter.addWidget(bottom_widget)
        
        # Set initial splitter sizes
        splitter.setSizes([400, 200])
        
        # Connect signals
        self.protocol_button_group.buttonClicked.connect(self.update_protocol_selection)
        self.nozzle_combo.currentIndexChanged.connect(self.update_nozzle_selection)
        
        # Set default protocol
        self.redstar_radio.setChecked(True)
        
        # Add a timer to periodically check connection status
        self.connection_timer = QTimer()
        self.connection_timer.timeout.connect(self.check_connection)
        self.connection_timer.start(5000)  # Check every 5 seconds
        
        self.log_message("UI initialized. Please connect to ESP32.")
    
    def show_connection_dialog(self):
        """Show dialog to connect to ESP32"""
        # Get list of available ports
        ports = [port.device for port in serial.tools.list_ports.comports()]
        
        if not ports:
            QMessageBox.warning(self, "No Ports", "No serial ports found. Please connect ESP32.")
            return
        
        # Create dialog
        dialog = QDialog(self)
        dialog.setWindowTitle("Connect to ESP32")
        dialog.setMinimumWidth(300)
        
        layout = QVBoxLayout()
        
        # Port selection
        port_label = QLabel("Select Port:")
        layout.addWidget(port_label)
        
        port_combo = QComboBox()
        port_combo.addItems(ports)
        layout.addWidget(port_combo)
        
        # Buttons
        button_layout = QHBoxLayout()
        connect_button = QPushButton("Connect")
        cancel_button = QPushButton("Cancel")
        
        button_layout.addWidget(connect_button)
        button_layout.addWidget(cancel_button)
        
        layout.addLayout(button_layout)
        
        dialog.setLayout(layout)
        
        # Connect signals
        connect_button.clicked.connect(lambda: self.connect_to_esp32(port_combo.currentText()))
        connect_button.clicked.connect(dialog.accept)
        cancel_button.clicked.connect(dialog.reject)
        
        dialog.exec_()
    
    def connect_to_esp32(self, port):
        """Connect to ESP32 on the specified port"""
        try:
            # Close existing connection if any
            if self.serial_port:
                self.serial_thread.stop()
                self.serial_port.close()
            
            # Open new connection
            self.serial_port = serial.Serial(port, 115200, timeout=1)
            self.log_message(f"Connected to {port}")
            self.statusBar.showMessage(f"Connected to ESP32 on {port}")
            
            # Start serial thread
            self.serial_thread = SerialThread(self.serial_port)
            self.serial_thread.message_received.connect(self.handle_serial_message)
            self.serial_thread.connection_error.connect(self.handle_connection_error)
            self.serial_thread.start()
            
            # Request status from ESP32
            self.request_status()
            
        except Exception as e:
            self.log_message(f"Error connecting to {port}: {str(e)}")
            self.statusBar.showMessage(f"Connection error: {str(e)}")
    
    def check_connection(self):
        """Check if connection to ESP32 is still active"""
        if self.serial_port and not self.serial_port.is_open:
            self.log_message("Connection to ESP32 lost")
            self.statusBar.showMessage("Connection to ESP32 lost")
            self.serial_port = None
            if self.serial_thread:
                self.serial_thread.stop()
                self.serial_thread = None
    
    def request_status(self):
        """Request current status from ESP32"""
        if self.serial_port:
            self.send_command("STATUS")
    
    def handle_serial_message(self, message):
        """Handle messages received from ESP32"""
        self.log_message(f"ESP32: {message}")
        
        # Parse message
        parts = message.split(":")
        
        if len(parts) < 2:
            return
            
        command = parts[0]
        
        # Handle different message types
        if command == "ACTIVE_PROTOCOL":
            self.active_protocol = parts[1]
            self.update_ui_protocol()
        
        elif command == "ACTIVE_NOZZLES":
            self.active_nozzles = int(parts[1])
            self.update_ui_nozzles()
        
        elif command == "PARAM" and len(parts) >= 4:
            protocol = parts[1]
            param = parts[2]
            value = int(parts[3])
            
            if protocol in self.protocol_params and param in self.protocol_params[protocol]:
                self.protocol_params[protocol][param] = value
                self.update_ui_param(protocol, param, value)
    
    def handle_connection_error(self, error_message):
        """Handle connection errors"""
        self.log_message(f"Connection error: {error_message}")
        self.statusBar.showMessage(f"Connection error: {error_message}")
        
        if self.serial_port:
            try:
                self.serial_port.close()
            except:
                pass
            self.serial_port = None
        
        if self.serial_thread:
            self.serial_thread.stop()
            self.serial_thread = None
    
    def send_command(self, command):
        """Send command to ESP32"""
        if self.serial_port:
            try:
                self.log_message(f"Sending: {command}")
                self.serial_port.write(f"{command}\n".encode('utf-8'))
                return True
            except Exception as e:
                self.log_message(f"Error sending command: {str(e)}")
                return False
        else:
            self.log_message("Not connected to ESP32")
            return False
    
    def update_protocol_selection(self):
        """Update protocol selection based on radio buttons"""
        if self.redstar_radio.isChecked():
            self.active_protocol = "REDSTAR"
        elif self.lanfeng_radio.isChecked():
            self.active_protocol = "LANFENG"
        elif self.tasuno_radio.isChecked():
            self.active_protocol = "TASUNO"
    
    def update_nozzle_selection(self):
        """Update nozzle selection based on combo box"""
        self.active_nozzles = int(self.nozzle_combo.currentText())
    
    def apply_protocol_selection(self):
        """Apply protocol and nozzle selection to ESP32"""
        # Send protocol command
        self.send_command(f"PROTOCOL:{self.active_protocol}")
        
        # Send nozzles command
        self.send_command(f"NOZZLES:{self.active_nozzles}")
    
    def apply_protocol_settings(self, protocol):
        """Apply protocol parameter settings to ESP32"""
        for param in self.protocol_params[protocol]:
            spinbox = self.findChild(QSpinBox, f"{protocol}_{param}_spinbox")
            if spinbox:
                value = spinbox.value()
                self.protocol_params[protocol][param] = value
                self.send_command(f"SET:{protocol}:{param}:{value}")
    
    def update_ui_protocol(self):
        """Update UI to reflect active protocol"""
        if self.active_protocol == "REDSTAR":
            self.redstar_radio.setChecked(True)
        elif self.active_protocol == "LANFENG":
            self.lanfeng_radio.setChecked(True)
        elif self.active_protocol == "TASUNO":
            self.tasuno_radio.setChecked(True)
    
    def update_ui_nozzles(self):
        """Update UI to reflect active nozzle count"""
        index = self.active_nozzles - 1
        if 0 <= index < self.nozzle_combo.count():
            self.nozzle_combo.setCurrentIndex(index)
    
    def update_ui_param(self, protocol, param, value):
        """Update UI to reflect parameter value"""
        spinbox = self.findChild(QSpinBox, f"{protocol}_{param}_spinbox")
        if spinbox:
            spinbox.setValue(value)
    
    def log_message(self, message):
        """Add message to log"""
        self.log_text.append(f"{time.strftime('%H:%M:%S')} - {message}")
        # Scroll to bottom
        self.log_text.verticalScrollBar().setValue(self.log_text.verticalScrollBar().maximum())
    
    def closeEvent(self, event):
        """Handle window close event"""
        if self.serial_thread:
            self.serial_thread.stop()
        
        if self.serial_port:
            self.serial_port.close()
        
        event.accept()

class QDialog(QWidget):
    """Simple dialog implementation"""
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setWindowModality(Qt.ApplicationModal)
    
    def accept(self):
        self.close()
    
    def reject(self):
        self.close()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = ProtocolChooserUI()
    ex.show()
    sys.exit(app.exec_())