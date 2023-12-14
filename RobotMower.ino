#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <AsyncTCP.h>
#include <Wire.h>

//#define PIN_RED    34
//#define PIN_GREEN  33
//#define PIN_BLUE   32 

const char* ssid = "***";
const char* password = "***";
bool bladesOn = false;

unsigned long lastToggleTime = 0;
const unsigned long toggleDelay = 300; // Delay of .3 seconds

unsigned long lastCommandTime = 0;
const unsigned long commandTimeout = 300;  // .3 seconds


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD I2C address


const char control_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Robot Lawn Mower Control</title>
<style>
  body {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
  }

  .left-container {
    display: flex;
    flex-direction: column;
    align-items: center;
  }
  
  .key-container {
    display: grid;
    grid-template-columns: repeat(3, 50px);
    grid-template-rows: repeat(3, 50px);
    gap: 5px;
    border: 8px solid; /* Initial border */
    border-radius: 10px;
    padding: 5px;
  }
  
  button {
    font-size: 2em;
    width: 50px;
    height: 50px;
    border: none;
    border-radius: 5px;
    outline: none;
    background-color: #ddd;
    color: #333;
  }
  
  .highlight {
    background-color: #ff0;
  }
  
  #upBtn { grid-column: 2; grid-row: 1; }
  #leftBtn { grid-column: 1; grid-row: 2; }
  #downBtn { grid-column: 2; grid-row: 3; }
  #rightBtn { grid-column: 3; grid-row: 2; }
  
  #serialOutputBox {
    border: 2px solid #000;
    width: 300px;
    height: 200px;
    overflow-y: auto;
    font-family: monospace;
    white-space: pre;
    margin-top: 20px;
  }

</style>
</head>
<body>

<div class="left-container">
  <div class="key-container" id="keyBorder">
    <button id="upBtn" onclick="sendCommand('forward')">↑</button>
    <button id="leftBtn" onclick="sendCommand('left')">←</button>
    <button id="downBtn" onclick="sendCommand('backward')">↓</button>
    <button id="rightBtn" onclick="sendCommand('right')">→</button>
  </div>
  <div id="serialOutput"></div>
</div>

<div id="serialOutputBox"></div>

<script>
var ws;
var reconnectInterval = 5000; // Delay in milliseconds for reconnection attempts
var reconnectTimer;

function getIPAddress() {
  fetch('/ip')
    .then(response => response.text())
    .then(ip => {
      ws = new WebSocket('ws://' + ip + '/ws');
      ws.onopen = function() {
        console.log('WebSocket Connection Opened');
        setupMessageHandler();  // Set up message handler once connection is open
        clearTimeout(reconnectTimer);  // Clear the reconnect timer if connection is successful
        reconnectTimer = null;
      };
      ws.onclose = function() {
        console.log("WebSocket Disconnected");
        attemptReconnect(); // Attempt to reconnect
      };
      ws.onerror = function(error) {
        console.error("WebSocket Error:", error);
        attemptReconnect(); // Attempt to reconnect
      };
    });
}

function attemptReconnect() {
    if (!reconnectTimer) {
        reconnectTimer = setTimeout(function() {
            console.log("Attempting to reconnect WebSocket...");
            getIPAddress(); // Attempt to reconnect
        }, reconnectInterval);
    }
}

function sendCommand(command) {
  if (ws && ws.readyState === WebSocket.OPEN) {
    ws.send(command);
  } else {
    console.error("WebSocket is not open: cannot send command");
  }
}

function unhighlightAllButtons() {
  var buttons = document.querySelectorAll('button');
  buttons.forEach(button => button.classList.remove('highlight'));
}

function highlightButton(command) {
  unhighlightAllButtons();  // Remove highlight from all buttons
  var btnId = '';
  switch(command) {
    case 'forward':
      btnId = 'upBtn';
      break;
    case 'backward':
      btnId = 'downBtn';
      break;
    case 'left':
      btnId = 'leftBtn';
      break;
    case 'right':
      btnId = 'rightBtn';
      break;
  }
  if(btnId) {
    document.getElementById(btnId).classList.add('highlight');
  }
}

document.addEventListener('keydown', function(event) {
  var command = '';
  switch(event.key) {
    case 'ArrowUp':
    case 'w':
      command = 'forward';
      break;
    case 'ArrowDown':
    case 's':
      command = 'backward';
      break;
    case 'ArrowLeft':
    case 'a':
      command = 'left';
      break;
    case 'ArrowRight':
    case 'd':
      command = 'right';
      break;
    case 'f':
    case 'F':
      command = 'toggleBlades';
      break;
  }
  if(command) {
    ws.send(command);
    highlightButton(command);
  }
});

document.addEventListener('keyup', function(event) {
  unhighlightAllButtons();
});

function checkGamepad() {
    var gamepads = navigator.getGamepads();
    for (var i = 0; i < gamepads.length; i++) {
        var gamepad = gamepads[i];
        if (gamepad) {
            var command = '';

            // Joystick Controls
            if (gamepad.axes[1] < -0.5) {
                command = 'forward';
            } else if (gamepad.axes[1] > 0.5) {
                command = 'backward';
            } else if (gamepad.axes[0] < -0.5) {
                command = 'left';
            } else if (gamepad.axes[0] > 0.5) {
                command = 'right';
            }

            // D-Pad Controls
            if (gamepad.buttons[12].pressed) {
                command = 'forward';
            } else if (gamepad.buttons[13].pressed) {
                command = 'backward';
            } else if (gamepad.buttons[14].pressed) {
                command = 'left';
            } else if (gamepad.buttons[15].pressed) {
                command = 'right';
            }

            // A and B Button Controls
            if (gamepad.buttons[0].pressed) {
                command = 'forward';
            } else if (gamepad.buttons[1].pressed) {
                command = 'backward';
            }

            //Start Button Controls
            if (gamepad.buttons[9].pressed) {  // Assuming button 9 is the start button
                ws.send("toggleBlades");
            }

            if (command) {
                ws.send(command);
                highlightButton(command);
            } else {
                ws.send('stop');
                unhighlightAllButtons();
            }
        }
    }
    requestAnimationFrame(checkGamepad);
}


function setupMessageHandler() {
  if(ws && ws.readyState === WebSocket.OPEN) {  // Check if ws is defined and connection is open
    ws.onmessage = function(event) {
      console.log('WebSocket Message:', event.data);
      document.getElementById('serialOutput').innerText = event.data;
      var serialOutputBox = document.getElementById('serialOutputBox');
      serialOutputBox.innerText += event.data + '\n';
      
      // Auto-scroll
      serialOutputBox.scrollTop = serialOutputBox.scrollHeight;
      
      // Assuming the message data is the color command
      switch(event.data) {
        case 'forward':
          setLedColorOnPage('green');
          break;
        case 'backward':
          setLedColorOnPage('red');
          break;
        case 'left':
          setLedColorOnPage('blue');
          break;
        case 'right':
          setLedColorOnPage('purple');
          break;
      }
    };
  } else {
    console.error('WebSocket is not open: cannot set message handler');
  }
}

function setLedColorOnPage(color) {
  document.getElementById('keyBorder').style.borderColor = color;
}

getIPAddress();
checkGamepad();

</script>
</body>
</html>
)rawliteral";


#define LOG_BUFFER_SIZE 1024
String logBuffer;
void addToLog(String message) {
  logBuffer += message + "\n";  // Append new log message with a newline
  if (logBuffer.length() > LOG_BUFFER_SIZE) {
    // Optional: Implement logic to trim the buffer if it exceeds a certain size
  }
}


void motorAction(String command); 
void WiFiEvent(WiFiEvent_t event);

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    Serial.println("WebSocket client connected");
  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("WebSocket client disconnected");
  } else if(type == WS_EVT_DATA){
    String msg = String((char*)data).substring(0, len); // Ensure using only 'len' characters
    Serial.print("Received message with length: ");
    Serial.println(len);
    Serial.println(msg);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cmd: ");
    lcd.print(msg);
    ws.textAll(msg);  // Send the message to all connected WebSocket clients
    motorAction(msg);  // Call motorAction with the received message
  }
}

void sendSerialData() {
  String serialData;
  while (Serial.available()) {
    serialData += (char)Serial.read();
  }
  if (serialData.length() > 0) {
    ws.textAll(serialData);  // Send the serial data to all connected WebSocket clients
  }
}

void setup(){

  pinMode(0, OUTPUT);  // ENA
  pinMode(2, OUTPUT);  // IN1
  pinMode(4, OUTPUT);  // IN2
  pinMode(12, OUTPUT);  // IN3
  pinMode(13, OUTPUT);  // IN4
  pinMode(14, OUTPUT);  // ENB
//  pinMode(PIN_RED, OUTPUT);
//  pinMode(PIN_GREEN, OUTPUT);
//  pinMode(PIN_BLUE, OUTPUT);
  pinMode(16, OUTPUT);  // Set pin 16 as an output

  WiFi.onEvent(WiFiEvent);

  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    addToLog("HTTP GET request on /");
    request->send_P(200, "text/html", control_page);
  });

  server.on("/ip", HTTP_GET, [](AsyncWebServerRequest *request){
    addToLog("HTTP GET request on /ip");
    request->send(200, "text/plain", WiFi.localIP().toString());
  });
// Endpoint to retrieve logs server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", logBuffer); }); 

  Wire.begin(21, 5);
  lcd.init();                    // Initialize the LCD
  lcd.backlight();               // Turn on the backlight
  lcd.setCursor(0, 0);           // Set the cursor to the first column, first row
  lcd.print("Robot Mower");
  lcd.setCursor(0, 1);           // Second row
  // Display "Initializing..." with a loading animation
  lcd.print("Initializing");
  for (int i = 0; i < 4; i++) {
    lcd.print(".");
    delay(1000); // Adjust the delay time to control the animation speed
    lcd.setCursor(12 + i, 1); // Position the cursor after "Initializing"
  }

  server.begin();

  delay (4000);
}


void updateLCDWithWiFiStatus() {
  lcd.setCursor(0, 1);  // Set cursor to the second row
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("IP: ");
    lcd.print(WiFi.localIP().toString());
  } else {
    lcd.print("Not Connected to WiFi");
  }
}

//void setLEDColor(int red, int green, int blue) {
//  analogWrite(PIN_RED, red);
//  analogWrite(PIN_GREEN, green);
//  analogWrite(PIN_BLUE, blue);
//}


void motorAction(String command) {
  unsigned long currentTime = millis();
  lastCommandTime = millis();  // Update the last command time
  if (command == "toggleBlades") {
    toggleBlades(currentTime); // Call toggleBlades with the current time
  }
  if(command == "forward") {
    // Previously Right - Adjust to make vehicle go forward
    digitalWrite(2, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    analogWrite(0, 255);  // full speed
    analogWrite(14, 255);  // full speed
//    setLEDColor(255, 0, 255);  // Purple
  }
  else if(command == "backward") {
    // Previously Left - Adjust to make vehicle go backward
    digitalWrite(2, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    analogWrite(0, 255);  // full speed
    analogWrite(14, 255);  // full speed
//    setLEDColor(0, 0, 255);  // Blue
  }
  else if(command == "left") {
    // Previously Backward - Adjust to make vehicle turn left
    digitalWrite(2, LOW);
    digitalWrite(4, HIGH);
    digitalWrite(12, HIGH);
    digitalWrite(13, LOW);
    analogWrite(0, 255);  // full speed
    analogWrite(14, 255);  // full speed
//    setLEDColor(255, 0, 0);  // Red
  }
  else if(command == "right") {
    // Previously Forward - Adjust to make vehicle turn right
    digitalWrite(2, HIGH);
    digitalWrite(4, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, HIGH);
    analogWrite(0, 255);  // full speed
    analogWrite(14, 255);  // full speed
//    setLEDColor(0, 255, 0);  // Green
  }
  else if(command == "stop") {
    // Stop Motor A and B
    analogWrite(0, 0);  // 0 speed
    analogWrite(14, 0);  // 0 speed
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
  }
}

void toggleBlades(unsigned long currentTime) {
  if (currentTime - lastToggleTime >= toggleDelay) {
    // Toggle blade state
    digitalWrite(16, !bladesOn);
    bladesOn = !bladesOn; // Update state
    lastToggleTime = currentTime; // Update the last toggle time
  }
}

void WiFiEvent(WiFiEvent_t event) {
    switch(event) {
        case SYSTEM_EVENT_STA_DISCONNECTED:
            addToLog("WiFi disconnected");
            Serial.println("WiFi lost connection");
            stopAllMotors();
            lcd.clear();
            lcd.setCursor(0, 0);        
            lcd.print("WiFi Disconnected");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            addToLog("WiFi disconnected");
            Serial.println("Connected to WiFi");
            lcd.clear();
            lcd.setCursor(0, 0);        
            lcd.print("Connected to WiFi");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            addToLog("WiFi disconnected");
            Serial.print("Got IP: ");
            Serial.println(WiFi.localIP());
            break;
        // Add additional case statements for other events if needed
        default:
            break;
    }
}

void stopAllMotors() {
    // Code to stop all motors
    analogWrite(0, 0);  // 0 speed
    analogWrite(14, 0);  // 0 speed
    digitalWrite(2, LOW);
    digitalWrite(4, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    digitalWrite(16, LOW); 
//  Reset LED color or turn off
//    setLEDColor(0, 0, 0);
}

void loop() {
  sendSerialData();
  // Check for command timeout
  if (millis() - lastCommandTime > commandTimeout) {
    stopAllMotors();  // Call function to stop all motors
    lastCommandTime = millis();  // Reset the last command time
  }
  updateLCDWithWiFiStatus();
}

