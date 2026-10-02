# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

## Test Results

### 1. Linux Character-Device Driver
- Device: `/dev/virtual_meter`
- Driver loaded successfully with `insmod`.
- Device major/minor: `10, 261`
- Driver read/write operation verified.
- Result: PASS

### 2. Direct Driver Read/Write
- Written pulse count: `4093`
- Read pulse count: `4093`
- Result: PASS

### 3. Pulse Accuracy
- Expected pulses: `1000`
- Counted pulses: `1000`
- Error: `0%`
- Result: PASS

### 4. Pulse Debounce
- Valid pulses: `1000`
- Accepted pulses: `1000`
- Debounce window: `5 ms`
- Noise handling: PASS
- Result: PASS

### 5. Pulse-to-Energy Conversion
- Impulse constant: `1600 pulses/kWh`
- New pulses: `10`
- Energy: `0.006250 kWh`
- Result: PASS

### 6. Power Calculation
- Sample interval: approximately `10 seconds`
- New pulses: `10`
- Calculated power: approximately `2249 W`
- Result: PASS

### 7. Configurable Meter Parameters
- M001 and M002 configurations verified.
- Impulse constant, cost, sample interval, and cloud endpoint loaded from configuration.
- Result: PASS

### 8. Cumulative Energy Persistence
- Previous cumulative energy restored after restart.
- Example: `0.503125 kWh` + `0.006250 kWh` = `0.509375 kWh`
- Result: PASS

### 9. Edge-to-Cloud Communication
- Smart-meter agent sends JSON over HTTP using C++/libcurl.
- C++ cloud receiver returned HTTP `200`.
- M002 integration reading successfully received.
- Result: PASS

### 10. Anomaly Detection
Verified conditions include:
- NORMAL
- NO-PULSE CONDITION
- POWER SPIKE DETECTED
- TAMPER/COUNTER RESET DETECTED
- SUDDEN PULSE INCREASE
- SUDDEN POWER DROP DETECTED is implemented in the current rule set.

### 11. Forecasting
- Next-hour forecast generated from historical meter readings.
- Next-day forecast generated from historical meter readings.
- Result: PASS

### 12. Virtual and Simulated-Real Meters
- M001: virtual meter simulation verified.
- M002: simulated-real variable-load operation verified.
- Result: PASS

### 13. Terminal Dashboard
Dashboard supports:
- Meter ID
- Timestamp
- Pulse count
- Interval energy
- Cumulative energy
- Power
- Estimated cost
- Anomaly information
- Historical reading information
- Result: PASS

## Overall Status

The major Linux, C/C++, device-driver, monitoring, analytics, persistence, anomaly, forecasting, and edge-to-cloud components were tested during development.
