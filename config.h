#ifndef CONFIG_H
#define CONFIG_H

// ==============================
// PIN CONFIGURATION
// ==============================
const int flowSensorPin = 2;   
const int pumpRelayPin = 4;      // Pin 4 -> IN2 (Pump)
const int fanRelayPin = 5;       // Pin 5 -> IN1 (Fan)
const int sensorPowerPin = 6;
const int dht11Pin = 7;
const int hx711DoutPin = 8;
const int hx711ClkPin = 9; 
const int lampPin = 10;          // Connected to Grow Light LED
const int soilSensorPin = A0;
const int lightSensorPin = A3;    

// ==============================
// SETTINGS & LIMITS (CALIBRATED)
// ==============================
const int dryThreshold = 50;       
const int healthyThreshold = 80;   
const int darkThreshold = 350;     

const float maxTempThreshold = 34.0;   // <-- Updated from 32.0 to 34.0°C!
const float maxHumidThreshold = 87.0;  

// Your teammates will adjust these two values once they finish calibration!
const float hxCalibrationFactor = -20000.0; 
const float flowCalibrationFactor = 5.59;  

// ==============================
// SYSTEM TIMING INTERVALS
// ==============================
const unsigned long soilInterval = 3000;
const unsigned long dhtInterval = 2000;
const unsigned long hxInterval = 2000;
const unsigned long flowInterval = 1000; 
const unsigned long lcdInterval = 3000;
const unsigned long lightInterval = 200; 
const unsigned long fanInterval = 1000;

// ==============================
// AUTOMATED WATERING CONFIGURATION
// ==============================
const unsigned long pumpRunDuration = 5000;  
const unsigned long soakDuration = 30000;    
const unsigned long darkRequiredDuration = 5000; 

#endif