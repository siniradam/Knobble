#include <U8g2lib.h>
#include "SmartMenuSystem.h"
#include "WebInterface.h"

// Global variable definitions
bool IPS = true;

// Hardware objects
Arduino_DataBus *bus = new Arduino_ESP32SPI(DC_PIN, CS_PIN, SCK_PIN, MOSI_PIN, MISO_PIN);
Arduino_GFX *gfx = new Arduino_GC9A01(bus, GFX_NOT_DEFINED, rotation, IPS);
Encoder myKnob(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN);
Bounce2::Button button = Bounce2::Button();

// Web Server and Storage
WebServer server(80);
Preferences preferences;

// Global Variables
std::vector<MenuLevel> mainMenu;
String wifi_ssid = "";
String wifi_password = "";
String main_url = "";
bool ap_mode = false;

// Navigation State
MenuState currentState = MAIN_MENU;
int currentMenuIndex = 0;
int currentSubmenuIndex = 0;
int currentDeviceIndex = 0;
int currentSettingIndex = 0;
int32_t lastEncoderValue = 0;
bool inEditMode = false;

void setup()
{
    Serial.begin(115200);
    delay(2000); // Give more time for serial to initialize

    Serial.println("=== SmartKnob Hardware Debug ===");
    Serial.println("Pin Configuration:");
    Serial.printf("GFX_BL (Backlight): %d\n", GFX_BL);
    Serial.printf("DC_PIN: %d\n", DC_PIN);
    Serial.printf("CS_PIN: %d\n", CS_PIN);
    Serial.printf("SCK_PIN: %d\n", SCK_PIN);
    Serial.printf("MOSI_PIN: %d\n", MOSI_PIN);
    Serial.printf("MISO_PIN: %d\n", MISO_PIN);

    // Test individual pins before display initialization
    Serial.println("\nTesting backlight pin...");
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, LOW); // Turn backlight ON (active low)
    delay(1000);
    Serial.println("Backlight should be ON now");

    digitalWrite(GFX_BL, HIGH); // Turn backlight OFF
    delay(1000);
    Serial.println("Backlight should be OFF now");

    digitalWrite(GFX_BL, LOW); // Turn backlight ON again
    Serial.println("Backlight back ON");

    Serial.println("Starting Smart Menu System...");

    // Initialize preferences
    preferences.begin("menu_config", false);
    Serial.println("Preferences initialized");

    // Initialize display
    Serial.println("Initializing display...");
    initializeDisplay();
    Serial.println("Display initialized");

    // Test display with simple content
    // Serial.println("Testing display...");
    // testDisplay();

    // Initialize input
    initializeInput();

    // Load configuration
    loadConfiguration();

    // Initialize WiFi
    initializeWiFi();

    // Initialize web server
    initializeWebServer();

    // Load menu structure
    loadMenuStructure();

    // Display initial menu
    displayCurrentMenu();

    Serial.println("Smart Menu System initialized");
}

void loop()
{
    // Keep the loop simple for debugging
    server.handleClient();

    // Handle encoder input
    handleEncoderInput();

    // Handle button input
    handleButtonInput();

    // Simple heartbeat to show the system is running
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 5000)
    {
        Serial.println("System running... (heartbeat)");
        lastHeartbeat = millis();
    }
}

void testDisplay()
{
    Serial.println("Testing display functionality...");

    // Test backlight
    Serial.println("Testing backlight...");
#ifdef GFX_BL
    digitalWrite(GFX_BL, LOW); // Turn ON (active low)
    delay(500);
    digitalWrite(GFX_BL, HIGH); // Turn OFF
    delay(500);
    digitalWrite(GFX_BL, LOW); // Turn ON again
    Serial.println("Backlight test complete");
#endif

    // Test display drawing
    Serial.println("Testing display drawing...");

    // Clear screen with red
    gfx->fillScreen(RGB565_RED);
    delay(1000);

    // Clear screen with green
    gfx->fillScreen(RGB565_GREEN);
    delay(1000);

    // Clear screen with blue
    gfx->fillScreen(RGB565_BLUE);
    delay(1000);

    // Clear screen with black
    gfx->fillScreen(COLOR_BACKGROUND);
    delay(500);

    // Draw some text
    gfx->setTextColor(COLOR_TEXT);
    gfx->setTextSize(3);
    gfx->setCursor(20, 50);
    gfx->println("DISPLAY");
    gfx->setCursor(30, 80);
    gfx->println("WORKING!");

    gfx->setTextSize(2);
    gfx->setCursor(10, 120);
    gfx->println("SmartKnob v1.0");

    // Draw a simple rectangle
    gfx->drawRect(10, 150, 220, 50, COLOR_SELECTED);
    gfx->setCursor(50, 170);
    gfx->println("Hardware Test OK");

    Serial.println("Display test complete");
}

void initializeDisplay()
{
    Serial.println("Setting up backlight pin...");
#ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH); // Start with backlight off (active low)
    delay(100);
    digitalWrite(GFX_BL, LOW); // Turn on backlight
    Serial.println("Backlight enabled");
#endif

    Serial.println("Initializing GFX library...");

    // Check if gfx object was created successfully
    if (gfx == nullptr)
    {
        Serial.println("ERROR: GFX object is null!");
        return;
    }

    bool result = gfx->begin();
    Serial.print("GFX begin result: ");
    Serial.println(result ? "SUCCESS" : "FAILED");

    gfx->setFont(u8g2_font_7x14_tr);

    if (!result)
    {
        Serial.println("ERROR: Failed to initialize display!");
        return;
    }

    Serial.println("Setting up display parameters...");
    gfx->fillScreen(COLOR_BACKGROUND);
    gfx->setTextSize(2);
    gfx->setTextColor(COLOR_TEXT);

    Serial.println("Display initialization complete");
}

void initializeInput()
{
    button.attach(ROTARY_ENCODER_BUTTON_PIN, INPUT_PULLUP);
    button.interval(25);
    button.setPressedState(LOW);
    myKnob.write(0);
}

void loadConfiguration()
{
    wifi_ssid = preferences.getString("wifi_ssid", "");
    wifi_password = preferences.getString("wifi_password", "");
    main_url = preferences.getString("main_url", "");
    ap_mode = preferences.getBool("ap_mode", false);
}

void saveConfiguration()
{
    preferences.putString("wifi_ssid", wifi_ssid);
    preferences.putString("wifi_password", wifi_password);
    preferences.putString("main_url", main_url);
    preferences.putBool("ap_mode", ap_mode);
}

void initializeWiFi()
{
    if (ap_mode || wifi_ssid.length() == 0)
    {
        startAPMode();
    }
    else
    {
        connectToWiFi();
    }
}

void startAPMode()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP("SmartKnob", "password123");
    ap_mode = true;
    Serial.println("AP Mode started");
    Serial.println("IP: " + WiFi.softAPIP().toString());
}

void connectToWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        ap_mode = false;
        Serial.println("\nWiFi connected");
        Serial.println("IP: " + WiFi.localIP().toString());
    }
    else
    {
        Serial.println("\nFailed to connect, starting AP mode");
        startAPMode();
    }
}

void initializeWebServer()
{
    // Serve the main configuration page
    server.on("/", HTTP_GET, handleRoot);
    server.on("/config", HTTP_POST, handleConfig);
    server.on("/menu", HTTP_POST, handleMenuConfig);
    server.on("/control", HTTP_POST, handleDeviceControl);
    server.on("/status", HTTP_GET, handleStatus);

    server.begin();
    Serial.println("Web server started");
}

void loadMenuStructure()
{
    String menuJson = preferences.getString("menu_json", getDefaultMenuJson());

    DynamicJsonDocument doc(4096);
    deserializeJson(doc, menuJson);

    mainMenu.clear();

    JsonArray menuArray = doc["menu"].as<JsonArray>();
    for (JsonObject menuItem : menuArray)
    {
        MenuLevel level;
        level.name = menuItem["name"].as<String>();

        if (menuItem.containsKey("submenus"))
        {
            JsonArray submenus = menuItem["submenus"];
            for (JsonObject submenu : submenus)
            {
                Room room;
                room.name = submenu["name"].as<String>();

                if (submenu.containsKey("devices"))
                {
                    JsonArray devices = submenu["devices"];
                    for (JsonObject device : devices)
                    {
                        Device dev;
                        dev.name = device["name"].as<String>();
                        dev.type = device["type"].as<String>();
                        dev.device_id = device["device_id"].as<String>();
                        room.devices.push_back(dev);
                    }
                }
                level.rooms.push_back(room);
            }
        }

        if (menuItem.containsKey("actions"))
        {
            JsonArray actions = menuItem["actions"];
            for (JsonObject action : actions)
            {
                Request req;
                req.name = action["name"].as<String>();
                req.url = action["url"].as<String>();
                level.requests.push_back(req);
            }
        }

        mainMenu.push_back(level);
    }

    // Load settings from JSON
    if (doc.containsKey("settings"))
    {
        JsonObject settings = doc["settings"];
        if (settings.containsKey("wifi_ssid"))
        {
            wifi_ssid = settings["wifi_ssid"].as<String>();
        }
        if (settings.containsKey("wifi_password"))
        {
            wifi_password = settings["wifi_password"].as<String>();
        }
        if (settings.containsKey("main_url"))
        {
            main_url = settings["main_url"].as<String>();
        }
        saveConfiguration();
    }
}

String getDefaultMenuJson()
{
    return R"({
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
          },
          {
            "name": "Master Bedroom",
            "devices": [
              {"name": "Light Toggle", "type": "onoff", "device_id": "bedroom_light1"}
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
      "wifi_ssid": "",
      "wifi_password": "",
      "main_url": "http://yourserver.com/api"
    }
  })";
}
