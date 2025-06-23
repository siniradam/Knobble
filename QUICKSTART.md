# Quick Start Guide - Smart Menu System

## Step 1: Hardware Setup
1. Connect your 240x240 board.
2. Make sure pins matches with your board.

## Step 2: Upload Arduino Code
1. Install required libraries in Arduino IDE:
   - Arduino_GFX_Library
   - Bounce2
   - Encoder (Teensyduino)
   - ArduinoJson
2. Open `Knobble.ino`
3. Upload to your ESP32

## Step 3: Initial Setup
1. Device starts in AP mode on first boot
2. Connect to WiFi: **SmartMenu_Config** (password: **password123**)
3. Open browser: **192.168.4.1**

## Step 4: Configure WiFi
1. Enter your WiFi credentials
2. Set server URL (e.g., `http://192.168.1.100:5000/api`)
3. Click "Save WiFi Config"
4. Device will restart and connect to your WiFi

## Step 5: Setup Test Server (Optional)
1. Install Python Flask: `pip install -r requirements.txt`
2. Run test server: `python example_server.py`
3. Server runs on port 5000

## Step 6: Configure Menu Structure
1. Access device web interface (check device IP on display)
2. Use the provided JSON example or create your own
3. Save menu structure

## Step 7: Start Using!
- **Rotate encoder**: Navigate menus
- **Press encoder**: Select/toggle items
- **Web interface**: Configure and control remotely

## Navigation Quick Reference
- **Main Menu**: Home, Requests, Settings
- **Home**: Your rooms and devices
- **Requests**: Predefined HTTP requests
- **Settings**: WiFi status, IP, AP mode toggle

## Device Control Types
- **On/Off**: Press to toggle
- **Brightness**: Press to edit, rotate to adjust, press to save
- **Color**: Press to cycle through colors

## Troubleshooting
- **No display**: Check connections and backlight pin
- **WiFi issues**: Device auto-switches to AP mode
- **Server errors**: Check server URL and network connectivity

Need help? Check the full README.md for detailed documentation!
