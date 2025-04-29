import sys
import time
import numpy as np
from PyQt5.QtWidgets import (QApplication, QMainWindow, QComboBox, QPushButton, 
                            QLabel, QVBoxLayout, QHBoxLayout, QWidget, QProgressBar,
                            QFrame)
from PyQt5.QtCore import QThread, pyqtSignal, Qt, QTimer
from PyQt5.QtGui import QFont, QPainter, QPen, QColor
import serial
import serial.tools.list_ports

class SignalView(QFrame):
    def __init__(self, parent=None):
        super(SignalView, self).__init__(parent)
        self.setMinimumHeight(200)
        self.setFrameStyle(QFrame.StyledPanel | QFrame.Sunken)
        self.setAutoFillBackground(True)
        
        # Set background color
        palette = self.palette()
        palette.setColor(self.backgroundRole(), Qt.white)
        self.setPalette(palette)
        
        self.signal_data = []
        
    def update_data(self, data):
        self.signal_data = data
        self.update()
        
    def paintEvent(self, event):
        super(SignalView, self).paintEvent(event)
        
        if not self.signal_data:
            return
            
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # Draw grid
        painter.setPen(QPen(QColor(200, 200, 200), 1, Qt.DashLine))
        h_step = self.width() / 10
        for i in range(1, 10):
            x = int(i * h_step)
            painter.drawLine(x, 0, x, self.height())
            
        v_step = self.height() / 4
        for i in range(1, 4):
            y = int(i * v_step)
            painter.drawLine(0, y, self.width(), y)
        
        # Draw signal
        painter.setPen(QPen(QColor(0, 0, 255), 2, Qt.SolidLine))
        
        width = self.width()
        height = self.height()
        data_len = len(self.signal_data)
        
        if data_len < 2:
            return
            
        x_scale = width / (data_len - 1)
        
        for i in range(1, data_len):
            x1 = int((i - 1) * x_scale)
            x2 = int(i * x_scale)
            y1 = int(height - self.signal_data[i-1] * (height - 20) - 10)
            y2 = int(height - self.signal_data[i] * (height - 20) - 10)
            
            # Draw horizontal line for the previous bit
            painter.drawLine(x1, y1, x2, y1)
            
            # Draw vertical line for the transition
            if i < data_len - 1 and self.signal_data[i] != self.signal_data[i+1]:
                painter.drawLine(x2, y1, x2, y2)

class BaudrateDetectorThread(QThread):
    update_signal = pyqtSignal(list)
    result_signal = pyqtSignal(int, float)
    
    def __init__(self, port):
        QThread.__init__(self)
        self.port = port
        self.running = True
        
    def run(self):
        try:
            # Open port at a high baudrate to capture raw data
            ser = serial.Serial(self.port, baudrate=115200, timeout=1)
            
            # Common baudrates to test
            baudrates = [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200]
            
            # Capture raw data
            raw_data = []
            start_time = time.time()
            
            # Collect data for 2 seconds
            while time.time() - start_time < 2 and self.running:
                if ser.in_waiting:
                    # Read a byte and convert to binary signal level
                    byte = ser.read(1)
                    for i in range(8):
                        bit = (byte[0] >> i) & 1
                        raw_data.append(bit)
                    
                    # Update the signal plot periodically
                    if len(raw_data) % 10 == 0:
                        self.update_signal.emit(raw_data[-100:] if len(raw_data) > 100 else raw_data)
                
            ser.close()
            
            if not self.running:
                return
                
            # Analyze transitions to detect baudrate
            transitions = []
            for i in range(1, len(raw_data)):
                if raw_data[i] != raw_data[i-1]:
                    transitions.append(i)
            
            if len(transitions) < 2:
                self.result_signal.emit(0, 0)
                return
                
            # Calculate time between transitions
            intervals = []
            for i in range(1, len(transitions)):
                intervals.append(transitions[i] - transitions[i-1])
            
            # Find the most common interval (mode)
            if intervals:
                # Convert to numpy array for analysis
                intervals_array = np.array(intervals)
                
                # Find the minimum interval (should correspond to bit time)
                min_interval = np.min(intervals_array[intervals_array > 1])  # Ignore very short intervals
                
                # Calculate baudrate based on sampling rate
                # Assuming our sampling rate is approximately 115200 Hz
                bit_time = min_interval / 115200
                detected_baudrate = 1 / bit_time
                
                # Find closest standard baudrate
                closest_baudrate = min(baudrates, key=lambda x: abs(x - detected_baudrate))
                confidence = 1 - abs(closest_baudrate - detected_baudrate) / detected_baudrate
                
                self.result_signal.emit(closest_baudrate, confidence)
            else:
                self.result_signal.emit(0, 0)
                
        except Exception as e:
            print(f"Error in baudrate detection: {e}")
            self.result_signal.emit(0, 0)
    
    def stop(self):
        self.running = False

class MainWindow(QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        
        self.setWindowTitle("UART Baudrate Detector")
        self.setGeometry(100, 100, 800, 600)
        
        # Create central widget and layout
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        
        # Port selection
        port_layout = QHBoxLayout()
        port_label = QLabel("Select Port:")
        port_label.setFont(QFont("Arial", 12))
        self.port_combo = QComboBox()
        self.port_combo.setFont(QFont("Arial", 12))
        self.refresh_button = QPushButton("Refresh")
        
        port_layout.addWidget(port_label)
        port_layout.addWidget(self.port_combo)
        port_layout.addWidget(self.refresh_button)
        main_layout.addLayout(port_layout)
        
        # Detect button
        self.detect_button = QPushButton("Detect Baudrate")
        self.detect_button.setFont(QFont("Arial", 14, QFont.Bold))
        self.detect_button.setMinimumHeight(50)
        main_layout.addWidget(self.detect_button)
        
        # Progress bar
        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 100)
        self.progress_bar.setValue(0)
        main_layout.addWidget(self.progress_bar)
        
        # Result display
        result_layout = QHBoxLayout()
        result_label = QLabel("Detected Baudrate:")
        result_label.setFont(QFont("Arial", 14))
        self.result_value = QLabel("N/A")
        self.result_value.setFont(QFont("Arial", 14, QFont.Bold))
        self.confidence_label = QLabel("Confidence: N/A")
        
        result_layout.addWidget(result_label)
        result_layout.addWidget(self.result_value)
        result_layout.addWidget(self.confidence_label)
        result_layout.addStretch()
        main_layout.addLayout(result_layout)
        
        # Signal visualization using custom widget (no matplotlib)
        self.signal_view = SignalView(self)
        main_layout.addWidget(self.signal_view)
        
        # Status bar
        self.statusBar().showMessage("Ready")
        
        # Connect signals
        self.refresh_button.clicked.connect(self.refresh_ports)
        self.detect_button.clicked.connect(self.start_detection)
        
        # Initialize
        self.detector_thread = None
        self.progress_timer = None
        self.refresh_ports()
    
    def refresh_ports(self):
        self.port_combo.clear()
        ports = [port.device for port in serial.tools.list_ports.comports()]
        if ports:
            self.port_combo.addItems(ports)
            self.detect_button.setEnabled(True)
            self.statusBar().showMessage(f"Found {len(ports)} ports")
        else:
            self.port_combo.addItem("No ports available")
            self.detect_button.setEnabled(False)
            self.statusBar().showMessage("No serial ports found")
    
    def start_detection(self):
        if self.detector_thread and self.detector_thread.isRunning():
            self.detector_thread.stop()
            self.detector_thread.wait()
        
        self.detect_button.setEnabled(False)
        self.progress_bar.setValue(0)
        self.result_value.setText("Detecting...")
        self.confidence_label.setText("Confidence: N/A")
        self.statusBar().showMessage("Detection in progress...")
        
        # Start progress animation
        if self.progress_timer:
            self.progress_timer.stop()
        self.progress_timer = QTimer()
        self.progress_timer.timeout.connect(self.update_progress)
        self.progress_timer.start(50)
        
        # Start detection thread
        selected_port = self.port_combo.currentText()
        self.detector_thread = BaudrateDetectorThread(selected_port)
        self.detector_thread.update_signal.connect(self.update_signal_plot)
        self.detector_thread.result_signal.connect(self.show_result)
        self.detector_thread.finished.connect(self.detection_finished)
        self.detector_thread.start()
    
    def update_signal_plot(self, data):
        self.signal_view.update_data(data)
    
    def update_progress(self):
        value = self.progress_bar.value() + 2
        if value > 100:
            value = 0
        self.progress_bar.setValue(value)
    
    def show_result(self, baudrate, confidence):
        if self.progress_timer:
            self.progress_timer.stop()
        self.progress_bar.setValue(100)
        
        if baudrate > 0:
            self.result_value.setText(f"{baudrate}")
            self.confidence_label.setText(f"Confidence: {confidence:.2f}")
            self.statusBar().showMessage(f"Detection complete: {baudrate} bps")
        else:
            self.result_value.setText("Detection failed")
            self.confidence_label.setText("Confidence: N/A")
            self.statusBar().showMessage("Detection failed")
    
    def detection_finished(self):
        self.detect_button.setEnabled(True)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())