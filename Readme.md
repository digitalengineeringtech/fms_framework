
# FMS Firmware 

fuel station monitoring system 


## Flow

![App Screenshot](https://github.com/digitalengineeringtech/fms_framework/blob/main/Flow.jpg)


## VS  code extension

To run this project, you will need to add the following extension variables to your VS Code

`Arduino Community Edition`

`vscode-arduino`


## Compiling Tests

To compile tests, run the following command

```bash
  ctrl+alt+r   (compiling sketch)
  ctrl+alt+u   (uploading sketch to fms controller board)
```


## Usage/Examples

```c++
#inlcude <fms_header.h>
void setup() {
    fms_log_printf("Program Starting");
    fms_task_create();
}
```


## Roadmap

- All Fuel Protocol Support

- Add more integrations


## Documentation

[Documentation](https://linktodocumentation)


## 🛠 Skills
Arduino,FreeRtos,C++,Mqtt,Html,CSS


## Badges



[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)
[![GPLv3 License](https://img.shields.io/badge/License-GPL%20v3-yellow.svg)](https://opensource.org/licenses/)
[![AGPL License](https://img.shields.io/badge/license-AGPL-blue.svg)](http://www.gnu.org/licenses/agpl-3.0)


