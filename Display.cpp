#include "SmartMenuSystem.h"

// Line height:

static uint8_t LINE_HEIGHT = 25;
static uint8_t MENU_NAME_START_X = 60;
static uint8_t MENU_NAME_START_Y = 20;
// static uint8_t MENU_NAME_SIZE = 2;

static uint8_t MENU_MAIN_SIZE = 2;
static uint8_t MENU_ITEM_START_Y = 50;
static uint8_t MENU_ITEM_REQUESTS_SIZE = 1;
static uint8_t MENU_ITEM_ROOMS_SIZE = 1;
static uint8_t MENU_ITEM_DEVICES_SIZE = 1;

void displayCurrentMenu()
{
    gfx->fillScreen(COLOR_BACKGROUND);
    gfx->setTextSize(1);

    switch (currentState)
    {
    case MAIN_MENU:
        displayMainMenu();
        break;
    case SUBMENU:
        displaySubmenu();
        break;
    case DEVICE_CONTROL:
        displayDeviceControl();
        break;
    case SETTINGS_MENU:
        displaySettingsMenu();
        break;
    }
}

void centeredText(const String &text, int y)
{
    int textWidth = 7 * text.length(); // Assuming 7 pixels per character for size 1
    int x = (gfx->width() - textWidth) / 2;
    gfx->setCursor(x, y);
    gfx->println(text);
}

void displayMainMenu()
{
    // Menu Name:
    gfx->setTextColor(COLOR_TITLE);
    gfx->setCursor(MENU_NAME_START_X, MENU_NAME_START_Y);
    centeredText("SMARTKNOB", MENU_NAME_START_Y);
    // gfx->println("MAIN MENU");

    // Main Menu Items:
    gfx->setTextSize(MENU_MAIN_SIZE);
    int y = MENU_ITEM_START_Y;
    for (int i = 0; i < mainMenu.size(); i++)
    {
        gfx->setTextColor(i == currentMenuIndex ? COLOR_SELECTED : COLOR_TEXT);
        gfx->setCursor(20, y);
        String prefix = (i == currentMenuIndex ? "> " : "  ");
        gfx->println(prefix + mainMenu[i].name);
        y += LINE_HEIGHT;
    }

    // Settings menu
    gfx->setTextColor(currentMenuIndex == mainMenu.size() ? COLOR_SELECTED : COLOR_TEXT);
    gfx->setCursor(20, y);
    String prefix = (currentMenuIndex == mainMenu.size() ? "> " : "  ");
    gfx->println(prefix + "Settings");
}

void displaySubmenu()
{
    if (currentMenuIndex >= mainMenu.size())
        return;

    MenuLevel &menu = mainMenu[currentMenuIndex];

    // Menu Name:
    gfx->setTextColor(COLOR_TITLE);
    String menuName = menu.name;
    menuName.toUpperCase();
    centeredText(menuName, MENU_NAME_START_Y);
    // gfx->setCursor(MENU_NAME_START_X, MENU_NAME_START_Y);
    // gfx->println(menuName);

    int y = MENU_ITEM_START_Y;
    int index = 0;

    // Menu Room Items:
    gfx->setTextSize(MENU_ITEM_ROOMS_SIZE);
    for (auto &room : menu.rooms)
    {
        gfx->setTextColor(index == currentSubmenuIndex ? COLOR_SELECTED : COLOR_TEXT);
        gfx->setCursor(20, y);
        String prefix = (index == currentSubmenuIndex ? "> " : "  ");
        gfx->println(prefix + room.name);
        y += LINE_HEIGHT;
        index++;
    }

    // Menu Request Items:
    gfx->setTextSize(MENU_ITEM_REQUESTS_SIZE);
    for (auto &request : menu.requests)
    {
        gfx->setTextColor(index == currentSubmenuIndex ? COLOR_SELECTED : COLOR_TEXT);
        gfx->setCursor(20, y);
        String prefix = (index == currentSubmenuIndex ? "> " : "  ");
        gfx->println(prefix + request.name);
        y += LINE_HEIGHT;
        index++;
    }

    // Back option
    gfx->setTextSize(2);
    gfx->setTextColor(index == currentSubmenuIndex ? COLOR_SELECTED : COLOR_TEXT);
    gfx->setCursor(20, y + 5);
    String prefix = (index == currentSubmenuIndex ? "> " : "  ");
    gfx->println(prefix + "Back");
}

void displayDeviceControl()
{
    if (currentMenuIndex >= mainMenu.size() || currentSubmenuIndex >= mainMenu[currentMenuIndex].rooms.size())
        return;

    Room &room = mainMenu[currentMenuIndex].rooms[currentSubmenuIndex];

    gfx->setTextColor(COLOR_TITLE);
    gfx->setCursor(MENU_NAME_START_X, MENU_NAME_START_Y);
    String roomName = room.name;
    roomName.toUpperCase();
    gfx->println(roomName);

    int y = MENU_ITEM_START_Y;
    int x = 20;
    int index = 0;

    gfx->setTextSize(MENU_ITEM_DEVICES_SIZE);
    for (auto &device : room.devices)
    {
        gfx->setTextColor(index == currentDeviceIndex ? COLOR_SELECTED : COLOR_TEXT);
        gfx->setCursor(x, y);

        String line = (index == currentDeviceIndex ? "> " : "  ") + device.name;

        if (device.type == "onoff")
        {
            line += " [" + String(device.state ? "ON" : "OFF") + "]";
            gfx->setTextColor(device.state ? COLOR_ON : COLOR_OFF);
        }
        else if (device.type == "brightness")
        {
            line += " [" + String(device.brightness) + "%]";
            if (inEditMode && index == currentDeviceIndex)
            {
                gfx->setTextColor(COLOR_SELECTED);
                line += " EDIT";
            }
        }
        else if (device.type == "color")
        {
            line += " [" + device.color + "]";
        }

        gfx->println(line);
        y += LINE_HEIGHT;
        // x -= 4; // Adjust x position for next item
        index++;
    }

    // RESET SIZE;
    gfx->setTextSize(2);

    // Back option
    gfx->setTextColor(index == currentDeviceIndex ? COLOR_SELECTED : COLOR_TEXT);
    gfx->setCursor(20, y + 5);
    String prefix = (index == currentDeviceIndex ? "> " : "  ");
    gfx->println(prefix + "Back");
}

void displaySettingsMenu()
{
    gfx->setTextColor(COLOR_TITLE);
    gfx->setCursor(MENU_NAME_START_X, MENU_NAME_START_Y);
    gfx->println("SETTINGS");

    String settings[] = {
        "WiFi: " + (ap_mode ? "AP Mode" : wifi_ssid),
        "IP: " + (ap_mode ? WiFi.softAPIP().toString() : WiFi.localIP().toString()),
        ap_mode ? "Switch to STA" : "Switch to AP",
        "Back"};

    int y = MENU_ITEM_START_Y;
    for (int i = 0; i < 4; i++)
    {
        gfx->setTextColor(i == currentSettingIndex ? COLOR_SELECTED : COLOR_TEXT);
        gfx->setCursor(20, y);
        String prefix = (i == currentSettingIndex ? "> " : "  ");
        gfx->println(prefix + settings[i]);
        y += LINE_HEIGHT;
    }
}
