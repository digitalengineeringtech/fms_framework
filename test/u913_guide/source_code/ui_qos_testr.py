# -*- coding: utf-8 -*-
import sys
import time
import json
import random
from datetime import datetime
from PyQt6.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                            QHBoxLayout, QLabel, QLineEdit, QComboBox, 
                            QPushButton, QSpinBox, QTextEdit, QGroupBox,
                            QProgressBar, QTabWidget, QGridLayout, QFormLayout)
from PyQt6.QtCore import Qt, QThread, pyqtSignal, QTimer
from PyQt6.QtGui import QFont, QColor, QPalette
import paho.mqtt.client as mqtt
import pyqtgraph as pg

class MQTTClient(QThread):
    """Thread for handling MQTT operations without blocking the UI"""
    message_received = pyqtSignal(dict)
    connection_status = pyqtSignal(bool, str)
    publish_status = pyqtSignal(int, bool)
    test_complete = pyqtSignal()
    
    def __init__(self):
        super().__init__()
        self.client = None
        self.connected = False
        self.config = {}
        self.stats = {
            "sent": 0,
            "received": 0,
            "start_time": 0,
            "end_time": 0,
            "messages": {}
        }
        self.test_running = False
        
    def setup(self, config):
        """Set up the MQTT client with the given configuration"""
        self.config = config
        self.stats = {
            "sent": 0,
            "received": 0,
            "start_time": 0,
            "end_time": 0,
            "messages": {}
        }
        
        # Create a new client instance
        client_id = f"mqtt-qos-tester-{random.randint(0, 1000)}"
        self.client = mqtt.Client(client_id=client_id, protocol=mqtt.MQTTv5)
        
        # Set up callbacks
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.client.on_disconnect = self.on_disconnect
        self.client.on_publish = self.on_publish
        
        # Set up authentication if provided
        if self.config.get("username"):
            self.client.username_pw_set(
                self.config["username"], 
                self.config.get("password", "")
            )
            
    def run(self):
        """Main thread function - connect to broker"""
        try:
            self.client.connect(
                self.config["broker_host"],
                int(self.config["broker_port"]),
                60
            )
            self.client.loop_start()
        except Exception as e:
            self.connection_status.emit(False, str(e))
            
    def stop(self):
        """Stop the MQTT client"""
        if self.client:
            self.client.loop_stop()
            self.client.disconnect()
            
    def on_connect(self, client, userdata, flags, rc, properties=None):
        """Callback for when the client connects to the broker"""
        if rc == 0:
            self.connected = True
            self.connection_status.emit(True, "Connected successfully")
        else:
            self.connected = False
            self.connection_status.emit(False, f"Connection failed with code {rc}")
            
    def on_disconnect(self, client, userdata, rc, properties=None):
        """Callback for when the client disconnects from the broker"""
        self.connected = False
        self.connection_status.emit(False, f"Disconnected with code {rc}")
        
    def on_message(self, client, userdata, msg):
        """Callback for when a message is received"""
        try:
            payload = json.loads(msg.payload.decode())
            message_id = payload.get("id")
            
            if message_id and self.test_running:
                self.stats["received"] += 1
                self.stats["messages"][message_id]["received"] = True
                self.stats["messages"][message_id]["receive_time"] = time.time()
                
                # Emit the message to update the UI
                self.message_received.emit({
                    "id": message_id,
                    "qos": msg.qos,
                    "received": True,
                    "latency": self.stats["messages"][message_id]["receive_time"] - 
                              self.stats["messages"][message_id]["send_time"]
                })
        except Exception as e:
            print(f"Error processing message: {e}")
            
    def on_publish(self, client, userdata, mid, properties=None):
        """Callback for when a message is published"""
        self.publish_status.emit(mid, True)
        
    def start_test(self):
        """Start the QoS test"""
        if not self.connected:
            self.connection_status.emit(False, "Not connected to broker")
            return
            
        # Reset statistics
        self.stats = {
            "sent": 0,
            "received": 0,
            "start_time": time.time(),
            "end_time": 0,
            "messages": {}
        }
        
        # Subscribe to the topic
        qos = int(self.config["qos_level"])
        self.client.subscribe(self.config["topic"], qos)
        
        # Start the test
        self.test_running = True
        
        # Send messages
        for i in range(int(self.config["message_count"])):
            message_id = i + 1
            payload = {
                "id": message_id,
                "timestamp": time.time(),
                "data": self.generate_random_string(int(self.config["message_size"]) - 50)
            }
            
            # Store message info
            self.stats["messages"][message_id] = {
                "sent": True,
                "received": False,
                "send_time": time.time(),
                "receive_time": None
            }
            
            # Publish the message
            info = self.client.publish(
                self.config["topic"],
                json.dumps(payload),
                qos=qos
            )
            
            self.stats["sent"] += 1
            
        # Set a timer to end the test after a timeout
        QTimer.singleShot(5000, self.end_test)
        
    def end_test(self):
        """End the QoS test and calculate results"""
        self.test_running = False
        self.stats["end_time"] = time.time()
        self.test_complete.emit()
        
    def generate_random_string(self, length):
        """Generate a random string of the specified length"""
        chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
        return ''.join(random.choice(chars) for _ in range(length))
        
    def get_results(self):
        """Get the test results"""
        duration = self.stats["end_time"] - self.stats["start_time"]
        loss_rate = 0
        if self.stats["sent"] > 0:
            loss_rate = ((self.stats["sent"] - self.stats["received"]) / self.stats["sent"]) * 100
            
        throughput = 0
        if duration > 0:
            throughput = self.stats["received"] / duration
            
        # Calculate latencies
        latencies = []
        for msg_id, msg_data in self.stats["messages"].items():
            if msg_data["received"] and msg_data["send_time"] and msg_data["receive_time"]:
                latencies.append(msg_data["receive_time"] - msg_data["send_time"])
                
        avg_latency = sum(latencies) / len(latencies) if latencies else 0
        
        return {
            "sent": self.stats["sent"],
            "received": self.stats["received"],
            "lost": self.stats["sent"] - self.stats["received"],
            "loss_rate": loss_rate,
            "duration": duration,
            "throughput": throughput,
            "avg_latency": avg_latency,
            "latencies": latencies
        }


class MainWindow(QMainWindow):
    """Main application window"""
    def __init__(self):
        super().__init__()
        
        self.setWindowTitle("MQTT QoS Tester")
        self.setMinimumSize(800, 600)
        
        # Create the MQTT client
        self.mqtt_client = MQTTClient()
        self.mqtt_client.connection_status.connect(self.update_connection_status)
        self.mqtt_client.message_received.connect(self.update_message_status)
        self.mqtt_client.test_complete.connect(self.display_results)
        
        # Set up the UI
        self.setup_ui()
        
    def setup_ui(self):
        """Set up the user interface"""
        # Create central widget and main layout
        central_widget = QWidget()
        main_layout = QVBoxLayout(central_widget)
        
        # Create tab widget
        tab_widget = QTabWidget()
        
        # Create tabs
        connection_tab = QWidget()
        test_tab = QWidget()
        results_tab = QWidget()
        
        # Set up connection tab
        self.setup_connection_tab(connection_tab)
        
        # Set up test tab
        self.setup_test_tab(test_tab)
        
        # Set up results tab
        self.setup_results_tab(results_tab)
        
        # Add tabs to tab widget
        tab_widget.addTab(connection_tab, "Connection")
        tab_widget.addTab(test_tab, "Test Configuration")
        tab_widget.addTab(results_tab, "Results")
        
        # Add tab widget to main layout
        main_layout.addWidget(tab_widget)
        
        # Set central widget
        self.setCentralWidget(central_widget)
        
    def setup_connection_tab(self, tab):
        """Set up the connection tab"""
        layout = QVBoxLayout(tab)
        
        # Connection group
        connection_group = QGroupBox("Broker Connection")
        connection_layout = QFormLayout()
        
        # Broker host
        self.broker_host = QLineEdit("localhost")
        connection_layout.addRow("Broker Host:", self.broker_host)
        
        # Broker port
        self.broker_port = QLineEdit("1883")
        connection_layout.addRow("Broker Port:", self.broker_port)
        
        # Username
        self.username = QLineEdit()
        connection_layout.addRow("Username (optional):", self.username)
        
        # Password
        self.password = QLineEdit()
        self.password.setEchoMode(QLineEdit.EchoMode.Password)
        connection_layout.addRow("Password (optional):", self.password)
        
        # Connect button
        self.connect_button = QPushButton("Connect")
        self.connect_button.clicked.connect(self.connect_to_broker)
        
        # Disconnect button
        self.disconnect_button = QPushButton("Disconnect")
        self.disconnect_button.clicked.connect(self.disconnect_from_broker)
        self.disconnect_button.setEnabled(False)
        
        # Connection status
        self.connection_status = QLabel("Not connected")
        self.connection_status.setStyleSheet("color: red;")
        
        # Add buttons to a horizontal layout
        button_layout = QHBoxLayout()
        button_layout.addWidget(self.connect_button)
        button_layout.addWidget(self.disconnect_button)
        
        # Set the connection group layout
        connection_group.setLayout(connection_layout)
        
        # Add widgets to the tab layout
        layout.addWidget(connection_group)
        layout.addLayout(button_layout)
        layout.addWidget(self.connection_status)
        layout.addStretch()
        
    def setup_test_tab(self, tab):
        """Set up the test configuration tab"""
        layout = QVBoxLayout(tab)
        
        # Test configuration group
        test_group = QGroupBox("Test Configuration")
        test_layout = QFormLayout()
        
        # Topic
        self.topic = QLineEdit("mqtt/qos/test")
        test_layout.addRow("Topic:", self.topic)
        
        # QoS level
        self.qos_level = QComboBox()
        self.qos_level.addItems(["0 - At most once", "1 - At least once", "2 - Exactly once"])
        test_layout.addRow("QoS Level:", self.qos_level)
        
        # Message count
        self.message_count = QSpinBox()
        self.message_count.setRange(1, 1000)
        self.message_count.setValue(10)
        test_layout.addRow("Number of Messages:", self.message_count)
        
        # Message size
        self.message_size = QSpinBox()
        self.message_size.setRange(10, 10000)
        self.message_size.setValue(100)
        self.message_size.setSuffix(" bytes")
        test_layout.addRow("Message Size:", self.message_size)
        
        # Set the test group layout
        test_group.setLayout(test_layout)
        
        # Test progress group
        progress_group = QGroupBox("Test Progress")
        progress_layout = QVBoxLayout()
        
        # Progress bar
        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 100)
        self.progress_bar.setValue(0)
        
        # Status text
        self.test_status = QTextEdit()
        self.test_status.setReadOnly(True)
        self.test_status.setMaximumHeight(150)
        
        # Start test button
        self.start_test_button = QPushButton("Start Test")
        self.start_test_button.clicked.connect(self.start_test)
        self.start_test_button.setEnabled(False)
        
        # Add widgets to the progress layout
        progress_layout.addWidget(self.progress_bar)
        progress_layout.addWidget(self.test_status)
        
        # Set the progress group layout
        progress_group.setLayout(progress_layout)
        
        # Add widgets to the tab layout
        layout.addWidget(test_group)
        layout.addWidget(progress_group)
        layout.addWidget(self.start_test_button)
        
    def setup_results_tab(self, tab):
        """Set up the results tab"""
        layout = QVBoxLayout(tab)
        
        # Results group
        results_group = QGroupBox("Test Results")
        results_layout = QGridLayout()
        
        # Results labels
        self.sent_label = QLabel("0")
        self.received_label = QLabel("0")
        self.lost_label = QLabel("0")
        self.loss_rate_label = QLabel("0%")
        self.duration_label = QLabel("0 sec")
        self.throughput_label = QLabel("0 msg/sec")
        self.avg_latency_label = QLabel("0 ms")
        
        # Add labels to grid
        results_layout.addWidget(QLabel("Messages Sent:"), 0, 0)
        results_layout.addWidget(self.sent_label, 0, 1)
        results_layout.addWidget(QLabel("Messages Received:"), 1, 0)
        results_layout.addWidget(self.received_label, 1, 1)
        results_layout.addWidget(QLabel("Messages Lost:"), 2, 0)
        results_layout.addWidget(self.lost_label, 2, 1)
        results_layout.addWidget(QLabel("Loss Rate:"), 3, 0)
        results_layout.addWidget(self.loss_rate_label, 3, 1)
        results_layout.addWidget(QLabel("Test Duration:"), 4, 0)
        results_layout.addWidget(self.duration_label, 4, 1)
        results_layout.addWidget(QLabel("Throughput:"), 5, 0)
        results_layout.addWidget(self.throughput_label, 5, 1)
        results_layout.addWidget(QLabel("Average Latency:"), 6, 0)
        results_layout.addWidget(self.avg_latency_label, 6, 1)
        
        # Set the results group layout
        results_group.setLayout(results_layout)
        
        # Graph group
        graph_group = QGroupBox("Latency Graph")
        graph_layout = QVBoxLayout()
        
        # Create graph widget
        self.graph_widget = pg.PlotWidget()
        self.graph_widget.setBackground('w')
        self.graph_widget.setLabel('left', 'Latency', 'ms')
        self.graph_widget.setLabel('bottom', 'Message ID')
        
        # Add graph to layout
        graph_layout.addWidget(self.graph_widget)
        
        # Set the graph group layout
        graph_group.setLayout(graph_layout)
        
        # QoS info group
        qos_info_group = QGroupBox("QoS Information")
        qos_info_layout = QVBoxLayout()
        
        # QoS info text
        self.qos_info = QTextEdit()
        self.qos_info.setReadOnly(True)
        
        # Set default QoS info
        self.update_qos_info(0)
        
        # Add QoS info to layout
        qos_info_layout.addWidget(self.qos_info)
        
        # Set the QoS info group layout
        qos_info_group.setLayout(qos_info_layout)
        
        # Add widgets to the tab layout
        layout.addWidget(results_group)
        layout.addWidget(graph_group)
        layout.addWidget(qos_info_group)
        
        # Connect QoS level change to update QoS info
        self.qos_level.currentIndexChanged.connect(self.update_qos_info)
        
    def connect_to_broker(self):
        """Connect to the MQTT broker"""
        # Get connection details
        broker_host = self.broker_host.text()
        broker_port = self.broker_port.text()
        username = self.username.text()
        password = self.password.text()
        
        # Update status
        self.connection_status.setText("Connecting...")
        self.connection_status.setStyleSheet("color: orange;")
        
        # Set up the MQTT client
        config = {
            "broker_host": broker_host,
            "broker_port": broker_port,
            "username": username,
            "password": password
        }
        self.mqtt_client.setup(config)
        
        # Start the client thread
        self.mqtt_client.start()
        
    def disconnect_from_broker(self):
        """Disconnect from the MQTT broker"""
        self.mqtt_client.stop()
        
    def update_connection_status(self, connected, message):
        """Update the connection status display"""
        if connected:
            self.connection_status.setText(f"Connected: {message}")
            self.connection_status.setStyleSheet("color: green;")
            self.connect_button.setEnabled(False)
            self.disconnect_button.setEnabled(True)
            self.start_test_button.setEnabled(True)
        else:
            self.connection_status.setText(f"Not connected: {message}")
            self.connection_status.setStyleSheet("color: red;")
            self.connect_button.setEnabled(True)
            self.disconnect_button.setEnabled(False)
            self.start_test_button.setEnabled(False)
            
    def start_test(self):
        """Start the QoS test"""
        # Get test configuration
        topic = self.topic.text()
        qos_level = self.qos_level.currentIndex()
        message_count = self.message_count.value()
        message_size = self.message_size.value()
        
        # Update the configuration
        config = {
            "topic": topic,
            "qos_level": qos_level,
            "message_count": message_count,
            "message_size": message_size
        }
        self.mqtt_client.config.update(config)
        
        # Reset the UI
        self.progress_bar.setValue(0)
        self.test_status.clear()
        self.graph_widget.clear()
        
        # Start the test
        self.mqtt_client.start_test()
        
        # Update the UI
        self.test_status.append(f"Starting test with QoS {qos_level}...")
        self.test_status.append(f"Topic: {topic}")
        self.test_status.append(f"Sending {message_count} messages of {message_size} bytes each")
        
    def update_message_status(self, message):
        """Update the message status display"""
        message_id = message["id"]
        qos = message["qos"]
        latency = message["latency"] * 1000  # Convert to ms
        
        # Update the progress bar
        progress = (self.mqtt_client.stats["received"] / int(self.mqtt_client.config["message_count"])) * 100
        self.progress_bar.setValue(int(progress))
        
        # Update the status text
        self.test_status.append(f"Received message {message_id} with QoS {qos} (latency: {latency:.2f} ms)")
        
    def display_results(self):
        """Display the test results"""
        # Get the results
        results = self.mqtt_client.get_results()
        
        # Update the results labels
        self.sent_label.setText(str(results["sent"]))
        self.received_label.setText(str(results["received"]))
        self.lost_label.setText(str(results["lost"]))
        self.loss_rate_label.setText(f"{results['loss_rate']:.2f}%")
        self.duration_label.setText(f"{results['duration']:.2f} sec")
        self.throughput_label.setText(f"{results['throughput']:.2f} msg/sec")
        self.avg_latency_label.setText(f"{results['avg_latency'] * 1000:.2f} ms")
        
        # Update the graph
        self.update_graph(results)
        
        # Update the test status
        self.test_status.append("Test complete!")
        self.test_status.append(f"Sent: {results['sent']}, Received: {results['received']}, Lost: {results['lost']}")
        self.test_status.append(f"Loss rate: {results['loss_rate']:.2f}%")
        self.test_status.append(f"Average latency: {results['avg_latency'] * 1000:.2f} ms")
        
    def update_graph(self, results):
        """Update the latency graph"""
        # Clear the graph
        self.graph_widget.clear()
        
        # Get the latencies
        latencies = []
        message_ids = []
        
        for msg_id, msg_data in self.mqtt_client.stats["messages"].items():
            if msg_data["received"] and msg_data["send_time"] and msg_data["receive_time"]:
                latency = (msg_data["receive_time"] - msg_data["send_time"]) * 1000  # Convert to ms
                latencies.append(latency)
                message_ids.append(msg_id)
                
        # Plot the latencies
        if latencies:
            pen = pg.mkPen(color=(0, 0, 255), width=2)
            self.graph_widget.plot(message_ids, latencies, pen=pen, symbol='o', symbolSize=5)
            
    def update_qos_info(self, index):
        """Update the QoS information text"""
        qos_info = {
            0: """
            <h3>QoS 0 - At most once</h3>
            <p>The message is delivered according to the best efforts of the network. 
            Message loss can occur. This has the lowest overhead but the least reliability.</p>
            <p><b>Characteristics:</b></p>
            <ul>
                <li>No guarantee of delivery</li>
                <li>No acknowledgment</li>
                <li>Message is not stored and is not retransmitted by the sender</li>
                <li>Fastest but least reliable</li>
            </ul>
            <p><b>Use cases:</b> Environmental sensor data, status updates where occasional loss is acceptable</p>
            """,
            1: """
            <h3>QoS 1 - At least once</h3>
            <p>The message is guaranteed to be delivered at least once. However, the message may be delivered multiple times.</p>
            <p><b>Characteristics:</b></p>
            <ul>
                <li>Guaranteed to be delivered at least once</li>
                <li>May be delivered multiple times</li>
                <li>Requires acknowledgment (PUBACK)</li>
                <li>Message is stored by the sender until PUBACK is received</li>
            </ul>
            <p><b>Use cases:</b> Logging information, non-critical updates where duplicates are acceptable</p>
            """,
            2: """
            <h3>QoS 2 - Exactly once</h3>
            <p>The message is guaranteed to be delivered exactly once. This has the highest overhead due to a four-part handshake.</p>
            <p><b>Characteristics:</b></p>
            <ul>
                <li>Guaranteed to be delivered exactly once</li>
                <li>Highest overhead</li>
                <li>Requires 4-part handshake (PUBLISH, PUBREC, PUBREL, PUBCOMP)</li>
                <li>Message is stored by both sender and receiver until handshake is complete</li>
            </ul>
            <p><b>Use cases:</b> Billing applications, financial transactions, critical control commands</p>
            """
        }
        
        self.qos_info.setHtml(qos_info[index])
        
    def closeEvent(self, event):
        """Handle the window close event"""
        # Disconnect from the broker
        self.mqtt_client.stop()
        event.accept()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
