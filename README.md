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
- Converts pulses to kWh using a configurable impulse constant.
- Calculates instantaneous power in watts.
- Maintains cumulative energy across application restarts.
- Estimates electricity cost.
- Uses configurable meter parameters and sampling interval.
- Saves timestamped readings to CSV time-series storage.
- Sends meter readings as JSON over HTTP using Linux C++/libcurl.
- Reports cloud synchronization status.

### Edge-to-Cloud Communication

- C++ cloud receiver implemented using Linux POSIX sockets.
- HTTP endpoint is configurable through `data/meter_configs.txt`.
- Meter agent sends:
  - Meter ID
  - Pulse count
  - Interval energy
  - Cumulative energy
  - Instantaneous power
  - Estimated cost
- Successful transmission is confirmed through an HTTP 200 response.

### Anomaly Detection

The current rule-based implementation detects:

- `NORMAL`
- `NO-PULSE CONDITION`
- `POWER SPIKE DETECTED`
- `TAMPER/COUNTER RESET DETECTED`
- `SUDDEN PULSE INCREASE`
- `SUDDEN POWER DROP DETECTED`

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
- Interval energy consumption
- Cumulative energy
- Instantaneous power
- Estimated cost
- Current anomaly status
- Number of alert records
- Historical sample count
- Recent reading history
- Linux device
- Platform
- C++ implementation status

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
---

## 5. Dependencies and Build

The project runs in a Linux/WSL2 environment and uses C/C++17.

### Install Required Packages

```bash
sudo apt update
sudo apt install -y build-essential pkg-config libcurl4-openssl-dev
```

### Build Driver

```bash
cd driver
make
sudo insmod virtual_meter_driver.ko
lsmod | grep virtual_meter
cat /sys/class/misc/virtual_meter/dev
sudo rm -f /dev/virtual_meter
sudo mknod /dev/virtual_meter c 10 261
sudo chmod 666 /dev/virtual_meter
ls -l /dev/virtual_meter
```

### Build User-Space Applications

```bash
cd ..
g++ -std=c++17 app/virtual_meter_simulator.cpp -o app/virtual_meter_simulator
g++ -std=c++17 app/simulated_real_meter.cpp -o app/simulated_real_meter
g++ -std=c++17 app/smart_meter_agent.cpp -o app/smart_meter_agent $(pkg-config --cflags --libs libcurl)
g++ -std=c++17 app/smart_meter_dashboard.cpp -o app/smart_meter_dashboard
g++ -std=c++17 analytics/forecast.cpp -o analytics/forecast
g++ -std=c++17 cloud/cloud_receiver.cpp -o cloud/cloud_receiver
```

## 6. Execution

Start the cloud receiver:
```bash
./cloud/cloud_receiver
```

Run a meter simulator in another terminal:
```bash
./app/virtual_meter_simulator
```

Run the simulated-real M002 meter:
```bash
./app/simulated_real_meter
```

Run the smart-meter agent:
```bash
./app/smart_meter_agent M001
./app/smart_meter_agent M002
```

View the dashboard:
```bash
./app/smart_meter_dashboard M001
./app/smart_meter_dashboard M002
```

Run forecasting:
```bash
./analytics/forecast M001
./analytics/forecast M002
```

## 7. Configuration

Configuration file: data/meter_configs.txt

Format:
```text
meter_id,impulse_constant,cost_per_kWh,sample_interval_seconds,cloud_endpoint
```

Example:
```text
M001,1600,15.0,10,http://127.0.0.1:8080/
M002,1600,15.0,10,http://127.0.0.1:8080/
```

## 8. Data Storage

Time-series data is stored in data/meter_readings.csv.
Columns:
```text
timestamp,meter_id,pulse_count,energy_kWh,cumulativeEnergy_kWh,power_W,cost_Rs
```

Cumulative energy is restored from the stored value when the agent restarts.

## 9. Cloud Communication

The C++ smart-meter agent sends JSON readings over HTTP using libcurl to the configured endpoint.
The C++ cloud receiver uses Linux POSIX sockets and returns an HTTP 200 response after receiving the reading.

## 10. Project Constraints

- Programming languages: C and C++17
- Operating system: Linux / WSL2
- Linux character-device driver and kernel synchronization concepts are used.
- Cloud communication uses C++ and libcurl.
- The project does not require Python, FastAPI, React, or PostgreSQL.
