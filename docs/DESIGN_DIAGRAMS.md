# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

# System Design and UML Diagrams

## 1. High-Level Architecture

```text
+----------------------+       +-------------------------+
| M001 Virtual Meter   |       | M002 Simulated-Real    |
| C++ Simulator        |       | Meter - C++            |
+----------+-----------+       +-----------+-------------+
           |                               |
           +---------------+---------------+
                           |
                           v
              +---------------------------+
              | Linux Character Driver    |
              | virtual_meter_driver.c    |
              +-------------+-------------+
                            |
                            v
                   /dev/virtual_meter
                            |
                            v
              +---------------------------+
              | C++ Smart Meter Agent     |
              | smart_meter_agent.cpp     |
              +-------------+-------------+
                            |
              +-------------+-------------+
              |             |             |
              v             v             v
        Energy/Power   Anomaly        CSV Storage
        /Cost          Detection      meter_readings.csv
                            |
                            v
                     HTTP JSON
                            |
                            v
              +---------------------------+
              | C++ Cloud Receiver        |
              | cloud_receiver.cpp        |
              +---------------------------+

CSV Historical Data
        |
        +------> Forecast Engine
        |
        +------> Terminal Dashboard
