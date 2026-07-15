#ifndef GREENHOUSE_SENSORS_H
#define GREENHOUSE_SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include <HX711.h>
#include <LiquidCrystal_I2C.h> // Included for LCD cross-referencing
#include "config.h"

// External physical objects declared here, defined in main sketch
extern LiquidCrystal_I2C lcd;
extern DHT dht11;
extern HX711 scale;

// External variables declared here, defined in main sketch
extern int currentMoisture;
extern int currentLightRaw;
extern float currentTemperature;
extern float currentHumidity;
extern float currentWeight;
extern bool scaleOnline;

extern volatile uint16_t pulseCount;
extern float flowRateInLmin;

// ==============================
// SENSOR SETUP
// ==============================
void setupSensors() {
  pinMode(sensorPowerPin, OUTPUT);
  digitalWrite(sensorPowerPin, LOW);
  pinMode(soilSensorPin, INPUT);
  pinMode(lightSensorPin, INPUT);
  
  pinMode(flowSensorPin, INPUT_PULLUP);
  
  dht11.begin();
  
  // Note: Your scale setup logic goes here.
  // For raw testing, your teammates can check scale.read() to get direct, uncalibrated ticks!
}

// ==============================
// SENSOR READ FUNCTIONS
// ==============================
void readSoilMoisture() {
  digitalWrite(sensorPowerPin, HIGH);
  delay(200);
  int raw = analogRead(soilSensorPin);
  digitalWrite(sensorPowerPin, LOW);

  // Mapping using raw boundaries
  currentMoisture = map(raw, 1002, 360, 0, 100);
  currentMoisture = constrain(currentMoisture, 0, 100);
}

void readDHT11() {
  float h = dht11.readHumidity();
  float t = dht11.readTemperature();
  if(!isnan(h) && !isnan(t)) {
    currentHumidity = h;
    currentTemperature = t;
  }
}

void readWeight() {
  scaleOnline = true; 
  // Simulated weight metrics as currently written:
  currentWeight = 0.50 + (((float)currentMoisture / 100.0) * 0.30); 
}

void calculateFlowRate() {
  noInterrupts();
  uint16_t pulses = pulseCount;
  pulseCount = 0;   
  interrupts();   
  
  flowRateInLmin = ((float)pulses) / flowCalibrationFactor;
}

#endif