# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

## Project Documentation

---

## 1. System Architecture

The final system uses a layered Linux architecture based on C/C++.

```text
Virtual Meter Simulator
        |
        | write()
        v
Linux Character Device Driver
/dev/virtual_meter
        |
        | read()
        v
Smart Meter Agent
C++17
        |
        +-------------------+-------------------+
        |                   |                   |
        v                   v                   v
Energy / Power        Anomaly Detection    CSV Persistence
Calculation                                Time-Series
        |                   |
        +---------+---------+
                  |
                  v
          Forecasting Module
               C++17
                  |
                  v
          Terminal Dashboard
               C++17
