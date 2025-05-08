import sys
import time
from datetime import datetime
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
                            QLabel, QPushButton, QTabWidget, QLineEdit, QRadioButton, 
                            QGroupBox, QGridLayout, QTextEdit, QScrollArea, QFrame,
                            QButtonGroup, QSplitter)
from PyQt5.QtCore import Qt, QTimer, pyqtSignal, QObject
from PyQt5.QtGui import QPainter, QPen, QColor, QFont, QBrush

class CommunicationChannel(QObject):
    """Simulates the communication between controller and pumps"""
    message_received = pyqtSignal(str)
    state_changed = pyqtSignal(int, str, float, float, float)
    mqtt_message = pyqtSignal(str, str)
    
    def __init__(self):
        super().__init__()
        self.pump_states = {
            1: {"status": "ide", "amount": 0.0, "price": 3.5, "total": 0.0},
            2: {"status": "ide", "amount": 0.0, "price": 3.5, "total": 0.0}
        }
    
    def send_command(self, command, pump_id):
        """Process commands sent to pumps"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.message_received.emit(f"[{timestamp}] Sending command: {command} to pump {pump_id}")
        
        if command == "read_state":
            self.message_received.emit(f"[{timestamp}] Reading state from pump {pump_id}")
            # Simulate response with hex values
            self.message_received.emit(f"[{timestamp}] Received: 0x12 0x57 {pump_id} {self.pump_states[pump_id]['status']} ...")
            
        elif command == "approve":
            if self.pump_states[pump_id]["status"] == "ide":
                self.pump_states[pump_id]["status"] = "fuel"
                self.state_changed.emit(pump_id, "fuel", 0.0, self.pump_states[pump_id]["price"], 0.0)
                self.message_received.emit(f"[{timestamp}] Pump {pump_id} approved for fueling")
                self.mqtt_message.emit("pump/req", f"permit for pump {pump_id}")
            
        elif command == "preset":
            self.message_received.emit(f"[{timestamp}] Setting preset for pump {pump_id}")
            
        elif command == "simulate_fueling":
            # Start a timer to simulate fueling process
            if self.pump_states[pump_id]["status"] == "fuel":
                self.message_received.emit(f"[{timestamp}] Simulating fueling on pump {pump_id}")
                
                # Simulate dispensing fuel over time
                def update_fuel():
                    amount = self.pump_states[pump_id]["amount"] + 0.5
                    price = self.pump_states[pump_id]["price"]
                    total = amount * price
                    
                    self.pump_states[pump_id]["amount"] = amount
                    self.pump_states[pump_id]["total"] = total
                    
                    self.state_changed.emit(pump_id, "fuel", amount, price, total)
                    self.message_received.emit(f"[{timestamp}] Pump {pump_id}: {amount:.2f}L, ${total:.2f}")
                    
                    # Stop after reaching ~10L
                    if amount >= 10:
                        timer.stop()
                        # Complete the transaction
                        self.pump_states[pump_id]["status"] = "ide"
                        self.state_changed.emit(pump_id, "ide", amount, price, total)
                        self.mqtt_message.emit("pump/final", f"Pump {pump_id} completed: {amount:.2f}L, ${total:.2f}")
                        self.message_received.emit(f"[{timestamp}] Fueling completed on pump {pump_id}")
                
                timer = QTimer()
                timer.timeout.connect(update_fuel)
                timer.start(500)  # Update every 500ms

class PumpWidget(QFrame):
    """Widget representing a single fuel pump"""
    command_signal = pyqtSignal(str, int)
    
    def __init__(self, pump_id):
        super().__init__()
        self.pump_id = pump_id
        self.status = "ide"
        self.amount = 0.0
        self.price = 3.5
        self.total = 0.0
        self.preset_value = ""
        
        self.init_ui()
        self.update_display()
        
    def init_ui(self):
        self.setFrameStyle(QFrame.Box | QFrame.Raised)
        self.setLineWidth(2)
        self.setMidLineWidth(1)
        
        layout = QVBoxLayout()
        
        # Header
        header_layout = QHBoxLayout()
        title = QLabel(f"Pump {self.pump_id}")
        title.setStyleSheet("font-size: 16px; font-weight: bold;")
        self.status_label = QLabel("Idle")
        self.status_label.setStyleSheet("background-color: #e0e0e0; padding: 3px 8px; border-radius: 4px;")
        
        header_layout.addWidget(title)
        header_layout.addStretch()
        header_layout.addWidget(self.status_label)
        
        # Fuel info
        info_layout = QGridLayout()
        
        # Volume display
        volume_frame = QFrame()
        volume_frame.setFrameStyle(QFrame.StyledPanel)
        volume_layout = QVBoxLayout(volume_frame)
        volume_layout.addWidget(QLabel("Volume"))
        self.volume_display = QLabel("0.00 L")
        self.volume_display.setStyleSheet("font-size: 18px; font-weight: bold;")
        volume_layout.addWidget(self.volume_display, alignment=Qt.AlignCenter)
        
        # Total display
        total_frame = QFrame()
        total_frame.setFrameStyle(QFrame.StyledPanel)
        total_layout = QVBoxLayout(total_frame)
        total_layout.addWidget(QLabel("Total"))
        self.total_display = QLabel("$0.00")
        self.total_display.setStyleSheet("font-size: 18px; font-weight: bold;")
        total_layout.addWidget(self.total_display, alignment=Qt.AlignCenter)
        
        info_layout.addWidget(volume_frame, 0, 0)
        info_layout.addWidget(total_frame, 0, 1)
        
        # Preset controls
        preset_layout = QHBoxLayout()
        preset_layout.addWidget(QLabel("Preset:"))
        self.preset_input = QLineEdit()
        self.preset_input.setPlaceholderText("Amount")
        preset_layout.addWidget(self.preset_input)
        preset_button = QPushButton("Set")
        preset_button.clicked.connect(self.on_preset)
        preset_layout.addWidget(preset_button)
        
        # Price display
        price_layout = QHBoxLayout()
        price_layout.addWidget(QLabel("Price per liter:"))
        self.price_label = QLabel(f"${self.price:.2f}/L")
        price_layout.addWidget(self.price_label)
        
        # Action buttons
        button_layout = QHBoxLayout()
        read_button = QPushButton("Read State")
        read_button.clicked.connect(self.on_read_state)
        
        self.approve_button = QPushButton("Approve")
        self.approve_button.clicked.connect(self.on_approve)
        
        button_layout.addWidget(read_button)
        button_layout.addWidget(self.approve_button)
        
        # Add all layouts to main layout
        layout.addLayout(header_layout)
        layout.addLayout(info_layout)
        layout.addLayout(preset_layout)
        layout.addLayout(price_layout)
        layout.addLayout(button_layout)
        
        self.setLayout(layout)
        
    def update_state(self, status, amount, price, total):
        """Update the pump state and display"""
        self.status = status
        self.amount = amount
        self.price = price
        self.total = total
        self.update_display()
        
    def update_display(self):
        """Update the UI to reflect current state"""
        # Update status label
        if self.status == "ide":
            self.status_label.setText("Idle")
            self.status_label.setStyleSheet("background-color: #e0e0e0; padding: 3px 8px; border-radius: 4px;")
            self.approve_button.setText("Approve")
            self.approve_button.setEnabled(True)
        else:
            self.status_label.setText("Fueling")
            self.status_label.setStyleSheet("background-color: #4caf50; color: white; padding: 3px 8px; border-radius: 4px;")
            self.approve_button.setText("Fueling")
            self.approve_button.setEnabled(False)
        
        # Update displays
        self.volume_display.setText(f"{self.amount:.2f} L")
        self.total_display.setText(f"${self.total:.2f}")
        self.price_label.setText(f"${self.price:.2f}/L")
        
        # Update background color based on status
        if self.status == "ide":
            self.setStyleSheet("background-color: #f5f5f5;")
        else:
            self.setStyleSheet("background-color: #e8f5e9;")
    
    def on_read_state(self):
        self.command_signal.emit("read_state", self.pump_id)
    
    def on_approve(self):
        if self.status == "ide":
            self.command_signal.emit("approve", self.pump_id)
        else:
            self.command_signal.emit("simulate_fueling", self.pump_id)
    
    def on_preset(self):
        preset_value = self.preset_input.text()
        if preset_value and preset_value.replace('.', '', 1).isdigit():
            self.preset_value = preset_value
            self.command_signal.emit("preset", self.pump_id)

class LogWidget(QWidget):
    """Widget for displaying communication logs"""
    def __init__(self):
        super().__init__()
        self.init_ui()
        
    def init_ui(self):
        layout = QVBoxLayout()
        
        self.log_text = QTextEdit()
        self.log_text.setReadOnly(True)
        self.log_text.setStyleSheet("background-color: #000; color: #0f0; font-family: monospace;")
        
        layout.addWidget(self.log_text)
        self.setLayout(layout)
        
    def add_log(self, message):
        self.log_text.append(message)
        # Auto-scroll to bottom
        scrollbar = self.log_text.verticalScrollBar()
        scrollbar.setValue(scrollbar.maximum())

class MqttWidget(QWidget):
    """Widget for MQTT messages and controls"""
    mqtt_send = pyqtSignal(str, str)
    
    def __init__(self):
        super().__init__()
        self.init_ui()
        
    def init_ui(self):
        layout = QVBoxLayout()
        
        # MQTT log
        self.mqtt_text = QTextEdit()
        self.mqtt_text.setReadOnly(True)
        self.mqtt_text.setStyleSheet("background-color: #f0f8ff; font-family: monospace;")
        
        # MQTT controls
        control_layout = QHBoxLayout()
        
        self.topic_input = QLineEdit("pump/cmd")
        self.message_input = QLineEdit()
        self.message_input.setPlaceholderText("Message")
        
        send_button = QPushButton("Send")
        send_button.clicked.connect(self.on_send)
        
        control_layout.addWidget(QLabel("Topic:"))
        control_layout.addWidget(self.topic_input)
        control_layout.addWidget(QLabel("Message:"))
        control_layout.addWidget(self.message_input)
        control_layout.addWidget(send_button)
        
        layout.addWidget(self.mqtt_text)
        layout.addLayout(control_layout)
        
        self.setLayout(layout)
        
    def add_message(self, topic, message):
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.mqtt_text.append(f"[{timestamp}] {topic}: {message}")
        
    def on_send(self):
        topic = self.topic_input.text()
        message = self.message_input.text()
        
        if topic and message:
            self.mqtt_send.emit(topic, message)
            self.message_input.clear()

class ControlPanel(QWidget):
    """Control panel for system-wide controls"""
    command_signal = pyqtSignal(str, int)
    address_changed = pyqtSignal(int)
    
    def __init__(self):
        super().__init__()
        self.active_address = 1
        self.init_ui()
        
    def init_ui(self):
        layout = QVBoxLayout()
        
        # Address selection
        address_group = QGroupBox("Active Address")
        address_layout = QHBoxLayout()
        
        self.address_buttons = QButtonGroup()
        
        address1_radio = QRadioButton("Pump 1")
        address1_radio.setChecked(True)
        address2_radio = QRadioButton("Pump 2")
        
        self.address_buttons.addButton(address1_radio, 1)
        self.address_buttons.addButton(address2_radio, 2)
        
        self.address_buttons.buttonClicked.connect(self.on_address_change)
        
        address_layout.addWidget(address1_radio)
        address_layout.addWidget(address2_radio)
        address_group.setLayout(address_layout)
        
        # Command buttons
        command_group = QGroupBox("Commands")
        command_layout = QGridLayout()
        
        read_button = QPushButton("Read State")
        read_button.clicked.connect(lambda: self.command_signal.emit("read_state", self.active_address))
        
        approve_button = QPushButton("Approve")
        approve_button.clicked.connect(lambda: self.command_signal.emit("approve", self.active_address))
        
        preset_button = QPushButton("Preset")
        preset_button.clicked.connect(lambda: self.command_signal.emit("preset", self.active_address))
        
        simulate_button = QPushButton("Simulate Fueling")
        simulate_button.clicked.connect(lambda: self.command_signal.emit("simulate_fueling", self.active_address))
        
        command_layout.addWidget(read_button, 0, 0)
        command_layout.addWidget(approve_button, 0, 1)
        command_layout.addWidget(preset_button, 1, 0)
        command_layout.addWidget(simulate_button, 1, 1)
        
        command_group.setLayout(command_layout)
        
        # Protocol info
        protocol_group = QGroupBox("Communication Protocol")
        protocol_layout = QVBoxLayout()
        
        protocol_text = QTextEdit()
        protocol_text.setReadOnly(True)
        protocol_text.setHtml("""
            <p><b>1. Read State Command:</b> 0x12 0x57/0x56/0x53/0x52</p>
            <p><b>2. Approve Command:</b> 0x12 0x77/0x76/0x72/0x73</p>
            <p><b>3. Preset Command:</b> Set amount before fueling</p>
            <p><b>4. Fueling Data:</b> 0x12 0x5F/0x5B</p>
            <p><b>5. Completion:</b> MQTT final message</p>
        """)
        
        protocol_layout.addWidget(protocol_text)
        protocol_group.setLayout(protocol_layout)
        
        # Add all to main layout
        layout.addWidget(address_group)
        layout.addWidget(command_group)
        layout.addWidget(protocol_group)
        layout.addStretch()
        
        self.setLayout(layout)
        
    def on_address_change(self, button):
        self.active_address = self.address_buttons.id(button)
        self.address_changed.emit(self.active_address)

class ProtocolDiagramWidget(QWidget):
    """Widget for displaying the protocol diagram"""
    def __init__(self):
        super().__init__()
        self.setMinimumHeight(400)
        
    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Define colors
        controller_color = QColor(226, 232, 240)
        pump_color = QColor(226, 232, 240)
        mqtt_color = QColor(219, 234, 254)
        arrow_color = QColor(100, 116, 139)
        mqtt_arrow_color = QColor(59, 130, 246)
        
        # Define positions
        width = self.width()
        height = self.height()
        
        controller_x = 50
        controller_y = 50
        controller_width = 120
        controller_height = 60
        
        pump1_x = width - 170
        pump1_y = 50
        pump1_width = 120
        pump1_height = 60
        
        pump2_x = width - 170
        pump2_y = height - 110
        pump2_width = 120
        pump2_height = 60
        
        mqtt_x = 50
        mqtt_y = height - 110
        mqtt_width = 120
        mqtt_height = 60
        
        # Draw boxes
        # Controller
        painter.setBrush(QBrush(controller_color))
        painter.setPen(QPen(arrow_color, 2))
        painter.drawRoundedRect(controller_x, controller_y, controller_width, controller_height, 5, 5)
        
        # Pump 1
        painter.setBrush(QBrush(pump_color))
        painter.setPen(QPen(arrow_color, 2))
        painter.drawRoundedRect(pump1_x, pump1_y, pump1_width, pump1_height, 5, 5)
        
        # Pump 2
        painter.setBrush(QBrush(pump_color))
        painter.setPen(QPen(arrow_color, 2))
        painter.drawRoundedRect(pump2_x, pump2_y, pump2_width, pump2_height, 5, 5)
        
        # MQTT
        painter.setBrush(QBrush(mqtt_color))
        painter.setPen(QPen(mqtt_arrow_color, 2))
        painter.drawRoundedRect(mqtt_x, mqtt_y, mqtt_width, mqtt_height, 5, 5)
        
        # Draw labels
        painter.setPen(Qt.black)
        font = QFont()
        font.setBold(True)
        painter.setFont(font)
        
        painter.drawText(controller_x, controller_y, controller_width, controller_height, 
                         Qt.AlignCenter, "Controller")
        painter.drawText(pump1_x, pump1_y, pump1_width, pump1_height, 
                         Qt.AlignCenter, "Pump 1")
        painter.drawText(pump2_x, pump2_y, pump2_width, pump2_height, 
                         Qt.AlignCenter, "Pump 2")
        painter.drawText(mqtt_x, mqtt_y, mqtt_width, mqtt_height, 
                         Qt.AlignCenter, "MQTT Broker")
        
        # Draw arrows
        painter.setPen(QPen(arrow_color, 2))
        
        # Controller to Pump 1 (Read State)
        self.draw_arrow(painter, 
                        controller_x + controller_width, controller_y + 20, 
                        pump1_x, controller_y + 20)
        
        # Pump 1 to Controller (Response)
        self.draw_arrow(painter, 
                        pump1_x, controller_y + 40, 
                        controller_x + controller_width, controller_y + 40)
        
        # Controller to Pump 2 (Approve)
        mid_y = (controller_y + controller_height + pump2_y) // 2
        self.draw_arrow_path(painter, [
            (controller_x + controller_width//2, controller_y + controller_height),
            (controller_x + controller_width//2, mid_y),
            (pump2_x + pump2_width//2, mid_y),
            (pump2_x + pump2_width//2, pump2_y)
        ])
        
        # Pump 2 to Controller (Fueling Data)
        mid_y2 = mid_y + 30
        self.draw_arrow_path(painter, [
            (pump2_x + pump2_width//4, pump2_y),
            (pump2_x + pump2_width//4, mid_y2),
            (controller_x + controller_width//4*3, mid_y2),
            (controller_x + controller_width//4*3, controller_y + controller_height)
        ])
        
        # Controller to MQTT
        painter.setPen(QPen(mqtt_arrow_color, 2))
        self.draw_arrow(painter, 
                        controller_x + controller_width//2, controller_y + controller_height, 
                        mqtt_x + mqtt_width//2, mqtt_y)
        
        # Draw text labels for arrows
        painter.setPen(Qt.black)
        font = QFont()
        font.setPointSize(8)
        painter.setFont(font)
        
        # Read State label
        painter.drawText((controller_x + controller_width + pump1_x)//2 - 50, 
                         controller_y + 15, 
                         "Read State (0x12 0x57)")
        
        # Response label
        painter.drawText((controller_x + controller_width + pump1_x)//2 - 40, 
                         controller_y + 55, 
                         "State Response")
        
        # Approve label
        painter.drawText((controller_x + pump2_x)//2, 
                         mid_y - 15, 
                         "Approve (0x12 0x77)")
        
        # Fueling Data label
        painter.drawText((controller_x + pump2_x)//2, 
                         mid_y2 - 15, 
                         "Fueling Data (0x12 0x5F)")
        
        # MQTT Final label
        painter.setPen(mqtt_arrow_color)
        painter.drawText(controller_x + controller_width//2 + 10, 
                         (controller_y + controller_height + mqtt_y)//2, 
                         "MQTT Final")
    
    def draw_arrow(self, painter, x1, y1, x2, y2):
        """Draw an arrow from (x1,y1) to (x2,y2)"""
        painter.drawLine(x1, y1, x2, y2)
        
        # Draw arrowhead
        angle = 0.5  # 30 degrees in radians
        arrow_size = 10
        
        dx = x2 - x1
        dy = y2 - y1
        length = (dx**2 + dy**2)**0.5
        
        if length == 0:
            return
            
        dx = dx / length
        dy = dy / length
        
        # Calculate arrowhead points
        x_arrow1 = x2 - arrow_size * (dx * 0.866 + dy * 0.5)
        y_arrow1 = y2 - arrow_size * (dy * 0.866 - dx * 0.5)
        
        x_arrow2 = x2 - arrow_size * (dx * 0.866 - dy * 0.5)
        y_arrow2 = y2 - arrow_size * (dy * 0.866 + dx * 0.5)
        
        # Draw arrowhead
        points = [
            (x2, y2),
            (x_arrow1, y_arrow1),
            (x_arrow2, y_arrow2)
        ]
        
        painter.setBrush(QBrush(painter.pen().color()))
        painter.drawPolygon(*points)
    
    def draw_arrow_path(self, painter, points):
        """Draw a path with multiple segments and an arrow at the end"""
        for i in range(len(points) - 1):
            painter.drawLine(points[i][0], points[i][1], points[i+1][0], points[i+1][1])
        
        # Draw arrowhead at the last segment
        if len(points) >= 2:
            x1, y1 = points[-2]
            x2, y2 = points[-1]
            
            # Draw arrowhead
            angle = 0.5  # 30 degrees in radians
            arrow_size = 10
            
            dx = x2 - x1
            dy = y2 - y1
            length = (dx**2 + dy**2)**0.5
            
            if length == 0:
                return
                
            dx = dx / length
            dy = dy / length
            
            # Calculate arrowhead points
            x_arrow1 = x2 - arrow_size * (dx * 0.866 + dy * 0.5)
            y_arrow1 = y2 - arrow_size * (dy * 0.866 - dx * 0.5)
            
            x_arrow2 = x2 - arrow_size * (dx * 0.866 - dy * 0.5)
            y_arrow2 = y2 - arrow_size * (dy * 0.866 + dx * 0.5)
            
            # Draw arrowhead
            points = [
                (x2, y2),
                (x_arrow1, y_arrow1),
                (x_arrow2, y_arrow2)
            ]
            
            painter.setBrush(QBrush(painter.pen().color()))
            painter.drawPolygon(*points)

class ProtocolWindow(QMainWindow):
    """Window for displaying the protocol diagram and details"""
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Fuel Pump Protocol")
        self.setMinimumSize(800, 600)
        self.init_ui()
        
    def init_ui(self):
        central_widget = QWidget()
        layout = QVBoxLayout(central_widget)
        
        # Title
        title = QLabel("Fuel Pump Protocol")
        title.setStyleSheet("font-size: 24px; font-weight: bold;")
        layout.addWidget(title)
        
        # Diagram
        diagram = ProtocolDiagramWidget()
        layout.addWidget(diagram)
        
        # Protocol description
        description_group = QGroupBox("Protocol Description")
        description_layout = QVBoxLayout()
        
        protocol_text = QTextEdit()
        protocol_text.setReadOnly(True)
        protocol_text.setHtml("""
            <h3>Read State Command</h3>
            <p>The controller sends a read state command (0x12 followed by 0x57, 0x56, 0x53, or 0x52) to the pump. 
            The pump responds with its current state information.</p>
            <pre>Controller → Pump: [0x12, 0x57, ...]
Pump → Controller: [Pump ID, Status, ...8 bytes]</pre>
            
            <h3>Approve Command</h3>
            <p>To authorize fueling, the controller sends an approve command (0x12 followed by 0x77, 0x76, 0x72, or 0x73). 
            This changes the pump status to "fuel".</p>
            <pre>Controller → Pump: [0x12, 0x77, ...]
Pump → Controller: [Pump ID, Status, ...8 bytes]</pre>
            
            <h3>Preset Command</h3>
            <p>The controller can send a preset amount for the fueling operation.</p>
            <pre>Controller → Pump: [Preset Command]
Pump → Controller: [Acknowledgment]</pre>
            
            <h3>Fueling Data</h3>
            <p>During fueling, the pump sends data (0x12 followed by 0x5F or 0x5B) with information about the current transaction.</p>
            <pre>Pump → Controller: [0x12, 0x5F, Amount, Price, Total, ...]
Controller → Pump: [Acknowledgment]</pre>
            
            <h3>Completion</h3>
            <p>When fueling is complete, the controller sends the final data to the MQTT broker and resets the pump status to "ide".</p>
            <pre>Controller → MQTT: [Pump ID, Amount, Price, Total]</pre>
        """)
        
        description_layout.addWidget(protocol_text)
        description_group.setLayout(description_layout)
        
        layout.addWidget(description_group)
        
        self.setCentralWidget(central_widget)

class MainWindow(QMainWindow):
    """Main application window"""
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Fuel Pump Control System")
        self.setMinimumSize(1000, 700)
        
        self.comm_channel = CommunicationChannel()
        self.init_ui()
        self.connect_signals()
        
    def init_ui(self):
        central_widget = QWidget()
        main_layout = QHBoxLayout(central_widget)
        
        # Left side - pumps
        left_widget = QWidget()
        left_layout = QVBoxLayout(left_widget)
        
        # Title
        title = QLabel("Fuel Pump Control System")
        title.setStyleSheet("font-size: 24px; font-weight: bold;")
        
        left_layout.addWidget(title)
        
        # Pump widgets
        pumps_layout = QHBoxLayout()
        self.pump1 = PumpWidget(1)
        self.pump2 = PumpWidget(2)
        
        pumps_layout.addWidget(self.pump1)
        pumps_layout.addWidget(self.pump2)
        
        left_layout.addLayout(pumps_layout)
        
        # Right side - controls and logs
        right_widget = QWidget()
        right_layout = QVBoxLayout(right_widget)
        
        # Control panel
        self.control_panel = ControlPanel()
        right_layout.addWidget(self.control_panel)
        
        # Tabs for logs and MQTT
        tabs = QTabWidget()
        
        # Communication log tab
        self.log_widget = LogWidget()
        tabs.addTab(self.log_widget, "Communication Log")
        
        # MQTT tab
        self.mqtt_widget = MqttWidget()
        tabs.addTab(self.mqtt_widget, "MQTT Messages")
        
        right_layout.addWidget(tabs)
        
        # Add splitter for resizable panels
        splitter = QSplitter(Qt.Horizontal)
        splitter.addWidget(left_widget)
        splitter.addWidget(right_widget)
        splitter.setSizes([600, 400])
        
        main_layout.addWidget(splitter)
        
        self.setCentralWidget(central_widget)
        
        # Create protocol window
        self.protocol_window = ProtocolWindow()
        
        # Add menu bar
        menubar = self.menuBar()
        view_menu = menubar.addMenu("View")
        
        protocol_action = view_menu.addAction("Protocol Diagram")
        protocol_action.triggered.connect(self.show_protocol)
    
    def connect_signals(self):
        # Connect pump commands to communication channel
        self.pump1.command_signal.connect(self.comm_channel.send_command)
        self.pump2.command_signal.connect(self.comm_channel.send_command)
        
        # Connect control panel commands
        self.control_panel.command_signal.connect(self.comm_channel.send_command)
        self.control_panel.address_changed.connect(self.on_address_change)
        
        # Connect communication channel signals
        self.comm_channel.message_received.connect(self.log_widget.add_log)
        self.comm_channel.state_changed.connect(self.update_pump_state)
        self.comm_channel.mqtt_message.connect(self.mqtt_widget.add_message)
        
        # Connect MQTT widget signals
        self.mqtt_widget.mqtt_send.connect(self.on_mqtt_send)
    
    def update_pump_state(self, pump_id, status, amount, price, total):
        """Update the state of the specified pump"""
        if pump_id == 1:
            self.pump1.update_state(status, amount, price, total)
        elif pump_id == 2:
            self.pump2.update_state(status, amount, price, total)
    
    def on_address_change(self, address):
        """Handle address change in control panel"""
        self.log_widget.add_log(f"Active address changed to pump {address}")
    
    def on_mqtt_send(self, topic, message):
        """Handle MQTT message sending"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.log_widget.add_log(f"[{timestamp}] MQTT message sent to {topic}: {message}")
        self.mqtt_widget.add_message(topic, message)
    
    def show_protocol(self):
        """Show the protocol diagram window"""
        self.protocol_window.show()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
