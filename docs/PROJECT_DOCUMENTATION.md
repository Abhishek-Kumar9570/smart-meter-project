# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

## Project Documentation

---

## 1. System Architecture

The system follows a layered architecture:

```text
Virtual Meter
      |
      v
C++ Pulse Processor
      |
      v
FastAPI REST API
      |
      v
PostgreSQL Database
      |
      +----------------------+
      |                      |
      v                      v
Analytics Module      Forecasting Module
      |                      |
      +----------+-----------+
                 |
                 v
          React Dashboard
```

---

## 2. Main Components

### Virtual Meter

The virtual meter simulates smart-meter pulse generation using software.

Responsibilities:

- Generate meter pulses
- Maintain persistent pulse counters
- Support multiple meters
- Produce pulse-data files

Current meters:

- M001
- M002

---

### C++ Pulse Processor

File:

```text
firmware/pulse_counter.cpp
```

Responsibilities:

- Read pulse counts
- Calculate new pulses
- Convert pulses to energy
- Calculate power
- Load meter configuration
- Generate meter JSON
- Send readings to the backend
- Buffer readings when transmission fails
- Handle negative interval-pulse calculations caused by counter resets

---

### FastAPI Backend

File:

```text
backend/main.py
```

Responsibilities:

- Receive meter readings
- Validate API key
- Store readings in PostgreSQL
- Retrieve historical data
- Provide latest meter readings
- Provide analytics
- Provide meter configuration
- Provide forecasting results
- Support multiple meters

---

### PostgreSQL Database

Database:

```text
smart_meter_db
```

Main table:

```text
meter_data
```

Fields:

```text
id
meter_id
timestamp
pulse_count
impulse_constant
energy_kwh
power_w
```

---

### Analytics Module

Responsibilities:

- Average power
- Minimum power
- Maximum power
- Total energy
- Estimated cost
- High-power spike detection
- Sudden power increase detection
- Sudden power drop detection

---

### Forecasting Module

File:

```text
analytics/forecast.py
```

Forecasting method:

```text
5-reading Moving Average
```

Outputs:

```text
forecastPower
nextHourKWh
nextDayKWh
```

---

### React Dashboard

Directory:

```text
dashboard/
```

Responsibilities:

- Meter selection
- Live/offline status
- Current power
- Energy consumption
- Estimated cost
- Historical power chart
- Anomaly status
- Forecasting
- Meter configuration

Technologies:

```text
React.js
JavaScript
Vite
Recharts
CSS
```

---

## 3. Data Flow

```text
Pulse Generation
       |
       v
Pulse Count
       |
       v
Energy Calculation
       |
       v
Power Calculation
       |
       v
JSON Meter Reading
       |
       v
REST API
       |
       v
PostgreSQL
       |
       +----------------------+
       |                      |
       v                      v
Analytics              Forecasting
       |                      |
       +----------+-----------+
                  |
                  v
           React Dashboard
```

---

## 4. Meter Configuration

Configuration file:

```text
data/meter_configs.txt
```

Current configuration:

```text
M001,1600,15.0,10
M002,1600,15.0,10
```

Format:

```text
Meter ID,Impulse Constant,Cost per kWh,Measurement Interval
```

---

## 5. Energy Calculation

```text
Energy (kWh) = Pulse Count / Impulse Constant
```

Example:

```text
1600 pulses / 1600 pulses per kWh
= 1 kWh
```

---

## 6. Power Calculation

```text
Interval Energy = New Pulses / Impulse Constant

Power (W) =
(Interval Energy / Time in Hours) × 1000
```

---

## 7. API Endpoints

```text
GET  /
POST /meter-data
GET  /meter-data
GET  /analytics
GET  /meters
GET  /meter-config
GET  /power-history
GET  /latest-reading
GET  /forecast
```

---

## 8. Authentication

Meter-data ingestion uses:

```text
X-API-Key
```

The backend compares the supplied API key against the configured API key and returns HTTP 401 when the key is invalid.

---

## 9. Anomaly Detection

### High Power Spike

The latest power value is compared against recent power statistics.

### Sudden Power Increase

```text
latest_power / previous_power >= 1.75
```

### Sudden Power Drop

```text
latest_power / previous_power <= 0.40
```

---

## 10. Store-and-Forward

When the backend is unavailable:

```text
data/pending_readings.txt
```

is used as the local buffer.

Before buffering, the C++ processor checks that the JSON is non-empty and has valid opening and closing braces.

---

## 11. Testing Results

### Pulse Accuracy Test

```text
Expected pulses: 1000
Counted pulses: 1000
Error: 0%
Result: PASS
```

### Debounce Test

```text
Debounce interval: 5 ms
Valid pulses accepted
Noise pulses rejected
Result: PASS
```

### Pulse Persistence

M001 pulse count continued across virtual-meter restarts.

Example:

```text
2057 → 2072
```

---

### Energy Calculation Test

Saved meter JSON values were verified against:

```text
pulse_count / impulse_constant
```

The maximum observed difference was:

```text
0.000005 kWh
```

which was within the tested tolerance.

---

### Forecasting Tests

Tested cases:

```text
5 readings
Fewer than 5 readings
Empty input
```

All returned valid results without crashing.

---

### Database Integrity Tests

Verified:

```text
Orphan records: 0
Invalid numeric records: 0
Duplicate measurement groups: 0
Future-dated readings: 0
Unknown meter IDs: 0
```

Final stored reading count:

```text
4135
```

Meter distribution:

```text
M001 → 1360 readings
M002 → 2775 readings
```

---

## 12. Build Verification

### C++

```text
firmware/pulse_counter.cpp
```

Compiled successfully with:

```text
-Wall
-Wextra
```

with no compiler warnings.

### Python

Successfully syntax-checked:

```text
backend/main.py
analytics/forecast.py
analytics/analytics.py
```

### React

Production build completed successfully:

```text
590 modules transformed
Build completed successfully
```

---

## 13. Known Limitations

### PostgreSQL Driver Environment

On the current Windows environment, the PostgreSQL `libpq` DLL required by Psycopg is blocked by an Application Control policy.

Therefore, the FastAPI + PostgreSQL backend cannot currently be started on this machine until the required library is permitted by the organization-approved environment.

---

### Counter Reset

The C++ processor prevents negative interval-pulse calculations when the counter decreases.

However, cumulative energy is currently derived from the current cumulative pulse counter. Therefore, an actual meter-counter reset can also cause the reported cumulative energy to decrease.

---

### No-Pulse Detection

Explicit `NO_PULSE` anomaly detection is not currently implemented.

---

### Tamper Detection

Explicit tamper-detection logic is not currently implemented.

---

## 14. Future Improvements

- Reset-aware cumulative energy tracking
- Explicit no-pulse detection
- Dedicated tamper detection
- Improved secret management
- Alert notifications
- Advanced forecasting
- Cloud deployment
- Automated end-to-end testing
- Real smart-meter hardware integration

---

## 15. Project Status

The project currently includes:

- Software-based virtual smart meter
- C++ pulse processing
- Energy and power calculation
- Multi-meter support
- FastAPI REST services
- PostgreSQL persistence
- Analytics
- Anomaly detection
- Forecasting
- React dashboard
- Store-and-forward buffering
- Automated C++ validation tests

The solution is designed as a software-only smart-meter system using virtual sensors.
