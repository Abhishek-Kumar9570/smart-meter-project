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
