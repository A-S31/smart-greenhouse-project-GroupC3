#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <HX711.h>

// ==============================
// PIN CONFIGURATION (FINAL MATCH)
// ==============================
const int flowSensorPin = 2;   
const int pumpRelayPin = 4;      // Pin 4 -> IN2 (Controls Top Relay Channel 2 -> Your Pump)
const int fanRelayPin = 5;       // Pin 5 -> IN1 (Controls Bottom Relay Channel 1 -> Your Fan)
const int sensorPowerPin = 6;
const int dht11Pin = 7;
const int hx711DoutPin = 8;
const int hx711ClkPin = 9; 
const int lampPin = 10;          // Connected to Grow Light LED
const int soilSensorPin = A0;
const int lightSensorPin = A3;    

// ==============================
// OBJECTS
// ==============================
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht11(dht11Pin, DHT11);
HX711 scale;

// ==============================
// SETTINGS & LIMITS (CALIBRATED)
// ==============================
const int dryThreshold = 50;       
const int healthyThreshold = 80;   
const int darkThreshold = 350;     // Calibrated Light Threshold

// --- TEST THRESHOLDS UPDATED: TEMP AT 34°C, HUMIDITY AT 87% ---
const float maxTempThreshold = 34.0;   // Fan turns ON if Air Temp > 34°C (UPDATED)
const float maxHumidThreshold = 87.0;  // Fan turns ON if Air Humidity > 87%

const float hxCalibrationFactor = -20000.0; 
const float flowCalibrationFactor = 5.59; 

unsigned long soilTimer = 0;
unsigned long dhtTimer = 0;
unsigned long hxTimer = 0;
unsigned long flowTimer = 0;
unsigned long lcdTimer = 0;
unsigned long lightTimer = 0;      
unsigned long fanTimer = 0;

const unsigned long soilInterval = 3000;
const unsigned long dhtInterval = 2000;
const unsigned long hxInterval = 2000;
const unsigned long flowInterval = 1000; 
const unsigned long lcdInterval = 3000;
const unsigned long lightInterval = 200; // Fast checking for timing logic
const unsigned long fanInterval = 1000;

// ==============================
// AUTOMATED WATERING STATE TIMERS
// ==============================
bool isWatering = false;
bool systemNeedsWater = false; 
unsigned long pumpStartTimer = 0;
unsigned long soakTimerStart = 0;
const unsigned long pumpRunDuration = 5000;  
const unsigned long soakDuration = 30000;    

// ==============================
// SUSTAINED DARKNESS TIMERS (UPDATED TO 5 SECONDS)
// ==============================
unsigned long darkStartTime = 0;
bool trackingDarkness = false;
const unsigned long darkRequiredDuration = 5000; // Must be dark for 5 seconds

// ==============================
// VARIABLES
// ==============================
int currentMoisture = 0;
int currentLightRaw = 0;
float currentTemperature = 0;
float currentHumidity = 0;
float currentWeight = 0.0; 
bool scaleOnline = true; 
bool isFanRunning = false;

volatile uint16_t pulseCount = 0; 
float flowRateInLmin = 0.0;

byte page = 0;
bool forceFlowPageUpdate = false; 

void pulseCounterISR() {
  pulseCount++;
}

// ==============================
// SETUP
// ==============================
void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize both relay channels to HIGH (OFF)
  digitalWrite(pumpRelayPin, HIGH); 
  pinMode(pumpRelayPin, OUTPUT);
  
  digitalWrite(fanRelayPin, HIGH); 
  pinMode(fanRelayPin, OUTPUT);
  
  pinMode(lampPin, OUTPUT);
  digitalWrite(lampPin, LOW); 
  
  pinMode(sensorPowerPin, OUTPUT);
  digitalWrite(sensorPowerPin, LOW);
  pinMode(soilSensorPin, INPUT);
  pinMode(lightSensorPin, INPUT);
  
  pinMode(flowSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounterISR, RISING);

  dht11.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Greenhouse");
  lcd.setCursor(0,1);
  lcd.print("System");
  delay(2000);
  lcd.clear();

  lcdTimer = millis() - lcdInterval;
  soakTimerStart = millis() - soakDuration; 
}

// ==============================
// MAIN LOOP
// ==============================
void loop() {
  unsigned long now = millis();

  // ------------------------------
  // Sustained Light Sensor Logic (5 Second Buffer)
  // ------------------------------
  if(now - lightTimer >= lightInterval) {
    currentLightRaw = analogRead(lightSensorPin);
    
    if(currentLightRaw < darkThreshold) {
      if(!trackingDarkness) {
        darkStartTime = now;
        trackingDarkness = true;
      }
      if(now - darkStartTime >= darkRequiredDuration) {
        digitalWrite(lampPin, HIGH);
      }
    } else {
      digitalWrite(lampPin, LOW);
      trackingDarkness = false;
    }
    lightTimer = now;
  }

  // ------------------------------
  // Automated Fan Ventilation Routine (Pin 5 -> IN1 -> Bottom Relay)
  // ------------------------------
  if(now - fanTimer >= fanInterval) {
    if(currentTemperature > maxTempThreshold || currentHumidity > maxHumidThreshold) {
      digitalWrite(fanRelayPin, LOW);  // Turn Fan ON
      isFanRunning = true;
    } else {
      digitalWrite(fanRelayPin, HIGH); // Turn Fan OFF
      isFanRunning = false;
    }
    fanTimer = now;
  }

  // ------------------------------
  // Soil Sensor Reading & Boundaries
  // ------------------------------
  if(now - soilTimer >= soilInterval)
  {
    digitalWrite(sensorPowerPin, HIGH);
    delay(200);

    int raw = analogRead(soilSensorPin);
    digitalWrite(sensorPowerPin, LOW);

    currentMoisture = map(raw, 1002, 360, 0, 100);
    currentMoisture = constrain(currentMoisture, 0, 100);

    if (currentMoisture < dryThreshold) {
      systemNeedsWater = true;   
    } 
    else if (currentMoisture >= healthyThreshold) {
      systemNeedsWater = false;  
      if (isWatering) {          
        digitalWrite(pumpRelayPin, HIGH); 
        isWatering = false;
      }
    }

    soilTimer = now;
  }

  // ------------------------------
  // Smart 5-Second Pump Sequencer (Pin 4 -> IN2 -> Top Relay)
  // ------------------------------
  if (systemNeedsWater) {
    if (!isWatering) {
      if (now - soakTimerStart >= soakDuration) {
        isWatering = true;
        pumpStartTimer = now;             
        digitalWrite(pumpRelayPin, LOW); // Turn Pump ON
        
        page = 5; 
        forceFlowPageUpdate = true; 
      }
    } 
    else {
      if (now - pumpStartTimer >= pumpRunDuration) {
        digitalWrite(pumpRelayPin, HIGH); // Turn Pump OFF 
        isWatering = false;
        soakTimerStart = now;             
      }
    }
  } else {
    if (isWatering || digitalRead(pumpRelayPin) == LOW) {
      digitalWrite(pumpRelayPin, HIGH);
      isWatering = false;
    }
  }

  // ------------------------------
  // DHT11 Sensor Routine
  // ------------------------------
  if(now - dhtTimer >= dhtInterval)
  {
    float h = dht11.readHumidity();
    float t = dht11.readTemperature();
    if(!isnan(h) && !isnan(t)) {
      currentHumidity = h;
      currentTemperature = t;
    }
    dhtTimer = now;
  }

  // ------------------------------
  // Simulated Weight Metrics
  // ------------------------------
  if(now - hxTimer >= hxInterval)
  {
    scaleOnline = true; 
    currentWeight = 0.50 + (((float)currentMoisture / 100.0) * 0.30); 
    hxTimer = now;
  }

  // ------------------------------
  // Flow Sensor Telemetry
  // ------------------------------
  if(now - flowTimer >= flowInterval)
  {
    noInterrupts();
    uint16_t pulses = pulseCount;
    pulseCount = 0; 
    interrupts();   
    
    flowRateInLmin = ((float)pulses) / flowCalibrationFactor;
    
    if (isWatering) {
      page = 5;
      forceFlowPageUpdate = true;
    }
    flowTimer = now;
  }

  // ------------------------------
  // LCD Display Rotation Engine
  // ------------------------------
  if((now - lcdTimer >= lcdInterval) || forceFlowPageUpdate)
  {
    lcd.clear();
    switch(page)
    {
      case 0:
        lcd.setCursor(0,0);
        lcd.print("Soil Moisture:");
        lcd.setCursor(0,1);
        lcd.print(currentMoisture);
        lcd.print("% ");
        if(isWatering) lcd.print("[PUMPING]");
        else if(systemNeedsWater) lcd.print("[SOAKING]");
        else lcd.print("[OK]");
        page = 1;
        break;

      case 1:
        lcd.setCursor(0,0);
        lcd.print("Temp:");
        lcd.print((int)currentTemperature);
        lcd.write(byte(223));
        lcd.print("C");
        lcd.setCursor(0,1);
        lcd.print("Humidity:");
        lcd.print((int)currentHumidity);
        lcd.print("%");
        page = 2;
        break;

      case 2:
        lcd.setCursor(0,0);
        lcd.print("Plant Weight:");
        lcd.setCursor(0,1);
        lcd.print(currentWeight, 2); 
        lcd.print(" kg");
        page = 3; 
        break;

      case 3:
        lcd.setCursor(0,0);
        lcd.print("Light Level:");
        lcd.setCursor(0,1);
        lcd.print(currentLightRaw);
        if(digitalRead(lampPin) == HIGH) {
          lcd.print(" [LAMP ON]");
        } else {
          lcd.print(" [OFF]");
        }
        page = 4;
        break;

      case 4:
        lcd.setCursor(0,0);
        lcd.print("Fan Ventilation:");
        lcd.setCursor(0,1);
        if(isFanRunning) lcd.print("STATUS: RUNNING");
        else lcd.print("STATUS: IDLE");
        page = 5;
        break;

      case 5:
        lcd.setCursor(0,0);
        lcd.print("Water Flow:");
        lcd.setCursor(0,1);
        lcd.print(flowRateInLmin, 1); 
        lcd.print(" L/min");
        
        if (isWatering) {
          page = 5; 
        } else {
          page = 0;
        }
        break;
    }
    forceFlowPageUpdate = false;
    lcdTimer = now;
  }
}