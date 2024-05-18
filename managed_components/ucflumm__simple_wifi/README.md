## Simple wifi connection

[![Component Registry](https://components.espressif.com/components/ucflumm/simple_wifi/badge.svg)](https://components.espressif.com/components/ucflumm/simple_wifi)

I found it hard to start with the wifi stack in ESP-IDF because just getting the wifi working required **60+** lines of code.

This component abstracts the wifi connection overhead code making it easy to get started with projects.

Basically all you need to do is include the header file and call wifi_manager_init(ssid, pwd).

It will attempt to reconnect if the wifi wasn't disconnected manually.

**Note that this library currently doesn't have a function to close the wifi connection**

### Usage

To add to your current idf
Run ``idf.py add-dependency "ucflumm/simple_wifi=*"``

Make sure to add ``#include "simple_wifi.h"`` in the preprocessor declarations.

Just call ``wifi_manager_init(const char*ssid, const char* password);`` in your main function.

### Troubleshooting

Refer to [Espressif Docs](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html)
