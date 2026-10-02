# Software Requirements Specification (SRS)

## Smart Energy Smart-Meter Pulse Counter & Analytics Agent

### 1. Introduction

#### 1.1 Purpose
This Software Requirements Specification defines the requirements, scope, interfaces, functional behavior, and non-functional requirements of the Smart Energy Smart-Meter Pulse Counter & Analytics Agent.

The system is a software-based smart-meter monitoring solution that simulates meter pulses, processes them through a Linux character device driver and C/C++ application layer, converts pulses into energy and power measurements, stores time-series readings, detects selected abnormal conditions, generates short-term forecasts, and presents monitoring information through a terminal dashboard.

#### 1.2 Project Objective
The system is designed to:

1. Count smart-meter pulses using a virtual or simulated meter.
2. Process pulse data using Linux and C/C++ components.
3. Convert pulse counts into energy in kWh using a configurable meter constant.
4. Calculate instantaneous power in watts.
5. Maintain cumulative energy across readings and application restarts.
6. Transmit meter readings using HTTP.
7. Store timestamped meter readings locally.
8. Detect selected abnormal consumption patterns.
9. Generate next-hour and next-day consumption forecasts.
10. Support virtual and simulated-real meter demonstrations.
11. Provide a terminal-based monitoring dashboard.
12. Maintain a modular, documented, version-controlled project.

#### 1.3 Scope

**In Scope**
- Virtual/simulated smart-meter pulse generation.
- Linux character-device driver interaction.
- Pulse counting and debounce testing.
- Configurable pulse-to-energy conversion.
- Instantaneous power calculation.
- Cumulative energy tracking.
- HTTP-based edge-to-cloud communication.
- Local time-series CSV persistence.
- Rule-based anomaly detection.
- Historical-data-based forecasting.
- Terminal dashboard.
- Testing and documentation.

**Out of Scope**
- Multi-tenant commercial billing systems.
- Grid-scale SCADA integration.
- Certified utility-meter hardware.
- Hardware certification such as ISI/BIS certification.
- Production cloud deployment and enterprise-scale infrastructure.

#### 1.4 Implementation Approach
The current project uses a software-only virtual/simulated sensor approach. A Linux character device is used to represent the meter counter, while C and C++ components provide the low-level driver and application processing layers.

---

## 2. System Overview

### 2.1 System Description

The system follows this processing flow:

Virtual/Simulated Meter
→ Linux Character Device Driver
→ C++ Smart Meter Agent
→ Energy/Power Calculation
→ Local CSV Persistence
→ HTTP Cloud Receiver
→ Analytics/Forecasting
→ Terminal Dashboard

### 2.2 Major Components

| Component | Responsibility |
|---|---|
| Virtual Meter Simulator | Generates meter pulse counts |
| Simulated-Real Meter | Produces variable-load pulse patterns |
| Linux Character Driver | Provides `/dev/virtual_meter` device interface |
| C++ Smart Meter Agent | Reads pulses, calculates energy/power, detects anomalies, persists readings and sends JSON |
| Cloud Receiver | Receives meter data through HTTP |
| Forecast Module | Generates next-hour and next-day estimates |
| Terminal Dashboard | Displays latest readings and monitoring information |
| Test Programs | Validate driver, pulse accuracy and debounce behavior |
| Configuration File | Stores meter-specific parameters |

---

## 3. Functional Requirements

### FR1 — Pulse Counting
The system shall detect and count meter pulses with less than 1% error under the tested operating condition.

### FR2 — Pulse-to-Energy Conversion
The system shall convert pulse increments into energy using a configurable impulse constant expressed in pulses per kWh.

### FR3 — Periodic Data Transmission
The system shall transmit processed meter readings at a configurable measurement interval.

### FR4 — Time-Series Persistence
The system shall persist meter readings with:
- Timestamp
- Meter/device ID
- Pulse count
- Interval energy
- Cumulative energy
- Instantaneous power
- Estimated cost

### FR5 — Anomaly Detection
The analytical processing shall identify selected abnormal conditions, including:
- No-pulse condition
- Power spike
- Tamper/counter-reset condition
- Sudden pulse increase
- Sudden power drop

### FR6 — Consumption Forecasting
The system shall generate next-hour and next-day consumption estimates using historical meter readings.

### FR7 — Monitoring Dashboard
The dashboard shall display:
- Meter ID
- Timestamp
- Pulse count
- Interval energy
- Cumulative energy
- Power
- Estimated cost
- Anomaly information
- Historical reading information

### FR8 — Multiple Meter Demonstration
The system shall support at least one virtual meter and one simulated-real meter for demonstration purposes through meter-specific configuration.

---

## 4. Non-Functional Requirements

### 4.1 Reliability
The system shall include debounce logic and should avoid invalid pulse counting under the tested operating conditions.

### 4.2 Configurability
The impulse constant, measurement interval, cost parameter, meter ID, and cloud endpoint shall be configurable.

### 4.3 Maintainability
The project shall use a modular source structure separating the driver, application, simulator, analytics, cloud receiver, data, tests, and documentation.

### 4.4 Portability
The final implementation shall target a Linux environment and use C/C++ for the system and application implementation.

### 4.5 Performance
The application shall process meter readings at the configured sampling interval and calculate interval power using elapsed time between readings.

### 4.6 Security
Secrets and credentials shall not be hard-coded into source files. Configuration templates shall be separated from private values.

### 4.7 Scalability
The software architecture shall use meter IDs and configuration so that additional simulated meters can be represented.

---

## 5. External Interfaces

### 5.1 Linux Device Interface
The application communicates with:

`/dev/virtual_meter`

The device driver provides read/write access to the virtual meter pulse counter.

### 5.2 Configuration Interface
Meter-specific parameters are read from:

`data/meter_configs.txt`

Parameters include:
- Meter ID
- Impulse constant
- Cost rate
- Measurement interval
- Cloud endpoint

### 5.3 Data Storage Interface
Meter readings are persisted in:

`data/meter_readings.csv`

The stored fields are:

`timestamp,meter_id,pulse_count,energy_kWh,cumulativeEnergy_kWh,power_W,cost_Rs`

### 5.4 Cloud Communication Interface
The C++ smart-meter agent creates a JSON payload containing meter information and measurement values and sends it through HTTP to the configured cloud receiver.

---

## 6. Data Processing Requirements

### 6.1 Pulse Delta
The system shall calculate new pulses from the difference between the current and previous meter counter values.

### 6.2 Energy Calculation
Energy shall be calculated from the pulse increment and the configured impulse constant.

Conceptually:

`Energy (kWh) = New Pulses / Impulse Constant`

### 6.3 Power Calculation
Instantaneous power shall be derived from the interval energy and elapsed measurement time.

### 6.4 Cumulative Energy
The system shall maintain cumulative energy for each meter and restore the previously stored cumulative value when the application is restarted.

### 6.5 Estimated Cost
The system shall calculate an estimated cost using the configured cost parameter.

---

## 7. Anomaly Requirements

The monitoring system shall classify readings using rule-based conditions.

### 7.1 Normal Condition
The reading is within the expected operating range.

### 7.2 No-Pulse Condition
No new pulses are observed during an expected measurement interval.

### 7.3 Power Spike
The calculated power exceeds the configured or implemented spike condition.

### 7.4 Counter Reset / Tamper Condition
A decrease in the pulse counter compared with the previous reading shall be treated as a reset/tamper indication.

### 7.5 Sudden Pulse Increase
A large increase in pulse activity shall be identified as an abnormal increase.

### 7.6 Sudden Power Drop
A significant reduction in power compared with recent readings shall be identified by the implemented rule set.

---

## 8. Forecasting Requirements

The analytics component shall use historical meter readings to generate:

- Next-hour energy estimate.
- Next-day energy estimate.

The current implementation uses historical readings as the basis for the forecasting calculation.

Forecast values shall be presented as estimates and not as guaranteed future measurements.

---

## 9. Meter Configurations

### M001 — Virtual Meter
M001 represents the primary virtual smart meter used for controlled pulse-generation and pulse-processing demonstrations.

### M002 — Simulated-Real Meter
M002 represents a simulated-real meter with variable pulse generation intended to demonstrate changing load conditions and anomaly behavior.

Both meters use configurable meter parameters.

---

## 10. Error Handling and Reliability

The system shall:

1. Reject or handle invalid meter-counter transitions.
2. Handle counter-reset conditions without treating negative pulse differences as normal consumption.
3. Preserve local readings through the local persistence mechanism.
4. Report HTTP/cloud communication failures.
5. Continue local monitoring when cloud communication is unavailable.
6. Provide diagnostic output through the terminal.

---

## 11. Testing Requirements

The project shall include tests covering:

### 11.1 Driver Test
Verify Linux character-device read/write functionality.

### 11.2 Pulse Accuracy Test
Compare expected and counted pulses and calculate counting error.

### 11.3 Debounce Test
Verify that valid pulses are accepted while noisy/invalid transitions are rejected.

### 11.4 Energy Conversion Test
Verify pulse-to-kWh conversion using the configured impulse constant.

### 11.5 Power Test
Verify power calculation using measured pulse activity and elapsed interval.

### 11.6 Persistence Test
Verify cumulative energy restoration after application restart.

### 11.7 Cloud Communication Test
Verify JSON transmission and HTTP response from the C++ cloud receiver.

### 11.8 Anomaly Tests
Verify implemented anomaly conditions using controlled meter-counter changes.

### 11.9 Forecast Test
Verify generation of next-hour and next-day forecast values.

### 11.10 Dashboard Test
Verify that the latest meter information and monitoring values are displayed correctly.

---

## 12. Constraints

The current project has the following constraints:

- The implementation is software-based and does not use a physical smart meter.
- The final implementation targets Linux.
- The system uses a virtual/simulated pulse source.
- The cloud receiver is demonstrated locally rather than as a production cloud deployment.
- The dashboard is terminal-based in the final Linux/C++ implementation.
- The system is intended for academic demonstration rather than certified utility billing.

---

## 13. Assumptions

1. The configured impulse constant accurately represents the simulated meter.
2. Meter pulse counters are available through the Linux device interface.
3. The configured sampling interval is appropriate for the demonstration.
4. Historical CSV readings are available for forecasting.
5. The HTTP receiver is reachable when cloud transmission is tested.

---

## 14. Project Deliverables

The project documentation and repository shall contain:

- Source code.
- Linux device-driver implementation.
- C++ application and simulators.
- Analytics and forecasting modules.
- Cloud communication component.
- Test programs and test data.
- Configuration files.
- README and project documentation.
- Architecture and design diagrams.
- SRS document.
- Test results.
- Demonstration instructions.
- Final presentation/report materials.

---

## 15. Acceptance Criteria

The implementation shall be considered demonstrable when the evaluator can verify that:

1. The Linux virtual-meter device exists and can be accessed.
2. Meter pulses can be generated and counted.
3. Pulse-to-energy conversion works using a configurable constant.
4. Power and cumulative energy are calculated.
5. Readings are persisted with timestamps and meter IDs.
6. Meter data can be transmitted to the HTTP receiver.
7. Implemented anomaly conditions can be demonstrated.
8. Forecast values can be generated from historical data.
9. Both M001 and M002 can be demonstrated.
10. The terminal dashboard displays current monitoring information.
11. The complete source and documentation are available in GitHub.

---

## 16. Reference

Primary requirements basis:

**Smart Meter Pulse Counter & Analytical Agent — Capstone Project Guide**

The guide defines the project objective, scope, FR1–FR8 functional requirements, non-functional requirements, architecture expectations, UML/design expectations, testing expectations, and final deliverables.

