# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

A software-based smart-meter monitoring and analytics system that simulates smart-meter pulses, converts pulse counts into electrical energy consumption, calculates power usage, stores time-series readings, detects abnormal consumption patterns, forecasts future usage, and visualizes meter data through an interactive dashboard.

---

## 1. Project Overview

The Smart Energy Smart-Meter Pulse Counter & Analytics Agent is designed as a software-only smart-meter solution using a virtual sensor approach.

The system simulates electricity-meter pulse generation and processes the readings through a C++ pulse-processing module. The processed readings are sent to a FastAPI backend, stored in PostgreSQL, analyzed for anomalies, used for short-term forecasting, and displayed through a React.js dashboard.

The system supports multiple virtual meters, configurable meter parameters, local store-and-forward buffering, anomaly detection, historical analysis, and forecasting.

---

## 2. Objectives

- Simulate smart-meter pulse generation without physical hardware.
- Count meter pulses and convert them into energy consumption.
- Calculate power consumption from pulse intervals.
- Support multiple virtual meters.
- Store time-series meter readings in PostgreSQL.
- Provide REST APIs for meter data and analytics.
- Detect abnormal power-consumption behavior.
- Forecast future power and energy consumption.
- Provide an interactive web dashboard.
- Support local buffering when the backend is temporarily unavailable.

---

## 3. Key Features

### Virtual Smart Meter

- Software-based pulse generation.
- Persistent pulse counters.
- Multiple meter support.
- Configurable impulse constant.
- Configurable measurement interval.
- Configurable electricity cost.

### Pulse Processing

- C++ based pulse processing.
- Pulse counting.
- Pulse-to-energy conversion.
- Interval power calculation.
- Basic counter-reset protection.
- JSON generation for meter readings.

### System Programming Layer

The C++ pulse processor provides the low-level system programming layer of the project.

- File I/O using `ifstream` and `ofstream`.
- Windows OS interaction using `GetCurrentDirectoryA()` and `Sleep()`.
- Command-line argument handling using `argc` and `argv`.
- Environment-variable access using `getenv()`.
- External process execution using `system()` to invoke `curl`.
- Process return-code and error handling.
- Time measurement using `std::chrono::steady_clock`.
- Continuous meter-processing loop with controlled execution intervals.
- Local persistent buffering for failed readings.
- Store-and-forward mechanism for recovering unsent readings.

### Backend

- FastAPI REST backend.
- PostgreSQL data persistence.
- API-key authentication for meter-data ingestion.
- Meter configuration retrieval.
- Historical power-data retrieval.
- Latest-reading retrieval.
- Consumption analytics.

### Analytics

- Average power calculation.
- Minimum and maximum power calculation.
- Total energy calculation.
- Estimated electricity cost.
- High-power spike detection.
- Sudden power increase detection.
- Sudden power drop detection.
- Moving-average forecasting.

### Dashboard

- React.js based dashboard.
- Meter selection.
- Live/offline status.
- Latest power display.
- Energy consumption display.
- Estimated cost.
- Historical power chart.
- Anomaly status.
- Forecasted power.
- Next-hour energy forecast.
- Next-day energy forecast.
- Meter configuration display.

### Store-and-Forward

- Failed readings are stored locally.
- Pending readings are retained in a local queue.
- Buffered readings can be forwarded when backend connectivity is restored.
- JSON validation is applied before buffering.

---

## 4. Technology Stack

### Firmware / Processing

- C++
- GNU g++

### Backend

- Python
- FastAPI
- Uvicorn
- SQLAlchemy
- Psycopg

### Database

- PostgreSQL
- pgAdmin

### Analytics

- Python

### Frontend

- React.js
- JavaScript
- Vite
- Recharts
- HTML
- CSS

### Development Tools

- Visual Studio Code
- Git
- Command Prompt

---

## 5. System Architecture

```text
             ┌──────────────────────────┐
             │   Virtual Smart Meter   │
             │        Python            │
             └────────────┬─────────────┘
                          │
                          ▼
             ┌──────────────────────────┐
             │   Pulse Processor        │
             │          C++             │
             └────────────┬─────────────┘
                          │
                 Meter JSON Reading
                          │
                          ▼
             ┌──────────────────────────┐
             │     FastAPI Backend      │
             │        REST API          │
             └────────────┬─────────────┘
                          │
                          ▼
             ┌──────────────────────────┐
             │       PostgreSQL         │
             │     Time-Series Data     │
             └────────────┬─────────────┘
                          │
              ┌───────────┴───────────┐
              │                       │
              ▼                       ▼
   ┌────────────────────┐   ┌────────────────────┐
   │ Python Analytics   │   │ Forecasting Module │
   │ Anomaly Detection  │   │ Moving Average    │
   └──────────┬─────────┘   └──────────┬─────────┘
              │                        │
              └────────────┬───────────┘
                           ▼
                ┌──────────────────────┐
                │   React Dashboard    │
                │      Recharts        │
                └──────────────────────┘
```

---

## 6. Project Structure

```text
smart-meter-project/
│
├── analytics/
│   ├── analytics.py
│   ├── forecast.py
│   └── virtual_meter.py
│
├── backend/
│   └── main.py
│
├── dashboard/
│   ├── src/
│   ├── public/
│   ├── package.json
│   ├── package-lock.json
│   └── vite.config.js
│
├── data/
│   └── meter_configs.txt
│
├── firmware/
│   └── pulse_counter.cpp
│
├── tests/
│   ├── pulse_accuracy_test.cpp
│   ├── pulse_debounce_test.cpp
│   ├── m001_pulse_count.txt
│   ├── m002_pulse_count.txt
│   ├── pulse_data.txt
│   ├── pulse_data_m002.txt
│   ├── meter_data_M001.json
│   └── meter_data_M002.json
│
├── docs/
│
└── README.md
```

---

## 7. Meter Configuration

Meter configuration is stored in:

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

For the current configuration:

- M001: 1600 pulses/kWh, ₹15/kWh, 10 seconds
- M002: 1600 pulses/kWh, ₹15/kWh, 10 seconds

---

## 8. Pulse-to-Energy Calculation

Energy consumption is calculated using:

```text
Energy (kWh) = Pulse Count / Impulse Constant
```

For example:

```text
Pulse Count = 1600
Impulse Constant = 1600 pulses/kWh

Energy = 1600 / 1600
       = 1 kWh
```

---

## 9. Power Calculation

Power is calculated using the number of new pulses observed during the measurement interval.

```text
Interval Energy = New Pulses / Impulse Constant

Power (W) =
(Interval Energy / Time in Hours) × 1000
```

The C++ processor also prevents negative interval pulse counts when a virtual meter counter decreases.

---

## 10. API Endpoints

The backend exposes the following major endpoints:

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

### Meter Data

```text
POST /meter-data
```

Receives meter readings and stores them in PostgreSQL.

Expected fields:

```json
{
  "meterId": "M001",
  "timestamp": "2026-09-29 16:00:00",
  "pulseCount": 100,
  "impulseConstant": 1600,
  "energyKWh": 0.0625,
  "powerW": 1000
}
```

---

## 11. Authentication

Meter-data ingestion uses an API-key header:

```text
X-API-Key
```

The backend validates the supplied key and returns HTTP `401` for an invalid key.

For security, do not commit production secrets directly into a public repository.

---

## 12. Analytics

The analytics module provides:

- Latest power
- Average power
- Minimum power
- Maximum power
- Total energy
- Estimated electricity cost
- Anomaly status
- Anomaly type
- Anomaly reason

### Anomaly Conditions

#### High Power Spike

The latest power value is compared with the recent average and standard deviation.

#### Sudden Power Increase

```text
latest_power / previous_power >= 1.75
```

#### Sudden Power Drop

```text
latest_power / previous_power <= 0.40
```

---

## 13. Forecasting

The forecasting module uses a **5-reading moving average**.

The system provides:

```text
forecastPower
nextHourKWh
nextDayKWh
```

Example:

```text
Input:
1000, 1100, 1200, 1300, 1400 W

Forecast:
1200 W

Next hour:
1.2 kWh

Next day:
28.8 kWh
```

The forecasting module also handles smaller datasets and empty input safely.

---

## 14. Store-and-Forward

When the backend is unavailable, meter readings can be stored locally in:

```text
data/pending_readings.txt
```

The system validates the buffered JSON before storing it.

Incomplete JSON is rejected rather than added to the queue.

When backend connectivity returns, pending readings can be forwarded to the REST API.

---

## 15. Database

The primary PostgreSQL table is:

```text
meter_data
```

Main fields:

```text
id
meter_id
timestamp
pulse_count
impulse_constant
energy_kwh
power_w
```

The database currently contains data for:

```text
M001
M002
```

and uses constraints to prevent `NULL` values in required fields.

---

## 16. Running the Backend

From the project root:

```cmd
python -m uvicorn backend.main:app --reload
```

Backend URL:

```text
http://127.0.0.1:8000
```

Swagger API documentation:

```text
http://127.0.0.1:8000/docs
```

---

## 17. Running the Dashboard

Go to the dashboard directory:

```cmd
cd dashboard
```

Install dependencies:

```cmd
npm install
```

Start the development server:

```cmd
npm run dev
```

Open:

```text
http://localhost:5173/
```

---

## 18. Building the Dashboard

Run:

```cmd
cd dashboard
npm run build
```

The production output is generated in:

```text
dashboard/dist/
```

---

## 19. Testing Performed

### Pulse Counting

- Virtual meter pulse counting verified.
- Persistent counter behavior verified.
- M001 counter continued after restart.
- M002 counter persistence was also verified.

### Pulse Accuracy

Dedicated C++ accuracy test:

```text
Expected pulses: 1000
Counted pulses:  1000
Error:           0%
Result:          PASS
```

### Debounce

Dedicated debounce test:

```text
Debounce interval: 5 ms
Valid pulses accepted correctly
Noise pulses rejected
Result: PASS
```

### Energy Calculation

Energy calculation was verified using:

```text
Energy = Pulse Count / Impulse Constant
```

Saved M001 and M002 JSON readings matched this calculation within the stored rounding precision.

### Forecasting

Forecasting was tested with:

- Five-reading input
- Fewer-than-five readings
- Empty input

All three cases returned valid results.

### Database Integrity

The database was checked for:

- Orphan meter records
- Negative readings
- Duplicate measurements
- Future timestamps
- Unknown meter IDs
- Invalid pulse ordering
- Energy calculation consistency

Duplicate exact measurement rows were removed during database cleanup.

### Dashboard Build

The React/Vite production build completed successfully:

```text
590 modules transformed
Build completed successfully
```

---

## 20. Current Meter IDs

```text
M001
M002
```

Both meters use independent local pulse counters and configuration.

---

## 21. Known Limitations

### PostgreSQL Driver / Windows Application Control

On the current Windows environment, the PostgreSQL `libpq` DLL used by Psycopg is blocked by an Application Control policy.

As a result, the FastAPI + PostgreSQL backend cannot currently start on that machine until the organization-approved environment permits the required PostgreSQL library.

No Windows security policy has been disabled or bypassed to work around this restriction.

### Counter Reset

The processor protects interval pulse calculations when the counter decreases.

However, cumulative energy is calculated directly from the current cumulative pulse count. Therefore, a true external counter reset can cause cumulative energy to decrease.

This can be improved in a future version using explicit cumulative-energy accumulation independent of the resettable pulse counter.

### No-Pulse and Tamper Detection

Explicit `NO_PULSE` and tamper-detection conditions are not currently implemented in the backend.

---

## 22. Future Enhancements

- Explicit no-pulse detection
- Dedicated tamper-detection logic
- Reset-aware cumulative energy tracking
- Improved authentication and secret management
- Alert notifications
- Role-based access
- Additional forecasting models
- Export reports
- Cloud deployment
- Automated test suite
- Real hardware sensor integration

---

## 23. Project Status

Core software functionality implemented:

- Virtual meter simulation
- Pulse processing
- Energy calculation
- Power calculation
- Multi-meter support
- REST API
- PostgreSQL persistence
- Analytics
- Anomaly detection
- Forecasting
- React dashboard
- Store-and-forward buffering
- Automated C++ test programs

The current project is designed as a **software-only smart-meter solution using virtual sensors**, allowing the complete architecture to be developed and demonstrated without physical smart-meter hardware.

---

## 24. Author

**Abhishek Kumar**

B.Tech – Computer Science and Engineering
Siksha 'O' Anusandhan University, Bhubaneswar
