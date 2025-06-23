# Knobble - Smart Knob menu system.
A menu system for ESP32 Knob with LCD display, rotary encoder navigation, and web-based configuration interface.

Since I saw [Scott Bez](https://github.com/scottbez1/smartknob)'s [Smart Knob](https://www.youtube.com/watch?v=ip641WmY4pA) project, I wanted to create a similar system that allows for easy control of smart home devices through a menu system. Joined Discord servers who were working on similar projects, and after some time.
So eventually similar products started to appear, and our bunch of similar hardware were available on the websites like AliExpress.

Eventually I've bought one a while, but couldn't find any software that would work with it. I'm not arduino savy, and the github project came with its own set of issues, and the documentation was not very clear.

So finally I started to write my own.

I've tried to design it to be modular and extensible, allowing you to add new devices and features as needed. 

Tried LVGL and Squareline Studio, but gosh it takes time, and a bit confusing for me. I just needed something right away.

# What it does?
> **Home**: `List of rooms`, then you can select a room, then `lists devices`. I gave ids to devices, what it does basically is that it `sends an HTTP request` to the server with the device id and the value. The value can be on/off, brightness, or color.

> **Requests**: List of predefined HTTP requests, which you can execute by selecting them. May be you may want to use for `triggering some web hooks`.

> **Settings**: `Switch to AP mode` to configure WiFi, view `current IP` address, and toggle between AP and Station modes.

## Hardware
The hardware is pretty generic, based on a ESP32 C3, altough seems like there are many variants of it. `Push Knob Smart Rotary` keyword will take you to the [product](https://www.aliexpress.com/p/order/index.html?spm=a2g0o.home.headerAcount.2.468a76dbxwY0Zq). (link is not affiliated, just a reference to the product I used)

This is the product I used for development:

![Push Knob Smart Rotary](https://raw.githubusercontent.com/siniradam/knobble/main/etc/smartknob.jpg)

Here is what mine looks like after flashing the firmware along with my cheap foam case:

![My Smart Knob](https://raw.githubusercontent.com/siniradam/knobble/main/etc/knobble.jpg)


## Features

### Hardware Features
- **240x240 TFT Display** using Arduino_GFX_Library with GC9A01 driver
- **Rotary Encoder Navigation** with button selection
- **WiFi Connectivity** with both Station and Access Point modes
- **Web-based Configuration Interface**

### Menu System Features
- **Hierarchical Menu Structure**: Main Menu → Submenus → Device Controls
- **Three Main Sections**:
  - **Home**: Room-based device control
  - **Requests**: Execute predefined HTTP requests
  - **Settings**: System configuration and status

### Device Control Types
1. **On/Off Devices**: Simple toggle controls (lights, TV, etc.)
2. **Brightness Control**: 0-100% brightness adjustment
3. **Color Control**: RGB color selection

### Web Interface Features
- **WiFi Configuration**: Set SSID, password, and main server URL
- **Menu Structure Configuration**: JSON-based menu definition
- **Device Control**: Direct device control from web interface
- **System Status**: View current WiFi status and IP address

## Hardware Connections

```
Display Connections: (For this specific hardware)
- GFX_BL (Backlight): Pin 8
- DC_PIN: Pin 4
- CS_PIN: Pin 10
- SCK_PIN: Pin 1
- MOSI_PIN: Pin 0
- MISO_PIN: Not connected (GFX_NOT_DEFINED)

Rotary Encoder Connections:
- ROTARY_ENCODER_A_PIN: Pin 7
- ROTARY_ENCODER_B_PIN: Pin 6
- ROTARY_ENCODER_BUTTON_PIN: Pin 9 (GPIO_NUM_9)
```

## Required Libraries

Install these libraries through the Arduino Library Manager:

1. **Arduino_GFX_Library** - For TFT display control (Honestly there isn't much documentation.)
2. **U8g2** - Arduino graphics library, 
   1. Bundled font's 1x too small 2x too big, so I used [this](https://github.com/olikraus/u8g2/wiki/fntlist12#10-pixel-height), I might change it again.
   2. If you don't want to use U8g2:
      1. Remove `#include <U8g2lib.h>` and 
      2. `gfx->setFont(u8g2_font_7x14_tr);` lines.
   3. Some text sizes can be adjusted in the `Display.cpp` file.
3. **[Bounce2](https://github.com/thomasfredericks/Bounce2)** - For button debouncing
4. **Encoder** - For rotary encoder handling (Teensyduino)
5. **ArduinoJson** - For JSON parsing and creation
6. **WiFi** - *Built-in* ESP32 library
7. **WebServer** - *Built-in* ESP32 library
8. **HTTPClient** - *Built-in* ESP32 library
9.  **Preferences** - *Built-in* ESP32 library

> All libraries should be available in the Arduino IDE Library Manager. 
> Bounce2 and Encoder libararies were in the original code, so I went with them.

## Project Structure

The code is now modularized for better organization and maintainability:

```
├── Knobble.ino                 # Main Arduino file
├── SmartMenuSystem.h           # Main header with declarations
├── WebInterface.h              # HTML interface (properly escaped)
├── WebHandlers.cpp             # Web server request handlers
├── Display.cpp                 # Display rendering functions
├── Navigation.cpp              # Menu navigation logic
├── HttpRequests.cpp            # HTTP request handling
├── README.md                   # You are here!
├── QUICKSTART.md               # Quick setup guide (AI generated)
├── menu_config_example.json    # Example menu configuration
├── example_server.py           # Python test server (This file generated by AI, not sure if it works.)
└── requirements.txt            # Python dependencies (Also AI)
```

## Setup Instructions

### 1. Hardware Setup
- Connect the TFT display and rotary encoder according to the pin configuration above
- Upload the `Knobble.ino` sketch to your ESP32 (make sure all .cpp and .h files are in the same folder)

### 2. Initial Configuration
- On first boot, the device will start in Access Point mode
- Connect to WiFi network: **SmartMenu_Config** (Password: **password123**)
- Open a web browser and navigate to: **192.168.4.1**

<img src="https://raw.githubusercontent.com/siniradam/knobble/main/etc/web-ui-1.png" width="30%" alt="Knobble Web UI 1"> <img src="https://raw.githubusercontent.com/siniradam/knobble/main/etc/web-ui-1.png" width="30%" alt="Knobble Web UI 2"> <img src="https://raw.githubusercontent.com/siniradam/knobble/main/etc/web-ui-1.png" width="30%" alt="Knobble Web UI 3">


### 3. WiFi Configuration
- Enter your WiFi SSID and password
- Set the main server URL where device control requests will be sent
- Click "Save WiFi Config" (device will restart)

### 4. Menu Structure Configuration
- Use the web interface to configure your menu structure
- Copy and modify the default JSON structure according to your needs
- Click "Save Menu Structure"

## Menu JSON Structure

```json
{
  "menu": [
    {
      "name": "Home",
      "submenus": [
        {
          "name": "Living Room",
          "devices": [
            {"name": "TV", "type": "onoff", "device_id": "tv1"},
            {"name": "Light", "type": "onoff", "device_id": "light1"},
            {"name": "Light Brightness", "type": "brightness", "device_id": "light_brightness1"},
            {"name": "Light Color", "type": "color", "device_id": "light_color1"}
          ]
        }
      ]
    },
    {
      "name": "Requests",
      "actions": [
        {"name": "Run Request 1", "url": "http://example.com/request1"},
        {"name": "Run Request 2", "url": "http://example.com/request2"}
      ]
    }
  ],
  "settings": {
    "wifi_ssid": "YourSSID",
    "wifi_password": "YourPassword",
    "main_url": "http://yourserver.com/api"
  }
}
```

### Device Types
- **onoff**: Simple on/off toggle (sends "1" for on, "0" for off)
- **brightness**: Brightness control 0-100% (sends numeric value)
- **color**: Color picker (sends hex color code like "#FF0000")

## Navigation Controls

### Rotary Encoder
- **Rotate**: Navigate through menu items
- **Press**: Select current menu item or toggle edit mode

### Menu Navigation
- **Main Menu**: Select Home, Requests, or Settings
- **Submenus**: Select rooms, requests, or back to main menu
- **Device Control**: Control individual devices or return to submenu

### Device Control
- **On/Off Devices**: Press to toggle
- **Brightness Control**: Press to enter edit mode, rotate to adjust, press to exit
- **Color Control**: Press to cycle through predefined colors

## HTTP API

### Device Control Requests
When device states change, the system sends POST requests to your main server URL:

```json
{
  "device_id": "light1",
  "type": "onoff",
  "value": "1"
}
```

### Request Types
- **onoff**: value is "1" (on) or "0" (off)
- **brightness**: value is "0" to "100"
- **color**: value is hex color code (e.g., "#FF0000")

## Web Interface Endpoints

- **GET /**: Main configuration interface
- **POST /config**: Save WiFi and server configuration
- **POST /menu**: Save menu structure
- **POST /control**: Send device control commands
- **GET /status**: Get current system status

## Current Issues
- Switching between AP and Station modes buggy.
- Saved config is not displayed on the web interface.

## Troubleshooting

Don't forget to use Espressif's board package to configure your IDE: [ESP32](https://espressif.github.io/arduino-esp32/package_esp32_index.json) I've selected my board as **ESP32C3 Dev Module**

### Connection Issues
- If WiFi connection fails, device automatically switches to AP mode
- Access point name: **SmartMenu_Config**
- Access point password: **password123**
- Access point IP: **192.168.4.1**

### Display Issues
- When the display is blank it could be the backlight. 
  - (GFX_BL pin) It took me a while to figure out that.
  - **LOW** turns the backlight on, **not HIGH**. `digitalWrite(GFX_BL, LOW)`
- Ensure the display driver (GC9A01) matches your hardware.

### Encoder Issues
- Verify encoder pin 
- Verify pull-up resistors are present (usually built into encoder modules)
- Button should be connected with internal pull-up enabled

### Memory Issues
- If the device crashes with large menu structures, reduce the menu size
- The current implementation supports moderate-sized menu structures
- Consider optimizing JSON structure for large deployments

## Extending the System

### Adding New Device Types
1. Add new type handling in `handleDeviceSelection()`
2. Update `adjustValue()` for new control types
3. Modify `displayDeviceControl()` for new display formats

### Adding New Menu Levels
1. Extend the MenuLevel structure
2. Add navigation logic in `navigateMenu()`
3. Update display functions for new menu types

### Customizing the Web Interface
- Modify the `getWebInterface()` function
- Add new endpoints in `initializeWebServer()`
- Extend the HTML/CSS/JavaScript as needed

## License

This project is open source and available under the MIT License.
