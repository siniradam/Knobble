#include "SmartMenuSystem.h"

void executeRequest(String url)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(url);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0)
        {
            Serial.println("Request executed successfully: " + String(httpResponseCode));
        }
        else
        {
            Serial.println("Request failed: " + String(httpResponseCode));
        }

        http.end();
    }
}

void sendDeviceRequest(String deviceId, String type, String value)
{
    if (main_url.length() == 0 || WiFi.status() != WL_CONNECTED)
        return;

    HTTPClient http;
    http.begin(main_url);
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument doc(200);
    doc["device_id"] = deviceId;
    doc["type"] = type;
    doc["value"] = value;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);

    if (httpResponseCode > 0)
    {
        Serial.println("Device request sent successfully: " + String(httpResponseCode));
    }
    else
    {
        Serial.println("Device request failed: " + String(httpResponseCode));
    }

    http.end();
}

void updateDeviceState(String deviceId, String type, String value)
{
    for (auto &menu : mainMenu)
    {
        for (auto &room : menu.rooms)
        {
            for (auto &device : room.devices)
            {
                if (device.device_id == deviceId)
                {
                    if (type == "onoff")
                    {
                        device.state = (value == "1");
                    }
                    else if (type == "brightness")
                    {
                        device.brightness = value.toInt();
                    }
                    else if (type == "color")
                    {
                        device.color = value;
                    }
                    return;
                }
            }
        }
    }
}
