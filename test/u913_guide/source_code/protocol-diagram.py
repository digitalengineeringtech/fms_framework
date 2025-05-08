from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                            QLabel, QGroupBox, QTextEdit)
from PyQt5.QtGui import QPainter, QPen, QColor, QFont, QBrush
from PyQt5.QtCore import Qt

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
