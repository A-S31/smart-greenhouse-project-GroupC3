# Calibration Equations

This file documents the calibration equations derived for each sensor in the Smart Greenhouse project.

---

## Soil Moisture Sensor
Reference points:
- Dry soil: Raw = 1002 → Moisture = 0%
- Wet soil: Raw = 360 → Moisture = 100%

Equation:


\[
Moisture(\%) \approx -0.156 \cdot Raw + 156.3
\]



---

## Light Sensor (LDR)
Reference points:
- Darkness: Raw ≈ 20 → Intensity = 0%
- Room light: Raw ≈ 900 → Intensity = 50%
- Flashlight: Raw ≈ 982 → Intensity = 100%

Equations:
- For 20 ≤ Raw ≤ 900:


\[
Intensity(\%) \approx 0.0568 \cdot Raw - 1.14
\]



- For 900 ≤ Raw ≤ 982:


\[
Intensity(\%) \approx 0.6098 \cdot Raw - 498.8
\]



---

## Temperature Sensor (DHT11)
- Direct sensor output in °C.
- Accuracy: ±1 °C.

Equation:


\[
Error(\%) = \frac{|Measured - Reference|}{Reference} \times 100
\]



---

## Humidity Sensor (DHT11)
- Direct sensor output in %RH.
- Accuracy: ±1%.

Equation:


\[
Error(\%) = \frac{|Measured - Reference|}{Reference} \times 100
\]



---

## Flow Sensor (YF-S201)
Calibration constant: 5.59 pulses per liter.

Equation:


\[
Flow(L/min) = \frac{PulseCount}{7.5}
\]



---

## Weight Sensor (HX711 + Load Cell)
Calibration factor: -20000.0 (experimentally determined).

Equation:


\[
Weight(kg) = \frac{RawValue}{2048}
\]


