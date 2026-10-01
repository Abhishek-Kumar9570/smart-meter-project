# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

A Linux-based smart-meter monitoring and analytics system implemented using **C/C++** and Linux system-programming concepts.

The project uses a Linux character-device driver to represent a smart-meter pulse source. C++ applications generate virtual and simulated-real meter pulses, read the pulse counter through `/dev/virtual_meter`, convert pulses into energy and power, persist readings as timestamped CSV time-series data, detect abnormal conditions, forecast future energy consumption, and display the latest reading through a terminal dashboard.

---

## 1. Project Objectives

- Generate smart-meter pulses in software.
- Count and process pulses through a Linux device driver.
- Convert configurable pulse counts into kWh.
- Calculate electrical power from pulse intervals.
- Support configurable meter parameters.
- Persist timestamped time-series readings.
- Detect no-pulse, power-spike, and counter-reset/tamper conditions.
- Forecast next-hour and next-day energy consumption.
- Demonstrate both a virtual meter and a simulated-real meter.
- Apply Linux system-programming and device-driver concepts.

---

## 2. Key Features

### Linux Virtual Meter Driver

- Linux character-device driver implemented in C.
- Device exposed as `/dev/virtual_meter`.
- Read/write support for pulse count.
- Kernel mutex used for safe access to the pulse counter.
- User-space C++ applications communicate with the driver through Linux file descriptors.

### Virtual Meter Simulator

- C++ Linux application.
- Generates one pulse per second.
- Updates the Linux device-driver pulse counter continuously.

### Simulated-Real Meter

- C++ Linux application representing meter M002.
- Produces variable pulse rates to simulate changing real-world loads.
- Uses low, medium, normal, and high load patterns.

### Smart Meter Agent

- C++17 Linux application.
- Reads pulse counts from `/dev/virtual_meter`.
- Calculates interval pulse count.
- Converts pulses to kWh.
- Calculates power in watts.
- Estimates electricity cost.
- Uses configurable meter parameters.
- Saves timestamped readings to CSV.
- Reports anomaly conditions.

### Anomaly Detection

The current rule-based implementation detects:

- `NORMAL`
- `NO-PULSE CONDITION`
- `POWER SPIKE DETECTED`
- `TAMPER/COUNTER RESET DETECTED`
- Sudden pulse increase

### Forecasting

The C++ forecasting module calculates:

- Next 1-hour energy forecast
- Next 24-hour energy forecast

Forecasting is based on the average historical energy per sampling interval.

### Terminal Dashboard

The C++ dashboard displays:

- Meter ID
- Last timestamp
- Pulse count
- Energy consumption
- Power consumption
- Estimated cost
- Linux device
- Platform
- Implementation status

---

## 3. Technology Stack

### Programming Languages

- C
- C++17

### Operating System

- Linux
- WSL2 Linux environment

### System Programming

- Linux character-device driver
- `open()`
- `read()`
- `write()`
- `close()`
- Linux file descriptors
- Kernel module APIs
- Mutex synchronization
- `dmesg`
- `insmod`
- `lsmod`

### Build Tools

- GNU `gcc`
- GNU `g++`
- Linux `make`

### Version Control

- Git
- GitHub

---

## 4. System Architecture

```text
                    ┌───────────────────────────┐
                    │     Virtual Meter         │
                    │   C++ Linux Simulator     │
                    └─────────────┬─────────────┘
                                  │
                                  │ write()
                                  ▼
                    ┌───────────────────────────┐
                    │ Linux Character Driver    │
                    │ /dev/virtual_meter        │
                    │        C                  │
                    │                           │
                    │ Pulse Counter + Mutex     │
                    └─────────────┬─────────────┘
                                  │
                                  │ read()
                                  ▼
                    ┌───────────────────────────┐
                    │    Smart Meter Agent      │
                    │        C++17               │
                    └─────────────┬─────────────┘
                                  │
                 ┌────────────────┼─────────────────┐
                 │                │                 │
                 ▼                ▼                 ▼
        ┌────────────────┐ ┌───────────────┐ ┌─────────────────┐
        │ Energy / Power │ │   Anomaly     │ │ CSV Persistence │
        │ Calculation    │ │   Detection   │ │ Time-Series     │
        └────────────────┘ └───────────────┘ └─────────────────┘
                                  │
                                  ▼
                    ┌───────────────────────────┐
                    │   Forecasting Module      │
                    │        C++17               │
                    └─────────────┬─────────────┘
                                  │
                                  ▼
                    ┌───────────────────────────┐
                    │    Terminal Dashboard     │
                    │        C++17               │
                    └───────────────────────────┘
