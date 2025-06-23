"""
Simple HTTP Server Example for Smart Menu System
This server receives device control requests from the Arduino menu system.
"""

from flask import Flask, request, jsonify
from flask_cors import CORS
import json
from datetime import datetime

app = Flask(__name__)
CORS(app)  # Enable CORS for web interface

# Store device states
device_states = {}

@app.route('/api', methods=['POST'])
def handle_device_control():
    """Handle device control requests from Arduino"""
    try:
        data = request.get_json()
        
        if not data:
            return jsonify({"error": "No JSON data received"}), 400
        
        device_id = data.get('device_id')
        control_type = data.get('type')
        value = data.get('value')
        
        if not all([device_id, control_type, value is not None]):
            return jsonify({"error": "Missing required fields"}), 400
        
        # Store the device state
        device_states[device_id] = {
            'type': control_type,
            'value': value,
            'timestamp': datetime.now().isoformat()
        }
        
        print(f"Device Control: {device_id} -> {control_type}: {value}")
        
        # Here you would implement actual device control logic
        # For example:
        # - Send commands to smart home systems (Home Assistant, OpenHAB, etc.)
        # - Control IoT devices directly
        # - Update databases
        # - Send notifications
        
        if control_type == 'onoff':
            print(f"  -> Turning {device_id} {'ON' if value == '1' else 'OFF'}")
        elif control_type == 'brightness':
            print(f"  -> Setting {device_id} brightness to {value}%")
        elif control_type == 'color':
            print(f"  -> Setting {device_id} color to {value}")
        
        return jsonify({
            "status": "success",
            "device_id": device_id,
            "type": control_type,
            "value": value
        })
        
    except Exception as e:
        print(f"Error handling device control: {str(e)}")
        return jsonify({"error": str(e)}), 500

@app.route('/request1', methods=['GET'])
def handle_request1():
    """Handle predefined request 1"""
    print("Executing Request 1")
    # Add your custom logic here
    return jsonify({"status": "success", "message": "Request 1 executed"})

@app.route('/request2', methods=['GET'])
def handle_request2():
    """Handle predefined request 2"""
    print("Executing Request 2")
    # Add your custom logic here
    return jsonify({"status": "success", "message": "Request 2 executed"})

@app.route('/devices', methods=['GET'])
def get_device_states():
    """Get current device states"""
    return jsonify(device_states)

@app.route('/devices/<device_id>', methods=['GET'])
def get_device_state(device_id):
    """Get specific device state"""
    if device_id in device_states:
        return jsonify(device_states[device_id])
    else:
        return jsonify({"error": "Device not found"}), 404

@app.route('/health', methods=['GET'])
def health_check():
    """Health check endpoint"""
    return jsonify({
        "status": "healthy",
        "timestamp": datetime.now().isoformat(),
        "devices_count": len(device_states)
    })

@app.route('/', methods=['GET'])
def home():
    """Simple home page showing current device states"""
    html = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Smart Menu Server</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 40px; }
            .device { padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; }
            .on { background-color: #d4edda; }
            .off { background-color: #f8d7da; }
        </style>
    </head>
    <body>
        <h1>Smart Menu Server Status</h1>
        <h2>Current Device States:</h2>
        <div id="devices"></div>
        <script>
            function loadDevices() {
                fetch('/devices')
                    .then(response => response.json())
                    .then(data => {
                        const container = document.getElementById('devices');
                        container.innerHTML = '';
                        
                        if (Object.keys(data).length === 0) {
                            container.innerHTML = '<p>No devices controlled yet.</p>';
                            return;
                        }
                        
                        for (const [deviceId, state] of Object.entries(data)) {
                            const div = document.createElement('div');
                            div.className = 'device';
                            
                            if (state.type === 'onoff') {
                                div.className += state.value === '1' ? ' on' : ' off';
                            }
                            
                            div.innerHTML = `
                                <strong>${deviceId}</strong><br>
                                Type: ${state.type}<br>
                                Value: ${state.value}<br>
                                Last Updated: ${new Date(state.timestamp).toLocaleString()}
                            `;
                            container.appendChild(div);
                        }
                    })
                    .catch(error => {
                        document.getElementById('devices').innerHTML = '<p>Error loading devices: ' + error + '</p>';
                    });
            }
            
            // Load devices on page load and refresh every 5 seconds
            loadDevices();
            setInterval(loadDevices, 5000);
        </script>
    </body>
    </html>
    """
    return html

if __name__ == '__main__':
    print("Starting Smart Menu Server...")
    print("Device control endpoint: POST /api")
    print("Device states endpoint: GET /devices")
    print("Health check endpoint: GET /health")
    print("Web interface: GET /")
    
    # Run the server
    app.run(host='0.0.0.0', port=5000, debug=True)
