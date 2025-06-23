inline const char *getWebInterfaceHTML()
{
    return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Smart Menu Configuration</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { 
            font-family: Arial, sans-serif; 
            margin: 20px; 
            background: #f0f0f0; 
        }
        .container { 
            max-width: 800px; 
            margin: 0 auto; 
            background: white; 
            padding: 20px; 
            border-radius: 10px; 
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        .section { 
            margin-bottom: 30px; 
            padding: 20px; 
            border: 1px solid #ddd; 
            border-radius: 5px; 
        }
        .section h2 { 
            color: #333; 
            border-bottom: 2px solid #007bff; 
            padding-bottom: 10px; 
        }
        input, textarea, button { 
            width: 100%; 
            padding: 10px; 
            margin: 5px 0; 
            border: 1px solid #ddd; 
            border-radius: 5px; 
            box-sizing: border-box;
        }
        button { 
            background: #007bff; 
            color: white; 
            cursor: pointer; 
            border: none;
        }
        button:hover { 
            background: #0056b3; 
        }
        .status { 
            padding: 10px; 
            margin: 10px 0; 
            border-radius: 5px; 
        }
        .success { 
            background: #d4edda; 
            color: #155724; 
            border: 1px solid #c3e6cb; 
        }
        .error { 
            background: #f8d7da; 
            color: #721c24; 
            border: 1px solid #f5c6cb; 
        }
        .device-control { 
            display: flex; 
            gap: 10px; 
            align-items: center; 
            margin: 5px 0; 
        }
        .device-control label { 
            min-width: 150px; 
        }
        .device-control input, .device-control select { 
            flex: 1; 
        }
        .loading {
            display: none;
            background: #e3f2fd;
            color: #1976d2;
            border: 1px solid #bbdefb;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Smart Menu Configuration</h1>
        
        <div class="section">
            <h2>System Status</h2>
            <div id="status"></div>
            <div id="loading" class="status loading">Loading...</div>
            <button onclick="loadStatus()">Refresh Status</button>
        </div>
        
        <div class="section">
            <h2>WiFi Configuration</h2>
            <input type="text" id="wifi_ssid" placeholder="WiFi SSID">
            <input type="password" id="wifi_password" placeholder="WiFi Password">
            <input type="text" id="main_url" placeholder="Main Server URL (e.g., http://yourserver.com/api)">
            <button onclick="saveConfig()">Save WiFi Config</button>
        </div>
        
        <div class="section">
            <h2>Menu Structure Configuration</h2>
            <textarea id="menu_structure" rows="20" placeholder="Paste your menu JSON structure here..."></textarea>
            <button onclick="saveMenuStructure()">Save Menu Structure</button>
            <button onclick="loadDefaultMenu()">Load Default Menu</button>
        </div>
        
        <div class="section">
            <h2>Device Control</h2>
            <div class="device-control">
                <label>Device ID:</label>
                <input type="text" id="device_id" placeholder="e.g., light1">
            </div>
            <div class="device-control">
                <label>Control Type:</label>
                <select id="control_type">
                    <option value="onoff">On/Off</option>
                    <option value="brightness">Brightness</option>
                    <option value="color">Color</option>
                </select>
            </div>
            <div class="device-control">
                <label>Value:</label>
                <input type="text" id="control_value" placeholder="e.g., 1 (on), 0 (off), 50 (brightness), #FF0000 (color)">
            </div>
            <button onclick="controlDevice()">Send Control Command</button>
        </div>
    </div>

    <script>
        function showLoading() {
            document.getElementById('loading').style.display = 'block';
            document.getElementById('status').innerHTML = '';
        }
        
        function hideLoading() {
            document.getElementById('loading').style.display = 'none';
        }

        function loadStatus() {
            showLoading();
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    hideLoading();
                    document.getElementById('status').innerHTML = `
                        <div class="success">
                            <strong>WiFi SSID:</strong> ${data.wifi_ssid}<br>
                            <strong>IP Address:</strong> ${data.ip_address}<br>
                            <strong>AP Mode:</strong> ${data.ap_mode ? 'Yes' : 'No'}<br>
                            <strong>Main URL:</strong> ${data.main_url}
                        </div>
                    `;
                })
                .catch(error => {
                    hideLoading();
                    document.getElementById('status').innerHTML = `
                        <div class="error">Error loading status: ${error}</div>
                    `;
                });
        }

        function saveConfig() {
            const data = new FormData();
            data.append('wifi_ssid', document.getElementById('wifi_ssid').value);
            data.append('wifi_password', document.getElementById('wifi_password').value);
            data.append('main_url', document.getElementById('main_url').value);

            fetch('/config', {
                method: 'POST',
                body: data
            })
            .then(response => response.json())
            .then(data => {
                alert('Configuration saved! Device will restart.');
            })
            .catch(error => {
                alert('Error saving configuration: ' + error);
            });
        }

        function saveMenuStructure() {
            const data = new FormData();
            data.append('menu_structure', document.getElementById('menu_structure').value);

            fetch('/menu', {
                method: 'POST',
                body: data
            })
            .then(response => response.json())
            .then(data => {
                alert('Menu structure saved successfully!');
            })
            .catch(error => {
                alert('Error saving menu structure: ' + error);
            });
        }

        function controlDevice() {
            const data = new FormData();
            data.append('device_id', document.getElementById('device_id').value);
            data.append('type', document.getElementById('control_type').value);
            data.append('value', document.getElementById('control_value').value);

            fetch('/control', {
                method: 'POST',
                body: data
            })
            .then(response => response.json())
            .then(data => {
                alert('Device control command sent!');
            })
            .catch(error => {
                alert('Error sending device control: ' + error);
            });
        }

        function loadDefaultMenu() {
            const defaultMenu = {
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
                    "wifi_ssid": "YourSSID",
                    "wifi_password": "YourPassword",
                    "main_url": "https://www.postb.in/b/1750652882601-7611868069507"
                }
            };
            
            document.getElementById('menu_structure').value = JSON.stringify(defaultMenu, null, 2);
        }

        // Load status on page load
        window.onload = function() {
            loadStatus();
            loadDefaultMenu();
        };
    </script>
</body>
</html>
)rawliteral";
}
