# Smart Energy Smart-Meter Pulse Counter & Analytics Agent

## Design Diagrams

---

## 1. High-Level Architecture

The capstone architecture is organized into sensing, edge/device, communication, backend/cloud, and presentation layers.

```mermaid
flowchart LR
    subgraph Sensing["Sensing Layer"]
        A1[Real Pulse Sensor<br/>IR/Optocoupler on Meter LED]
        A2[Virtual Sensor<br/>Simulated Pulse Generator]
    end

    subgraph Edge["Edge / Device Layer"]
        B1[Pulse Counter Firmware<br/>ESP32/Arduino/RPi]
        B2[Debounce + Calibration<br/>pulses → kWh/kW]
        B3[Local Buffer<br/>store-and-forward on disconnect]
    end

    subgraph Comm["Communication Layer"]
        C1[MQTT Broker]
        C2[REST API Gateway]
    end

    subgraph Cloud["Backend / Cloud Layer"]
        D1[Ingestion Service]
        D2[(Time-Series DB)]
        D3[Analytical Agent<br/>Anomaly Detection + Forecasting]
        D4[REST/GraphQL API]
    end

    subgraph Presentation["Presentation Layer"]
        E1[Web Dashboard]
        E2[Alerts: Email/SMS/Push]
    end

    A1 --> B1
    A2 --> B1
    B1 --> B2 --> B3
    B3 --> C1
    B3 --> C2
    C1 --> D1
    C2 --> D1
    D1 --> D2
    D2 --> D3
    D3 --> D4
    D2 --> D4
    D4 --> E1
    D3 --> E2
```

### Implementation Note

For this software-only implementation, the **Virtual Sensor** path is used instead of physical smart-meter hardware. The communication path implemented in the project uses the **REST API** route rather than MQTT.

---

## 2. Use Case Diagram

```mermaid
flowchart TB
    User((Consumer/Utility Admin))
    Device((Smart Meter Device))
    UC1([Monitor Live Power/Energy])
    UC2([View Historical Consumption])
    UC3([Receive Anomaly Alerts])
    UC4([Configure Meter Constant])
    UC5([Export/View Bill Estimate])
    UC6([Register New Meter])

    User --> UC1
    User --> UC2
    User --> UC3
    User --> UC4
    User --> UC5
    User --> UC6
    Device --> UC1
```

### Main Use Cases

- Monitor live power and energy
- View historical consumption
- Receive anomaly alerts
- Configure meter constant
- View bill estimate
- Register a new meter

---

## 3. Class Diagram

```mermaid
classDiagram
    class Meter {
        +string meterId
        +float impulseConstant
        +string location
        +registerMeter()
    }

    class PulseReading {
        +string meterId
        +datetime timestamp
        +int pulseCount
        +float energyKWh
        +float powerW
    }

    class AnalyticalAgent {
        +detectAnomaly(readings)
        +forecastConsumption(history)
        +generateAlert()
    }

    class Alert {
        +string type
        +string severity
        +datetime timestamp
        +string message
    }

    class Dashboard {
        +renderLiveData()
        +renderHistory()
        +renderAlerts()
    }

    Meter "1" --> "many" PulseReading : produces
    PulseReading "many" --> "1" AnalyticalAgent : analyzed by
    AnalyticalAgent --> Alert : generates
    Dashboard --> PulseReading : displays
    Dashboard --> Alert : displays
```

---

## 4. Sequence Diagram

The sequence below represents the end-to-end flow defined in the capstone architecture.

```mermaid
sequenceDiagram
    participant S as Sensor
    participant F as Firmware
    participant B as MQTT Broker
    participant I as Ingestion Service
    participant DB as Time-Series DB
    participant AA as Analytical Agent
    participant UI as Dashboard

    S->>F: Pulse (interrupt)
    F->>F: Debounce + Count
    F->>F: Convert to kWh/kW
    F->>B: Publish reading (JSON)
    B->>I: Deliver message
    I->>DB: Store reading
    I->>AA: Trigger analysis (async)
    AA->>DB: Fetch recent history
    AA->>AA: Run anomaly/forecast model
    AA-->>I: Alert (if anomaly)
    UI->>I: GET /readings, /alerts
    I->>DB: Query
    DB-->>I: Data
    I-->>UI: JSON response
```

### Implementation Note

The actual project currently uses:

```text
Virtual Meter
      ↓
C++ Pulse Processor
      ↓
REST API
      ↓
PostgreSQL
      ↓
Analytics / Forecasting
      ↓
React Dashboard
```

Therefore, the `MQTT Broker` and generic `Ingestion Service` shown above are part of the **guide architecture**, not components currently used in this implementation.

---

## 5. Activity Diagram

```mermaid
flowchart TD
    Start([Pulse Detected]) --> Debounce{Valid pulse?<br/>debounce check}
    Debounce -- No --> Discard[Discard as noise]
    Debounce -- Yes --> Count[Increment counter]
    Count --> Convert[Convert to kWh/kW]
    Convert --> Store[Buffer locally]
    Store --> NetCheck{Network available?}
    NetCheck -- No --> Queue[Queue for retry]
    NetCheck -- Yes --> Send[Send to broker/API]
    Send --> Persist[Persist in DB]
    Persist --> Analyze[Run analytical agent]
    Analyze --> AnomalyCheck{Anomaly detected?}
    AnomalyCheck -- Yes --> Alert[Generate + send alert]
    AnomalyCheck -- No --> UpdateUI[Update dashboard]
    Alert --> UpdateUI
    UpdateUI --> End([End])
```

---

## 6. Project-Specific Architecture

The current software implementation can be represented as:

```mermaid
flowchart LR
    A[Python Virtual Meter] --> B[C++ Pulse Processor]
    B --> C[Meter JSON]
    C --> D[FastAPI REST API]
    D --> E[(PostgreSQL)]
    E --> F[Analytics]
    E --> G[Forecasting]
    F --> H[React Dashboard]
    G --> H
    E --> H
```

---

## 7. Project-Specific Data Flow

```mermaid
flowchart TD
    A[Generate Virtual Pulses]
    B[Read Pulse Count]
    C[Calculate New Pulses]
    D[Calculate Energy]
    E[Calculate Power]
    F[Generate JSON Reading]
    G{Backend Available?}
    H[POST /meter-data]
    I[Store in pending_readings.txt]
    J[PostgreSQL]
    K[Analytics]
    L[Forecasting]
    M[React Dashboard]

    A --> B
    B --> C
    C --> D
    D --> E
    E --> F
    F --> G
    G -- Yes --> H
    G -- No --> I
    H --> J
    I --> H
    J --> K
    J --> L
    K --> M
    L --> M
```

---

## 8. Component Mapping

| System Component        | Project Implementation                       |
| ----------------------- | -------------------------------------------- |
| Virtual Sensor          | `analytics/virtual_meter.py`                 |
| Pulse Processing        | `firmware/pulse_counter.cpp`                 |
| Local Configuration     | `data/meter_configs.txt`                     |
| Local Store-and-Forward | `data/pending_readings.txt`                  |
| REST Backend            | `backend/main.py`                            |
| Database                | PostgreSQL                                   |
| Analytics               | `backend/main.py` / `analytics/analytics.py` |
| Forecasting             | `analytics/forecast.py`                      |
| Dashboard               | React.js + Recharts                          |

---

## 9. Supported Meters

```text
M001
M002
```

Each meter has its own pulse counter and meter configuration.

---

## 10. Design Summary

The system follows a layered smart-meter architecture in which pulse data is generated, processed, transmitted, stored, analyzed, forecasted, and visualized.

The implementation uses a **software-based virtual sensor**, allowing the complete system to be demonstrated without physical smart-meter hardware.
