#include "SmartMenuSystem.h"
#include "WebInterface.h"

// Web Server Handlers
void handleRoot()
{
    String html = getWebInterfaceHTML();
    server.send(200, "text/html", html);
}

void handleConfig()
{
    if (server.hasArg("wifi_ssid"))
    {
        wifi_ssid = server.arg("wifi_ssid");
    }
    if (server.hasArg("wifi_password"))
    {
        wifi_password = server.arg("wifi_password");
    }
    if (server.hasArg("main_url"))
    {
        main_url = server.arg("main_url");
    }

    saveConfiguration();
    server.send(200, "application/json", "{\"status\":\"success\"}");

    // Restart WiFi with new credentials
    delay(1000);
    ESP.restart();
}

void handleMenuConfig()
{
    String menuJson = server.arg("menu_structure");
    preferences.putString("menu_json", menuJson);
    loadMenuStructure();
    server.send(200, "application/json", "{\"status\":\"success\"}");
}

void handleDeviceControl()
{
    String deviceId = server.arg("device_id");
    String type = server.arg("type");
    String value = server.arg("value");

    // Update local device state
    updateDeviceState(deviceId, type, value);

    // Send HTTP request to main server
    sendDeviceRequest(deviceId, type, value);

    server.send(200, "application/json", "{\"status\":\"success\"}");
}

void handleStatus()
{
    DynamicJsonDocument doc(1024);
    doc["wifi_ssid"] = ap_mode ? "AP Mode" : wifi_ssid;
    doc["ip_address"] = ap_mode ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
    doc["ap_mode"] = ap_mode;
    doc["main_url"] = main_url;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

String getWebInterface()
{
    return getWebInterfaceHTML();
}
