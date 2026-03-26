# LIFX ESP32 Local AP

Control LIFX smart lights over the local network using the [LIFX LAN Protocol](https://lan.developer.lifx.com/) from an ESP32-based **CYD (Cheap Yellow Display)** development board.

The device connects to your home WiFi as a station and uses UDP broadcasts to discover and communicate with LIFX bulbs on the same network — no cloud, no LIFX app, no internet required.

## Status

| Feature | Status |
|---|---|
| WiFi station connection | Working |
| LIFX device discovery (GetService) | Working |
| StateService response parsing | Working |
| Device registry (in-RAM) | Working |
| Serial REPL console | Working |
| Light control (on/off/colour/brightness) | Not yet implemented |
| CYD display UI | Not yet implemented |

## Hardware

- **ESP32 CYD** (Cheap Yellow Display) — ESP32 board with an ILI9341 2.8" TFT display
- USB cable for flashing and serial monitor

## Prerequisites

- [ESP-IDF v5.x](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) installed and sourced (`idf.py` in `PATH`)
- A WiFi network with LIFX bulbs on the same subnet

## Setup

### 1. WiFi Credentials

Create `main/include/wifi_cred.h` (this file is gitignored — never commit it):

```c
#define SSID     "your-network-ssid"
#define PASSWORD "your-network-password"
```

### 2. Configure target

```bash
idf.py set-target esp32
```

### 3. Build and Flash

```bash
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Replace `/dev/ttyUSB0` with your actual serial port (e.g., `/dev/cu.usbserial-...` on macOS).

## Serial Console Commands

Once running, the REPL prompt appears (`esp32>`). Connect at 115200 baud.

| Command | Description |
|---|---|
| `hello` | Test command — prints "Hello ESP-IDF!" |
| `echo <message>` | Echoes the message back |
| `list_lights` | Lists all discovered LIFX devices (IP, service, port) |

## Project Structure

```
lifx_esp32_local_ap/
├── CMakeLists.txt
├── main/
│   ├── CMakeLists.txt
│   ├── idf_component.yml       # External dependency: ucflumm/simple_wifi
│   ├── main.c                  # Entry point, FreeRTOS task spawning
│   ├── tx_encode.c             # LIFX UDP broadcast (GetService discovery)
│   ├── rx_parser.c             # UDP receive loop and packet parsing
│   ├── data_structure.c        # Linked-list device registry
│   ├── console.c               # ESP-IDF REPL console setup
│   ├── console_cmd.c           # Console command handlers
│   └── include/
│       ├── lifx_struct_def.h   # LIFX protocol data structures
│       ├── main.h              # Shared declarations
│       └── wifi_cred.h         # WiFi credentials (gitignored — create locally)
└── README.md
```

## Dependencies

| Component | Version | Source |
|---|---|---|
| `ucflumm/simple_wifi` | >= 1.0.4 | [ESP Component Registry](https://components.espressif.com/components/ucflumm/simple_wifi) |
| ESP-IDF built-ins | — | `esp_console`, `driver`, `lwip`, `freertos`, `esp_log` |

Run `idf.py update-dependencies` if you encounter missing component errors after cloning.

## How It Works

1. On boot, the ESP32 connects to WiFi via the `simple_wifi` component.
2. A discovery task broadcasts a LIFX `GetService` (type 2) UDP packet to `255.255.255.255:56700` every 30 seconds.
3. A receiver task listens on UDP port 56700 and parses incoming `StateService` (type 512) replies, storing each responding device by IP in a RAM-based linked list.
4. The serial REPL console allows runtime inspection of discovered devices via `list_lights`.
