# Smart Energy Smart-Meter Pulse Counter & Analytics Agent
# System Design and UML Diagrams

## 1. System Architecture

The final implementation is a Linux-based, C/C++ smart-meter monitoring system using a virtual/simulated meter, Linux character-device driver, C++ monitoring agent, local persistence, HTTP communication, analytics, forecasting, and a terminal dashboard.

```mermaid
flowchart LR
    A[M001 Virtual Meter<br/>C++ Simulator]
    B[M002 Simulated-Real Meter<br/>C++ Simulator]

    D[Linux Character Device Driver<br/>virtual_meter_driver.c]
    DEV[/dev/virtual_meter/]

    C[C++ Smart Meter Agent<br/>smart_meter_agent.cpp]

    E[Local CSV Persistence<br/>meter_readings.csv]
    F[C++ Cloud Receiver<br/>cloud_receiver.cpp]
    G[HTTP JSON Communication]

    H[Forecast Engine<br/>forecast.cpp]
    I[Terminal Dashboard<br/>smart_meter_dashboard.cpp]

    A --> D
    B --> D
    D --> DEV
    DEV --> C
    C --> E
    C --> G
    G --> F
    E --> H
    E --> I
