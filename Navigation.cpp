#include "SmartMenuSystem.h"

void handleEncoderInput()
{
    int32_t currentEncoderValue = myKnob.read() / 2;
    int32_t encoderDiff = currentEncoderValue - lastEncoderValue;

    if (encoderDiff != 0)
    {
        lastEncoderValue = currentEncoderValue;

        if (!inEditMode)
        {
            navigateMenu(encoderDiff > 0 ? 1 : -1);
        }
        else
        {
            adjustValue(encoderDiff > 0 ? 1 : -1);
        }

        displayCurrentMenu();
        delay(100);
    }
}

void handleButtonInput()
{
    button.update();

    if (button.pressed())
    {
        handleMenuSelection();
        delay(200);
        displayCurrentMenu();
    }
}

void navigateMenu(int direction)
{
    switch (currentState)
    {
    case MAIN_MENU:
        currentMenuIndex = constrain(currentMenuIndex + direction, 0, mainMenu.size()); // +1 for Settings
        break;

    case SUBMENU:
        if (currentMenuIndex < mainMenu.size())
        {
            int maxIndex = mainMenu[currentMenuIndex].rooms.size() + mainMenu[currentMenuIndex].requests.size();
            currentSubmenuIndex = constrain(currentSubmenuIndex + direction, 0, maxIndex); // +1 for Back
        }
        break;

    case DEVICE_CONTROL:
        if (currentMenuIndex < mainMenu.size() && currentSubmenuIndex < mainMenu[currentMenuIndex].rooms.size())
        {
            int maxIndex = mainMenu[currentMenuIndex].rooms[currentSubmenuIndex].devices.size();
            currentDeviceIndex = constrain(currentDeviceIndex + direction, 0, maxIndex); // +1 for Back
        }
        break;

    case SETTINGS_MENU:
        currentSettingIndex = constrain(currentSettingIndex + direction, 0, 3); // 4 settings + Back
        break;
    }
}

void adjustValue(int direction)
{
    if (currentState == DEVICE_CONTROL && currentDeviceIndex < mainMenu[currentMenuIndex].rooms[currentSubmenuIndex].devices.size())
    {
        Device &device = mainMenu[currentMenuIndex].rooms[currentSubmenuIndex].devices[currentDeviceIndex];

        if (device.type == "brightness")
        {
            device.brightness = constrain(device.brightness + direction * 5, 0, 100);
            sendDeviceRequest(device.device_id, "brightness", String(device.brightness));
        }
    }
}

void handleMenuSelection()
{
    switch (currentState)
    {
    case MAIN_MENU:
        if (currentMenuIndex < mainMenu.size())
        {
            currentState = SUBMENU;
            currentSubmenuIndex = 0;
        }
        else
        {
            // Settings menu
            currentState = SETTINGS_MENU;
            currentSettingIndex = 0;
        }
        break;

    case SUBMENU:
        handleSubmenuSelection();
        break;

    case DEVICE_CONTROL:
        handleDeviceSelection();
        break;

    case SETTINGS_MENU:
        handleSettingsSelection();
        break;
    }
}

void handleSubmenuSelection()
{
    if (currentMenuIndex >= mainMenu.size())
        return;

    MenuLevel &menu = mainMenu[currentMenuIndex];
    int roomCount = menu.rooms.size();
    int requestCount = menu.requests.size();

    if (currentSubmenuIndex < roomCount)
    {
        // Room selected
        currentState = DEVICE_CONTROL;
        currentDeviceIndex = 0;
    }
    else if (currentSubmenuIndex < roomCount + requestCount)
    {
        // Request selected
        int requestIndex = currentSubmenuIndex - roomCount;
        executeRequest(menu.requests[requestIndex].url);
    }
    else
    {
        // Back selected
        currentState = MAIN_MENU;
    }
}

void handleDeviceSelection()
{
    if (currentMenuIndex >= mainMenu.size() || currentSubmenuIndex >= mainMenu[currentMenuIndex].rooms.size())
        return;

    Room &room = mainMenu[currentMenuIndex].rooms[currentSubmenuIndex];

    if (currentDeviceIndex < room.devices.size())
    {
        Device &device = room.devices[currentDeviceIndex];

        if (device.type == "onoff")
        {
            device.state = !device.state;
            sendDeviceRequest(device.device_id, "onoff", device.state ? "1" : "0");
        }
        else if (device.type == "brightness")
        {
            inEditMode = !inEditMode;
        }
        else if (device.type == "color")
        {
            // For simplicity, cycle through predefined colors
            static int colorIndex = 0;
            String colors[] = {"#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#00FFFF", "#FFFFFF"};
            colorIndex = (colorIndex + 1) % 7;
            device.color = colors[colorIndex];
            sendDeviceRequest(device.device_id, "color", device.color);
        }
    }
    else
    {
        // Back selected
        currentState = SUBMENU;
    }
}

void handleSettingsSelection()
{
    switch (currentSettingIndex)
    {
    case 0:
        // Show WiFi SSID - just display, no action
        break;
    case 1:
        // Show IP Address - just display, no action
        break;
    case 2:
        // Switch to AP mode
        ap_mode = !ap_mode;
        saveConfiguration();
        ESP.restart();
        break;
    case 3:
        // Back to main menu
        currentState = MAIN_MENU;
        break;
    }
}
