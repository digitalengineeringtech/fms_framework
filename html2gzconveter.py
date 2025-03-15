import sys
import os
import gzip
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                            QHBoxLayout, QPushButton, QLabel, QLineEdit, 
                            QFileDialog, QTextEdit, QGroupBox, QFormLayout,
                            QMessageBox, QSplitter, QFrame)
from PyQt5.QtCore import Qt, QSize
from PyQt5.QtGui import QFont, QIcon

class HtmlToGzArrayConverter(QMainWindow):
    def __init__(self):
        super().__init__()
        self.initUI()
        
    def initUI(self):
        # Set window properties
        self.setWindowTitle('(IIH)HTML to Gzipped Array Converter for ESP32')
        self.setMinimumSize(800, 600)
        
        # Create central widget and main layout
        central_widget = QWidget()
        main_layout = QVBoxLayout(central_widget)
        
        # Create a splitter for the two main sections
        splitter = QSplitter(Qt.Vertical)
        
        # Top section - Input controls
        top_widget = QWidget()
        top_layout = QVBoxLayout(top_widget)
        
        # Input file selection
        file_group = QGroupBox("Input File")
        file_layout = QHBoxLayout()
        
        self.input_file_path = QLineEdit()
        self.input_file_path.setPlaceholderText("Select an HTML file...")
        
        browse_button = QPushButton("Browse...")
        browse_button.clicked.connect(self.browse_input_file)
        
        file_layout.addWidget(self.input_file_path)
        file_layout.addWidget(browse_button)
        file_group.setLayout(file_layout)
        
        # Output settings
        output_group = QGroupBox("Output Settings")
        output_layout = QFormLayout()
        
        self.output_file_path = QLineEdit()
        self.output_file_path.setPlaceholderText("Leave empty to use input filename with .h extension")
        
        self.variable_name = QLineEdit()
        self.variable_name.setPlaceholderText("Leave empty to generate from filename")
        
        output_layout.addRow("Output File:", self.output_file_path)
        output_layout.addRow("Variable Name:", self.variable_name)
        output_group.setLayout(output_layout)
        
        # Convert button
        convert_button = QPushButton("Convert")
        convert_button.setMinimumHeight(40)
        convert_button.clicked.connect(self.convert_file)
        
        # Add widgets to top layout
        top_layout.addWidget(file_group)
        top_layout.addWidget(output_group)
        top_layout.addWidget(convert_button)
        
        # Bottom section - Output preview
        bottom_widget = QWidget()
        bottom_layout = QVBoxLayout(bottom_widget)
        
        preview_group = QGroupBox("Generated Code Preview")
        preview_layout = QVBoxLayout()
        
        self.code_preview = QTextEdit()
        self.code_preview.setReadOnly(True)
        self.code_preview.setFont(QFont("Courier", 10))
        self.code_preview.setPlaceholderText("Generated code will appear here...")
        
        preview_layout.addWidget(self.code_preview)
        preview_group.setLayout(preview_layout)
        
        # Stats display
        stats_group = QGroupBox("Conversion Statistics")
        stats_layout = QFormLayout()
        
        self.original_size_label = QLabel("0 bytes")
        self.compressed_size_label = QLabel("0 bytes")
        self.compression_ratio_label = QLabel("0%")
        
        stats_layout.addRow("Original Size:", self.original_size_label)
        stats_layout.addRow("Compressed Size:", self.compressed_size_label)
        stats_layout.addRow("Compression Ratio:", self.compression_ratio_label)
        
        stats_group.setLayout(stats_layout)
        
        # Add widgets to bottom layout
        bottom_layout.addWidget(preview_group)
        bottom_layout.addWidget(stats_group)
        
        # Add widgets to splitter
        splitter.addWidget(top_widget)
        splitter.addWidget(bottom_widget)
        splitter.setSizes([200, 400])  # Set initial sizes
        
        # Add splitter to main layout
        main_layout.addWidget(splitter)
        
        # Set central widget
        self.setCentralWidget(central_widget)
        
    def browse_input_file(self):
        file_path, _ = QFileDialog.getOpenFileName(
            self, "Select HTML File", "", "HTML Files (*.html *.htm);;All Files (*)"
        )
        
        if file_path:
            self.input_file_path.setText(file_path)
            
            # Auto-generate output file path
            base_name = os.path.splitext(file_path)[0]
            self.output_file_path.setText(base_name + ".h")
            
            # Auto-generate variable name
            file_name = os.path.basename(base_name).replace(".", "_").replace("-", "_")
            self.variable_name.setText("gz_" + file_name)
    
    def convert_file(self):
        input_file = self.input_file_path.text()
        
        if not input_file or not os.path.isfile(input_file):
            QMessageBox.warning(self, "Error", "Please select a valid input file.")
            return
        
        # Get output file path
        output_file = self.output_file_path.text()
        if not output_file:
            output_file = os.path.splitext(input_file)[0] + ".h"
            self.output_file_path.setText(output_file)
        
        # Get variable name
        var_name = self.variable_name.text()
        if not var_name:
            base_name = os.path.splitext(os.path.basename(input_file))[0]
            var_name = "gz_" + base_name.replace(".", "_").replace("-", "_")
            self.variable_name.setText(var_name)
        # Make sure the variable name is valid C identifier
        if not var_name.isidentifier() and not all(c.isalnum() or c == '_' for c in var_name):
            QMessageBox.warning(self, "Warning", 
                       f"Variable name '{var_name}' contains invalid characters. "
                       "Using a sanitized version.")
            var_name = ''.join(c if c.isalnum() or c == '_' else '_' for c in var_name)
            self.variable_name.setText(var_name)
        try:
            # Read the input file
            with open(input_file, 'rb') as f:
                content = f.read()
            
            # Compress the content
            compressed = gzip.compress(content)
            
            # Calculate statistics
            original_size = len(content)
            compressed_size = len(compressed)
            compression_ratio = (1 - (compressed_size / original_size)) * 100
            
            # Update statistics display
            self.original_size_label.setText(f"{original_size:,} bytes")
            self.compressed_size_label.setText(f"{compressed_size:,} bytes")
            self.compression_ratio_label.setText(f"{compression_ratio:.2f}%")
            
            # Generate C code
            code = f"#include <pgmspace.h>\n\n"
            code += f"// Compressed file: {os.path.basename(input_file)}\n"
            code += f"// Original size: {original_size} bytes\n"
            code += f"// Compressed size: {compressed_size} bytes\n\n"
            
            code += f"const size_t {var_name}_len = {compressed_size};\n"
            code += f"const uint8_t {var_name}[] PROGMEM = {{\n  "
            
            # Write the byte array
            for i, byte in enumerate(compressed):
                code += f"0x{byte:02x}"
                if i < len(compressed) - 1:
                    code += ", "
                if (i + 1) % 12 == 0 and i < len(compressed) - 1:
                    code += "\n  "
            
            code += "\n};\n"
            
            # Update preview
            self.code_preview.setText(code)
            
            # Write to output file
            with open(output_file, 'w') as f:
                f.write(code)
            
            QMessageBox.information(
                self, 
                "Success", 
                f"Converted {os.path.basename(input_file)} to {os.path.basename(output_file)}\n"
                f"Original size: {original_size:,} bytes\n"
                f"Compressed size: {compressed_size:,} bytes\n"
                f"Compression ratio: {compression_ratio:.2f}%"
            )
            
        except Exception as e:
            QMessageBox.critical(self, "Error", f"Conversion failed: {str(e)}")

def main():
    app = QApplication(sys.argv)
    app.setStyle('Fusion')  # Use Fusion style for a consistent look
    window = HtmlToGzArrayConverter()
    window.show()
    sys.exit(app.exec_())

if __name__ == "__main__":
    main()