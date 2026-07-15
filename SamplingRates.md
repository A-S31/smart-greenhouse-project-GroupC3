# Sensor Sampling Rates

The sampling rates were defined based on the physical dynamics of each sensor and the rate at which environmental variables change.

| Sensor                  | Sampling Rate | Justification |
|--------------------------|---------------|---------------|
| Soil Moisture Sensor     | Every 3 s     | Soil water content changes slowly, so frequent sampling is unnecessary. |
| Temperature & Humidity (DHT11) | Every 2 s | Environmental variation is moderate; sensor response time is ~2 s. |
| Weight Sensor (HX711)    | Every 2 s     | Plant mass changes gradually; short intervals allow detection of small trends. |
| Flow Sensor (YF-S201)    | Every 1 s     | Water flow can change quickly during pump operation; requires high resolution. |
| Light Sensor (LDR)       | Every 200 ms  | Light intensity can change rapidly (shadows, lamp switching). |
| LCD Display Update       | Every 3 s     | Prevents flicker while keeping data fresh. |
