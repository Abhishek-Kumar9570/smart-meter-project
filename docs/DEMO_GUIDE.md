# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

## Final 5–10 Minute Evaluation Demo Guide

## 1. Objective

Demonstrate the complete Linux/C++ smart-meter pipeline:

Virtual/Simulated Meter
→ Linux Character Device Driver
→ `/dev/virtual_meter`
→ C++ Smart Meter Agent
→ Energy / Power / Cost
→ CSV Persistence
→ HTTP Cloud Receiver
→ Anomaly Detection
→ Forecasting
→ Terminal Dashboard

## 2. Before the Demo

Run everything from the project root:

`smart-meter-project/`

Important components:

- `driver/virtual_meter_driver.c`
- `app/smart_meter_agent.cpp`
- `app/virtual_meter_simulator.cpp`
- `app/simulated_real_meter.cpp`
- `app/smart_meter_dashboard.cpp`
- `analytics/forecast.cpp`
- `cloud/cloud_receiver.cpp`
- `tests/`
- `data/`
- `docs/`

## 3. Recommended Demo Sequence

### 0:00–1:00 — Introduction

Say:

> "This is a Linux-based Smart Energy Smart-Meter Pulse Counter and Analytics Agent. It uses a virtual or simulated meter, a Linux character-device driver, and C++ applications to process pulses, calculate energy and power, store readings, communicate with a cloud receiver, detect anomalies, and generate forecasts."

### 1:00–2:00 — Linux Driver

Show:

`driver/virtual_meter_driver.c`

Explain:

- Linux kernel module
- Character device
- `/dev/virtual_meter`
- Read/write operations
- Kernel/user-space communication
- Synchronization

Demonstrate:

```bash
ls -l /dev/virtual_meter

### 2:00–3:00 — Driver Read/Write and Tests

Demonstrate the device:

    echo 4093 | sudo tee /dev/virtual_meter
    cat /dev/virtual_meter

Expected value:

    4093

Show the pulse accuracy and debounce tests.

Expected accuracy result:

    Expected pulses: 1000
    Counted pulses: 1000
    Error: 0%
    PASS

Expected debounce result:

    Valid pulses: 1000
    Accepted pulses: 1000
    Debounce window: 5 ms
    Noise handling: PASS

### 3:00–4:30 — Virtual Meter and C++ Agent

Run the virtual meter:

    ./app/virtual_meter_simulator

Then run:

    ./app/smart_meter_agent M001

Show:
- Meter ID
- Pulse count
- Energy
- Power
- Cumulative energy
- Estimated cost
- Anomaly status

Explain:

    Energy (kWh) = New Pulses / Impulse Constant

### 4:30–5:30 — Persistence and Cloud Communication

Show:

    data/meter_readings.csv

Stored fields:

    timestamp,meter_id,pulse_count,energy_kWh,cumulativeEnergy_kWh,power_W,cost_Rs

Start the cloud receiver:

    ./cloud/cloud_receiver

Show the agent result:

    Cloud Sync : SENT (HTTP 200)

Explain that the C++ agent sends a JSON meter reading over HTTP and the C++ receiver confirms successful reception.

### 5:30–6:30 — M002 and Anomaly Detection

Run:

    ./app/simulated_real_meter

Then:

    ./app/smart_meter_agent M002

M002 generates variable-load pulse activity.

Implemented anomaly categories include:

- NORMAL
- NO-PULSE CONDITION
- POWER SPIKE DETECTED
- TAMPER/COUNTER RESET DETECTED
- SUDDEN PULSE INCREASE
- SUDDEN POWER DROP DETECTED

### 6:30–7:30 — Forecasting

Run:

    ./analytics/forecast M001

or:

    ./analytics/forecast M002

Explain:

> The forecast module uses historical meter readings to generate next-hour and next-day consumption estimates.

### 7:30–8:30 — Terminal Dashboard

Run:

    ./app/smart_meter_dashboard M001

or:

    ./app/smart_meter_dashboard M002

Show:
- Meter ID
- Timestamp
- Pulse count
- Interval energy
- Cumulative energy
- Power
- Estimated cost
- Anomaly information
- Historical information
- Linux device
- Platform
- C++ status

### 8:30–9:30 — Architecture Explanation

Show:

    docs/DESIGN_DIAGRAMS.md

Explain:

    Virtual Meter
        ->
    Linux Character Driver
        ->
    /dev/virtual_meter
        ->
    C++ Smart Meter Agent
        ->
    CSV + HTTP
        ->
    Cloud Receiver / Analytics / Dashboard

Mention the Linux system-programming concepts:

- Linux kernel module
- Character device
- `/dev/virtual_meter`
- User space / kernel space interaction
- `read()` and `write()`
- Mutex synchronization
- File I/O
- Timing
- Error handling
- POSIX socket communication

### 9:30–10:00 — Conclusion

Suggested closing statement:

> This project demonstrates an end-to-end Linux and C/C++ smart-meter system. It starts with a virtual or simulated meter, passes through a Linux character-device driver, processes readings using C++, calculates energy and power, persists time-series data, communicates over HTTP, detects abnormal conditions, generates forecasts, and provides terminal monitoring.

## 4. Important Technical Questions

### Why did you use a virtual meter?

> The capstone guide allows a virtual or simulated sensor when physical smart-meter hardware is unavailable.

### Why Linux?

> Linux is used to demonstrate the required system-programming concepts and a real character-device interface.

### Why a character device?

> It provides a clear interface between the Linux kernel and the user-space C++ application.

### How is energy calculated?

> New pulse count is divided by the configured impulse constant in pulses per kWh.

### How is power calculated?

> Power is calculated from interval energy and the elapsed measurement time.

### What happens when the counter decreases?

> The decrease is treated as a counter-reset or tamper-related condition instead of normal positive consumption.

### How does cloud communication work?

> The C++ agent creates a JSON reading and sends it over HTTP to the C++ cloud receiver, which returns HTTP 200.

### How is forecasting done?

> Historical meter readings are used to generate next-hour and next-day estimates.

## 5. Requirement Coverage

| Requirement | Demonstration |
|---|---|
| FR1 | Pulse accuracy test |
| FR2 | Configurable pulse-to-kWh conversion |
| FR3 | HTTP transmission |
| FR4 | Timestamped CSV persistence |
| FR5 | Anomaly detection |
| FR6 | Forecasting |
| FR7 | Terminal dashboard |
| FR8 | M001 virtual + M002 simulated-real meter |

## 6. Final Documentation Set

    docs/
    ├── SRS.md
    ├── DESIGN_DIAGRAMS.md
    ├── PROJECT_DOCUMENTATION.md
    ├── TEST_RESULTS.md
    ├── DEMO_GUIDE.md
    └── FINAL_REPORT.md

The root `README.md` provides the overall project setup, build, execution, configuration, and repository information.

## 7. Final Demo Tip

Keep the following ready before evaluation:

- Linux driver already loaded.
- `/dev/virtual_meter` available.
- Required C++ binaries compiled.
- Sample CSV readings present.
- Cloud receiver ready.
- M001 and M002 configurations available.

Demonstrate the most important working path first:

    Driver
      ->
    Meter
      ->
    C++ Agent
      ->
    Energy/Power
      ->
    CSV
      ->
    HTTP
      ->
    Anomaly/Forecast
      ->
    Dashboard
