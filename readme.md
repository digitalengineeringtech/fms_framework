# HTML to Gzipped Array Converter for ESP32

This is a PyQt5-based desktop tool that converts HTML files into C code containing a gzipped byte array. The generated C code is intended for use on ESP32 platforms and stores the compressed data in program memory.

## Features

- **User-Friendly Interface:** Easily select an HTML file via the file browser.
- **Automatic Output Generation:** Auto-generates the output file name (with a `.h` extension) and variable name based on the input file.
- **Compression Statistics:** Displays original size, compressed size, and compression ratio.
- **Code Preview:** Shows the generated C code in a preview pane before saving.

## Requirements

- Python 3.x  
- [PyQt5](https://pypi.org/project/PyQt5/)  
- Python standard library modules: `os`, `sys`, and `gzip`

## How It Works

1. **Select Input File:** Use the *Browse...* button to choose an HTML file.
2. **Output Settings:**  
   - The output file path is automatically set to the input file name with a `.h` extension.  
   - The variable name is auto-generated (e.g., `gz_index` for `index.html`) but can be modified.
3. **Conversion:** Clicking the *Convert* button compresses the HTML file and generates C code that can be included in your ESP32 projects.
4. **Statistics & Preview:** The UI displays the original file size, compressed size, and compression ratio along with a C code preview.

## Usage

Run the Python script to launch the application:

```
python html2gzconveter.py
```

After launching the app, follow the GUI instructions to complete the conversion.

## Example

Suppose you have an HTML file named `index.html` with the following content:

```html
<!DOCTYPE html>
<html>
<head>
  <title>Example</title>
</head>
<body>
  <h1>Welcome to the Example Page</h1>
  <p>This is a sample HTML file.</p>
</body>
</html>
```

After conversion, the tool generates a C header file (`index.h`) with gzipped data similar to:

```c
// Automatically generated file from index.html
const unsigned char gz_index[] = {
    0x1f, 0x8b, 0x08, 0x00, // ... additional data ...
};
const unsigned int gz_index_len = sizeof(gz_index);
```

## Arduino Example

Below is an example Arduino sketch that demonstrates how to serve the gzipped HTML content from the generated header file on an ESP32 using a simple web server:

````c
// filepath: d:\FMS Framework\tool\html to h file\ArduinoExample.ino
#include <WiFi.h>
#include <WebServer.h>
#include "index.h" // Include the generated header file

// Replace with your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

WebServer server(80);

void handleRoot() {
  // Serve the gzipped HTML with the proper header for gzip encoding
  server.sendHeader("Content-Encoding", "gzip");
  server.send(200, "text/html", (const char*)gz_index, gz_index_len);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
````

## License 

@2025 IIH, All rights reserved, part of fms framework

## Acknowledgements
This tool was developed using PyQt5 to provide and easy to use graphical interface for conveting HTML resources for embedded projects

## Author
Sir Thiha Kyaw, NCK