# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

## Final Project Report

**Domain:** IoT, Embedded & Virtual Sensors  
**Implementation:** Linux + C/C++  
**Project Type:** Software-based smart-meter monitoring and analytics system

---

## 1. Abstract

The Smart Energy Smart-Meter Pulse Counter & Analytics Agent is a software-based smart-meter monitoring system designed to simulate meter pulse generation, process meter data through a Linux character-device driver, calculate energy and power consumption, maintain cumulative energy, detect selected abnormal conditions, generate short-term forecasts, persist time-series readings, and communicate readings to a cloud receiver.

The project uses a virtual meter and a simulated-real meter because physical smart-meter hardware is not required for the software demonstration. A C Linux kernel module provides the `/dev/virtual_meter` character-device interface. C++ applications provide meter simulation, pulse processing, energy and power calculation, anomaly detection, forecasting, dashboard display, and HTTP communication.

The system demonstrates an end-to-end flow from simulated meter pulses to processed readings, local persistence, cloud communication, analytics, and monitoring.

---

## 2. Problem Statement

Smart-meter pulse outputs are low-level events. A pulse count by itself does not directly provide useful information such as energy consumption, instantaneous power, cumulative energy, estimated cost, historical usage, anomalies, or forecasts.

The project addresses this problem by creating a software-based system that can:

- Generate or simulate meter pulses.
- Count and process pulses.
- Convert pulses into energy.
- Calculate interval power.
- Maintain cumulative energy.
- Persist timestamped readings.
- Communicate readings through HTTP.
- Detect selected abnormal conditions.
- Generate short-term consumption forecasts.
- Display monitoring information through a dashboard.

---

## 3. Objectives

The main objectives are:

1. Implement a virtual/simulated smart-meter pulse source.
2. Develop a Linux character-device driver for the meter counter.
3. Provide a `/dev/virtual_meter` device interface.
4. Process meter readings using C/C++.
5. Implement pulse accuracy and debounce validation.
6. Convert pulse increments into kWh using a configurable impulse constant.
7. Calculate interval power in watts.
8. Calculate estimated consumption cost.
9. Maintain cumulative energy across readings and application restarts.
10. Store timestamped meter readings locally.
11. Transmit processed readings using HTTP and JSON.
12. Implement a C++ cloud receiver for edge-to-cloud demonstration.
13. Detect selected anomalous meter conditions.
14. Generate next-hour and next-day forecasts.
15. Support M001 virtual and M002 simulated-real meter demonstrations.
16. Provide a terminal monitoring dashboard.
17. Maintain complete Git/GitHub project documentation.

---

## 4. Scope

### 4.1 In Scope

- Virtual meter simulation.
- Simulated-real meter operation.
- Linux character-device driver.
- `/dev/virtual_meter`.
- C/C++ pulse processing.
- Configurable meter parameters.
- Pulse-to-energy conversion.
- Power calculation.
- Cumulative energy persistence.
- Cost estimation.
- CSV time-series persistence.
- HTTP JSON communication.
- C++ cloud receiver.
- Rule-based anomaly detection.
- Historical forecasting.
- Terminal dashboard.
- Automated and manual testing.
- Git/GitHub version control.
- Project documentation.

### 4.2 Out of Scope

- Certified commercial smart-meter hardware.
- Utility-grade billing infrastructure.
- Grid-scale SCADA integration.
- Hardware certification.
- Production cloud deployment.
- Enterprise-scale multi-tenant infrastructure.
- Physical sensor installation.

---

## 5. System Architecture

The final implementation consists of the following major layers:

### 5.1 Meter Layer

Two meter sources are available:

- **M001:** Virtual meter.
- **M002:** Simulated-real meter with variable load behavior.

### 5.2 Linux Device Layer

The Linux kernel module implements a character device:

`/dev/virtual_meter`

The driver provides controlled read/write access to the simulated meter counter.

### 5.3 C++ Application Layer

The C++ Smart Meter Agent:

- Reads the meter counter.
- Calculates new pulses.
- Converts pulses to energy.
- Calculates power.
- Calculates cumulative energy.
- Calculates estimated cost.
- Detects configured anomaly conditions.
- Stores readings.
- Generates JSON.
- Sends meter data through HTTP.

### 5.4 Persistence Layer

Processed readings are stored in:

`data/meter_readings.csv`

Fields:

`timestamp,meter_id,pulse_count,energy_kWh,cumulativeEnergy_kWh,power_W,cost_Rs`

### 5.5 Communication Layer

The Smart Meter Agent sends JSON readings through HTTP to the C++ cloud receiver.

The cloud receiver returns HTTP 200 after successfully receiving a reading.

### 5.6 Analytics Layer

Historical meter readings are used by the C++ forecasting module to generate next-hour and next-day estimates.

### 5.7 Presentation Layer

The terminal dashboard displays current and historical monitoring information.

---

## 6. End-to-End Data Flow

```text
M001 Virtual Meter
        |
        v
M002 Simulated-Real Meter
        |
        v
Linux Character Device Driver
        |
        v
/dev/virtual_meter
        |
        v
C++ Smart Meter Agent
        |
        +----> Energy Calculation
        |
        +----> Power Calculation
        |
        +----> Cumulative Energy
        |
        +----> Cost Calculation
        |
        +----> Anomaly Detection
        |
        +----> CSV Persistence
        |
        +----> JSON / HTTP
                    |
                    v
             C++ Cloud Receiver

Historical CSV
        |
        +----> Forecast Engine
        |
        +----> Terminal Dashboard
