#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <WebSocketsServer.h>


const char* ssid = "DIGISOL_SIGMA";
const char* password = "1234554321";



// for ip address
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// const char* serverURL = "http://192.168.1.31:5000/api/orientation";

String header;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);
const float threshold = 3.0; // degree 
const int ledPin = 15;
const int sensorPin = 33;
int sensorState = 0;


float initialHeading = 0;
float initialRoll = 0;
float initialPitch = 0;
float roll1 = 0;


void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html, body { 
      font-family: Helvetica, Arial, sans-serif;
      margin: 0;
      padding: 0;
      background: #f2f5f7;
      height: 100vh;
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      align-items: center;
    }
    header {
      width: 100%;
      background:#005bea;
      color: white;
      text-align: center;
      padding: 18px 0;
      box-shadow: 0 3px 8px rgba(0,0,0,0.25);
    }

     header h1 {
      font-size: 26px;
      margin: 0;
      font-weight: 600;
      letter-spacing: 0.5px;
    }
    
    main{
      width: 100vw;
      background: #263238;
      color: #ccc;
      text-align: center;
      padding: 12px 0;
      font-size: 13px;
      letter-spacing: 0.5px;
      box-shadow: 0 -2px 8px rgba(0,0,0,0.2);
    }
    
    main h1{
    color: #FFFFFF;
    font-weight: bold;
    }
   
    .card {
      background: #fff;
      border-radius: 16px;
      box-shadow: 0 4px 12px rgba(0,0,0,0.15);
      width: 340px;
      padding: 24px;
      text-align: center;
      transition: 0.3s ease;
    }
    h2 {
      color: #333;
      font-size: 22px;
      margin-bottom: 8px;
    }

    p {
      color: #444;
      font-size: 16px;
      margin: 10px 0;
    }

    .button { 
      background-color: #4CAF50; 
      border: none; 
      color: white; 
      padding: 16px 40px;
      text-decoration: none; 
      font-size: 28px; 
      margin: 8px 0; 
      border-radius: 8px;
      cursor: pointer; 
      transition: background 0.3s;
    }

    .yellow { 
      background-color: #ffe100ff; 
      color: black; 
    }

    .orange { 
      background-color: orange; 
      color: black; 
    }

    #connectionStatus {
      display: inline-block;
      padding: 6px 12px;
      border-radius: 6px;
      font-size: 14px;
      background-color: #ccc;
      color: black;
      margin-top: 10px;
      font-weight: bold;
    }

    .online {
      background-color: #91ff89 !important;
      color: black;
    }

    .offline {
      background-color: #ff7b7b !important;
      color: white;
    }

    #ledStatus {
      font-weight: bold;
    }

    footer {
      width: 100vw;
      background: #263238;
      color: #ccc;
      text-align: center;
      padding: 12px 0;
      font-size: 13px;
      letter-spacing: 0.5px;
      box-shadow: 0 -2px 8px rgba(0,0,0,0.2);
    }

    footer span {
      color: #00bcd4;
      font-weight: bold;
      
    }
  </style>
</head>
<body>

  <main>
    <h1>Position Sensor Dashboard</h1>
  </main>

  <div class="card">
    <h2>Magnetic Seperator - 701</h2>
    <p>Shaft Change in Degree</p>
    <p>Threshold: <b><span id="thresholdValue">3.00</span>°</b></p>
    <p><button class="button" id="rollValue">0.00°</button></p>
    <p>BEAM : <b id="beamStatus" style="color:gray;font-size:1.1rem">INACTIVE</b></p>
     <span class="led">INDICATOR Status: <b id="ledStatus" style="color:orange;">ON</b></span><br>
    <div id="connectionStatus" class="offline">OFFLINE</div>
  </div>

  <footer>
    Developed by <span>Sigma e Solution Pvt Ltd</span>. © 2025 All Rights Reserved.
  </footer>

  <script>
   let socket;
  let reconnectInterval = 5000; 
  const statusBox = document.getElementById('connectionStatus');

  function connectWebSocket() {

    socket = new WebSocket('ws://' + window.location.hostname + ':81/');
   ;

    socket.onopen = function() {
      console.log("WebSocket Connected");
      statusBox.textContent = 'ONLINE';
      statusBox.classList.remove('offline');
      statusBox.classList.add('online');
    };

    socket.onclose = function() {
      console.warn("WebSocket Disconnected. Retrying in " + reconnectInterval/1000 + "s...");
      statusBox.textContent = 'OFFLINE';
      statusBox.classList.remove('online');
      statusBox.classList.add('offline');

     
      setTimeout(connectWebSocket, reconnectInterval);
    };

    socket.onerror = function(error) {
      console.error("WebSocket Error:", error);
      socket.close(); 
    };

    socket.onmessage = function(event) {
      try {
        const data = JSON.parse(event.data);

        document.getElementById('rollValue').innerText = data.location_change.toFixed(2) + '°';
        document.getElementById('ledStatus').innerText = data.led == 1 ? 'ON' : 'OFF';
        document.getElementById('ledStatus').style.color = data.led == 1 ? 'orange' : 'gray';
        document.getElementById('beamStatus').innerText = data.beam == 0 ? 'ACTIVE' : 'INACTIVE';
        document.getElementById('beamStatus').style.color = data.beam == 0 ? 'green' : 'red';
        document.getElementById('rollValue').className = data.led == 1 ? 'orange button' : 'button';

      } catch (e) {
        console.error("Invalid JSON from WebSocket:", e);
      }
    };
  }

  connectWebSocket();

  </script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html; charset=utf-8", html);
}




void handleData() {
  String json = "{";
  json += "\"location_change\":" + String(roll1, 2) + ",";
  json += "\"led\":" + String(digitalRead(ledPin));
  json += "}";
  server.send(200, "application/json", json);
}

void sendSensorData(){
  String json = "{";
  json += "\"location_change\":" + String(roll1, 2) + ",";
  json += "\"led\":" + String(digitalRead(ledPin)) + ",";
  json += "\"beam\":" + String(digitalRead(sensorPin));
  json += "}";
  webSocket.broadcastTXT(json);
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("Client %u connected from %s\n", num, ip.toString().c_str());
      break;
    }
    case WStype_DISCONNECTED:
      Serial.printf("Client %u disconnected\n", num);
      break;
    case WStype_TEXT:
      Serial.printf("Received: %s\n", payload);
      break;
  }
}

void setup() {
  Wire.begin(21, 22);
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);

  digitalWrite(ledPin, LOW);
  // ip address
  if (!WiFi.config(local_IP, gateway, subnet)) {
  Serial.println("STA Failed to configure");
 }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  if (!bno.begin()) {
    Serial.println("Orientation sensor not detected.");
    while (1);
  }

  delay(1000);
  bno.setExtCrystalUse(true);


  Serial.println("Waiting for sensor calibration");
  uint8_t system, gyro, accel, mag;
  do {
    bno.getCalibration(&system, &gyro, &accel, &mag);
    Serial.print("Calibration - Sys:");
    Serial.print(system);
    Serial.print(", G:");
    Serial.print(gyro);
  
    Serial.println(" ");
    delay(500);
  } while (gyro < 3);

  sensors_event_t orientationData;
  bno.getEvent(&orientationData);
  initialHeading = orientationData.orientation.x;
  initialRoll = orientationData.orientation.y;
  initialPitch = orientationData.orientation.z;

  Serial.println("Initial orientation recorded.");

  
  server.on("/",handleRoot);
  server.on("/data", handleData);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  Serial.println("Web server and WebSocket started.");
}

float angleDiff(float initial, float current) {
  float diff = abs(initial - current);
  if(diff > 180) diff = 360 - diff;
  return diff;
}




void loop() {
  
  sensorState = digitalRead(sensorPin);

  sensors_event_t orientationData;
  bno.getEvent(&orientationData);
  float heading = orientationData.orientation.x;
  float roll = orientationData.orientation.y;
  float pitch = orientationData.orientation.z;

  roll1 = roll - initialRoll;

  // only bno055
  // bool orientationChanged = (angleDiff(roll, initialRoll) > threshold || abs(roll - initialRoll) > threshold);

  // with Beam sensor and bno055
  // bool orientationChanged = ((angleDiff(roll, initialRoll) > threshold || abs(roll - initialRoll) > threshold) && sensorState == 1);

  // with Beam sensor or bno055
  bool orientationChanged = ((angleDiff(roll, initialRoll) > threshold || abs(roll - initialRoll) > threshold) || sensorState == 1);

   if (orientationChanged) {
    digitalWrite(ledPin, HIGH);
    Serial.println("LED ON - Motion Detected");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("LED OFF - No Motion");
  }


  Serial.print(", Roll (y): ");
  Serial.print(roll);
  Serial.print(", Change: ");
  Serial.print(roll1);
  Serial.print(" | ");
  

  sendSensorData();
  webSocket.loop();
  server.handleClient();
  delay(500);
}