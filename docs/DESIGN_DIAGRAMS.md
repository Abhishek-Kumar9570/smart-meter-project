# Smart Energy Smart-Meter Pulse Counter & Analytics Agent
## System Design and Architecture

---

## 1. Final Architecture

```text
                  ┌──────────────────────────────┐
                  │     Virtual Meter Simulator  │
                  │            C++               │
                  │   1 pulse / second           │
                  └──────────────┬───────────────┘
                                 │
                                 │ write()
                                 ▼
                  ┌──────────────────────────────┐
                  │   Linux Character Device     │
                  │      /dev/virtual_meter      │
                  │             C                │
                  │                              │
                  │ Pulse Counter + Mutex        │
                  └──────────────┬───────────────┘
                                 │
                                 │ read()
                                 ▼
                  ┌──────────────────────────────┐
                  │      Smart Meter Agent       │
                  │            C++17             │
                  │                              │
                  │ Pulse → Energy → Power      │
                  │ Cost + Anomaly Detection     │
                  └───────┬───────────┬──────────┘
                          │           │
                          │           │
                          ▼           ▼
                ┌──────────────┐  ┌────────────────┐
                │ CSV Storage  │  │ Forecast Module│
                │ Time Series  │  │      C++       │
                └──────┬───────┘  └───────┬────────┘
                       │                  │
                       └────────┬─────────┘
                                ▼
                   ┌──────────────────────────┐
                   │   Terminal Dashboard     │
                   │          C++             │
                   └──────────────────────────┘
