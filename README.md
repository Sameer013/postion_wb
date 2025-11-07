# Poistion Sensor (BNO055 + Beam Sensor)

A real-position tracking system using **ESP32** and **Boash BNO055** Orientaion sensor.
The project streams live orientation data(Roll) over **WebSockets** to a **Web Dashboard**, allowing smooth and instant updates without page reloads.

---
## Features
- **Real-time Orientaion Data** 
- **Real-time Light/Indicator** status on the dashboard
- Built using **ESP32** and **BNO055** 
- **Auto Reconnect** for WiFi and WebSocket
- Simple, responsive Web UI (works on mobile and desktop)
- **API** for fetching raw data for custom development

## How It Works

1. The **BNO055** sensor continuously reads orientation data (Roll, Pitch, Yaw).  
2. The **ESP32 WebServer** hosts a simple HTML dashboard accessible via any browser.  
3. A **WebSocket** connection streams live data from the ESP32 to the dashboard in real-time.  
4. The browser updates the displayed sensor values instantly using JavaScript.
5. (OPTIONAL) Exposed /data API for fetching raw data


### Access the Web Dashboard

1. Open **Serial Monitor** (115200 baud)  
2. After connection, you’ll see something like:
   ```
   Connected to WiFi!
   IP address: 192.168.1.100
   ```
3. Open a browser and visit:
   ```
   http://192.168.1.100/
   ```
4. You’ll see a live dashboard updating orientation data in real time!

---

##  API Data Format

The ESP32 sends data in JSON format on <IP_ADDRESS_OF_ESP32>/data 
E.g: 192.168.1.100/data
```json
{
  "location_change": 12.34,
  "led": 1,
  
}
```
## 🧑‍💻 Author

**Sameer @ Sigma e Solution Pvt Ltd**

- 💼 GitHub: [@Sameer013](https://github.com/Sameer013)  

---




