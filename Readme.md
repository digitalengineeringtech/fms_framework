# FMS Firmware

Fuel Station Monitoring System

## Table of Contents

- [Flow](#flow)
- [VS Code Extension](#vs-code-extension)
- [Compiling Tests](#compiling-tests)
- [Usage/Examples](#usageexamples)
- [Roadmap](#roadmap)
- [Documentation](#documentation)
- [Requirement](#requirement)
- [Skills](#skills)
- [Badges](#badges)

## Flow

![App Screenshot](https://github.com/digitalengineeringtech/fms_framework/blob/main/Flow.jpg)

## VS Code Extension

To run this project, you will need to add the following extension variables to your VS Code:

- `Arduino Community Edition`
- `vscode-arduino`

## Compiling Tests

To compile tests, run the following commands:

```bash
ctrl+alt+r   # Compiling sketch
ctrl+alt+u   # Uploading sketch to FMS controller board
```

## Usage/Examples

```c++
#include <fms_header.h>

void setup() {
    fms_log_printf("Program Starting");
    fms_task_create();
}

int strcasecmp(const char *str1, const char *str2);

#include <stdio.h>
#include <strings.h>  // Required for strcasecmp

int main() {
    char str1[] = "Hello";
    char str2[] = "hello";
    
    if (strcasecmp(str1, str2) == 0) {
        printf("Strings are equal (case-insensitive)\n");
    } else {
        printf("Strings are NOT equal\n");
    }
    
    return 0;
}
```

## Roadmap

- All Fuel Protocol Support
- Add more integrations

## Documentation

[Documentation](https://github.com/digitalengineeringtech/fms_framework/blob/main/FMS_FW_Architecture_P2.0.adoc)

## Requirement

[API](https://github.com/digitalengineeringtech/fms_framework/blob/main/whatdo.md)

## Skills

Arduino, FreeRTOS, C++, MQTT, HTML, CSS

## Badges

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)
[![AGPL License](https://img.shields.io/badge/license-AGPL-blue.svg)](http://www.gnu.org/licenses/agpl-3.0)


