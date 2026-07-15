# System Architecture

The Smart Greenhouse system is designed around an Arduino UNO microcontroller, which serves as the central unit for sensor data collection and actuator control.

## Sensors
- **Soil Moisture Sensor**: Measures soil water content to control irrigation.
- **Light Sensor (LDR)**: Detects ambient light intensity for artificial lighting control.
- **Temperature & Humidity Sensor (DHT11)**: Monitors environmental conditions.
- **Weight Sensor (HX711 with load cell)**: Tracks plant growth and water uptake.
- **Flow Sensor (YF-S201)**: Measures water flow rate during irrigation.

## Actuators
- **Relay Module**: Controls water pump and artificial lighting.
- **LCD Display**: Provides real-time feedback of sensor readings.

## Connections
- Sensors are connected to the Arduino UNO analog and digital pins.
- The relay module is connected to digital output pins for pump and light control.
- The LCD is connected via I2C interface for efficient communication.

## Data Flow
1. Sensors collect environmental data at defined sampling rates.
2. Arduino processes raw values and applies calibration equations.
3. Thresholds are checked; if exceeded, actuators are triggered.
4. LCD displays updated sensor values and system status.
