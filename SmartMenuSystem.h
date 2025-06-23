#pragma once

#include <Arduino_GFX_Library.h>
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <Encoder.h>
#include <Bounce2.h>

// Display and Input Pins
#define GFX_BL 8
#define ROTARY_ENCODER_A_PIN 7
#define ROTARY_ENCODER_B_PIN 6
#define ROTARY_ENCODER_BUTTON_PIN GPIO_NUM_9
#define DC_PIN 4
#define CS_PIN 10
#define SCK_PIN 1
#define MOSI_PIN 0
#define MISO_PIN GFX_NOT_DEFINED

// Display Configuration
static int32_t rotation = 2;
static uint32_t screenWidth = 240;
static uint32_t screenHeight = 240;
extern bool IPS;

// Colors
#define COLOR_BACKGROUND RGB565_BLACK
#define COLOR_TEXT RGB565_WHITE
#define COLOR_SELECTED RGB565_CYAN
#define COLOR_TITLE RGB565_YELLOW
#define COLOR_ON RGB565_GREEN
#define COLOR_OFF RGB565_RED

// Menu System Structures
struct Device
{
    String name;
    String type; // "onoff", "brightness", "color"
    String device_id;
    bool state = false;
    int brightness = 0;
    String color = "#FFFFFF";
};

struct Room
{
    String name;
    std::vector<Device> devices;
};

struct Request
{
    String name;
    String url;
};

struct MenuLevel
{
    String name;
    std::vector<Room> rooms;
    std::vector<Request> requests;
};

// Navigation State
enum MenuState
{
    MAIN_MENU,
    SUBMENU,
    DEVICE_CONTROL,
    SETTINGS_MENU
};

// Global variables declarations
extern Arduino_DataBus *bus;
extern Arduino_GFX *gfx;
extern Encoder myKnob;
extern Bounce2::Button button;
extern WebServer server;
extern Preferences preferences;

extern std::vector<MenuLevel> mainMenu;
extern String wifi_ssid;
extern String wifi_password;
extern String main_url;
extern bool ap_mode;

extern MenuState currentState;
extern int currentMenuIndex;
extern int currentSubmenuIndex;
extern int currentDeviceIndex;
extern int currentSettingIndex;
extern int32_t lastEncoderValue;
extern bool inEditMode;

// Function declarations
void testDisplay();
void initializeDisplay();
void initializeInput();
void initializeWiFi();
void initializeWebServer();
void loadConfiguration();
void saveConfiguration();
void startAPMode();
void connectToWiFi();
void loadMenuStructure();
String getDefaultMenuJson();
void handleEncoderInput();
void handleButtonInput();
void navigateMenu(int direction);
void adjustValue(int direction);
void handleMenuSelection();
void handleSubmenuSelection();
void handleDeviceSelection();
void handleSettingsSelection();
void executeRequest(String url);
void sendDeviceRequest(String deviceId, String type, String value);
void updateDeviceState(String deviceId, String type, String value);
void displayCurrentMenu();
void displayMainMenu();
void displaySubmenu();
void displayDeviceControl();
void displaySettingsMenu();

// Web handler functions
void handleRoot();
void handleConfig();
void handleMenuConfig();
void handleDeviceControl();
void handleStatus();
String getWebInterface();
