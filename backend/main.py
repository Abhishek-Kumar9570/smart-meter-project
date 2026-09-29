import os
from dotenv import load_dotenv

load_dotenv()
from fastapi import FastAPI, Header, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
from sqlalchemy import create_engine, text
from analytics.forecast import moving_average_forecast, forecast_energy
from datetime import datetime



app = FastAPI()
API_KEY = os.getenv("API_KEY")

if not API_KEY:
    raise RuntimeError("API_KEY environment variable is not set.")
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:5173"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# PostgreSQL connection
DATABASE_URL = os.getenv("DATABASE_URL")

if not DATABASE_URL:
    raise RuntimeError("DATABASE_URL environment variable is not set.")

engine = create_engine(DATABASE_URL)

# Test database connection
try:
    with engine.connect() as connection:
        connection.execute(text("SELECT 1"))
    print("PostgreSQL connection successful")
except Exception as e:
    print("PostgreSQL connection failed:", e)


class MeterData(BaseModel):
    meterId: str
    timestamp: str
    pulseCount: int
    impulseConstant: float
    energyKWh: float
    powerW: float


@app.get("/")
def home():
    return {
        "message": "Smart Meter Backend is running"
    }


@app.post("/meter-data")
def receive_meter_data(
    data: MeterData,
    x_api_key: str = Header(None)
):
    if x_api_key != API_KEY:
        raise HTTPException(
            status_code=401,
            detail="Invalid API key"
        )

    query = text("""
        INSERT INTO meter_data
        (meter_id, timestamp, pulse_count, impulse_constant, energy_kwh, power_w)
        VALUES
        (:meter_id, :timestamp, :pulse_count, :impulse_constant, :energy_kwh, :power_w)
    """)

    with engine.begin() as connection:
        connection.execute(query, {
            "meter_id": data.meterId,
            "timestamp": data.timestamp,
            "pulse_count": data.pulseCount,
            "impulse_constant": data.impulseConstant,
            "energy_kwh": data.energyKWh,
            "power_w": data.powerW
        })

    return {
        "message": "Meter data saved successfully",
        "data": data
    }


@app.get("/meter-data")
def get_meter_data():

    query = text("""
        SELECT
            id,
            meter_id,
            timestamp,
            pulse_count,
            impulse_constant,
            energy_kwh,
            power_w
        FROM meter_data
        ORDER BY id DESC
    """)

    with engine.connect() as connection:
        result = connection.execute(query)
        rows = result.mappings().all()

    return {
        "count": len(rows),
        "data": rows
    }


@app.get("/analytics")
def get_analytics(meterId: str = None):

    if meterId:
        query = text("""
            SELECT
                meter_id,
                timestamp,
                power_w,
                energy_kwh
            FROM meter_data
            WHERE meter_id = :meter_id
            ORDER BY timestamp;
        """)

        query_params = {
            "meter_id": meterId
        }

    else:
        query = text("""
            SELECT
                meter_id,
                timestamp,
                power_w,
                energy_kwh
            FROM meter_data
            ORDER BY timestamp;
        """)

        query_params = {}

    with engine.connect() as connection:
        result = connection.execute(query, query_params)
        rows = result.fetchall()

    if not rows:
        return {
            "message": "No meter data available"
        }

    power_values = [float(row[2]) for row in rows]
    energy_values = [float(row[3]) for row in rows]

    average_power = sum(power_values) / len(power_values)
    positive_power_values = [power for power in power_values if power > 0]
    minimum_power = min(positive_power_values) if positive_power_values else 0
    maximum_power = max(power_values)
    total_energy = float(rows[-1][3])

    config_path = "data/meter_configs.txt"
    cost_per_kwh = None

    try:
       with open(config_path, "r") as file:
         for line in file:
            parts = line.strip().split(",")

            if len(parts) == 4 and parts[0].strip() == (meterId or rows[-1][0]):
                cost_per_kwh = float(parts[2].strip())
                break
    except (FileNotFoundError, ValueError):
        cost_per_kwh = None

    if cost_per_kwh is None:
        return {
          "message": "Meter configuration not found"
        }

    estimated_cost = total_energy * cost_per_kwh

    # Latest meter reading
    latest_row = rows[-1]

    meter_id = latest_row[0]
    latest_timestamp = latest_row[1]
    latest_power = float(latest_row[2])

    # Dynamic anomaly detection
    # Recent-data anomaly detection
    recent_power_values = power_values[-20:]

    recent_mean = (
        sum(recent_power_values) /
        len(recent_power_values)
    )

    if len(recent_power_values) > 1:
        variance = sum(
            (power - recent_mean) ** 2
            for power in recent_power_values
        ) / len(recent_power_values)

        recent_std = variance ** 0.5
    else:
        recent_std = 0


    # Default status
    anomaly_status = "NORMAL"
    anomaly_type = "NONE"
    anomaly_reason = "Power consumption is within normal range."


    # -------------------------------------------------
    # 1. HIGH POWER SPIKE DETECTION
    # -------------------------------------------------

    high_threshold = recent_mean + (2 * recent_std)

    if latest_power > high_threshold:
        anomaly_status = "ANOMALY"
        anomaly_type = "HIGH_POWER_SPIKE"
        anomaly_reason = (
            "Power consumption is significantly higher "
            "than the recent average."
        )


    # -------------------------------------------------
    # 2. SUDDEN POWER CHANGE DETECTION
    # -------------------------------------------------

    if len(power_values) >= 2:

        previous_power = power_values[-2]

        if previous_power > 0:

            increase_ratio = latest_power / previous_power
            decrease_ratio = latest_power / previous_power

            # Sudden increase of more than 75%
            if increase_ratio >= 1.75:
                anomaly_status = "ANOMALY"
                anomaly_type = "SUDDEN_POWER_INCREASE"
                anomaly_reason = (
                    "Power increased suddenly compared "
                    "with the previous reading."
                )

            # Sudden decrease of more than 60%
            elif decrease_ratio <= 0.40:
                anomaly_status = "ANOMALY"
                anomaly_type = "SUDDEN_POWER_DROP"
                anomaly_reason = (
                    "Power decreased suddenly compared "
                    "with the previous reading."
                )

    # Determine meter status from the latest reading
    current_time = datetime.now()
    time_difference = (
        current_time - latest_timestamp
    ).total_seconds()

    if time_difference <= 30:
        meter_status = "ACTIVE"
    else:
        meter_status = "OFFLINE"

    return {
        "meterId": meter_id,
        "status": meter_status,
        "latestTimestamp": str(latest_timestamp),
        "latestPower": latest_power,
        "anomalyStatus": anomaly_status,
        "anomalyType": anomaly_type,
        "anomalyReason": anomaly_reason,
        "averagePower": round(average_power, 2),
        "minimumPower": minimum_power,
        "maximumPower": maximum_power,
        "totalEnergy": total_energy,
        "costPerKWh": cost_per_kwh,
        "estimatedCost": round(estimated_cost, 2)
    }

@app.get("/meters")
def get_meters():

    query = text("""
        SELECT DISTINCT meter_id
        FROM meter_data
        WHERE meter_id IS NOT NULL
        AND meter_id <> ''
        ORDER BY meter_id;
    """)

    with engine.connect() as connection:
        result = connection.execute(query)
        rows = result.fetchall()

    return {
        "meters": [
            row[0]
            for row in rows
        ]
    }

class TariffUpdate(BaseModel):
    meterId: str
    costPerKWh: float


@app.put("/tariff")
def update_tariff(data: TariffUpdate):

    if not data.meterId:
        return {
            "message": "Meter ID is required"
        }

    if data.costPerKWh <= 0:
        return {
            "message": "Tariff must be greater than 0"
        }

    config_path = "data/meter_configs.txt"

    try:
        with open(config_path, "r") as file:
            lines = file.readlines()

        updated = False
        new_lines = []

        for line in lines:
            parts = line.strip().split(",")

            if len(parts) == 4 and parts[0].strip() == data.meterId:
                parts[2] = str(data.costPerKWh)
                new_lines.append(",".join(parts) + "\n")
                updated = True
            else:
                new_lines.append(line)

        if not updated:
            return {
                "message": "Meter configuration not found",
                "meterId": data.meterId
            }

        with open(config_path, "w") as file:
            file.writelines(new_lines)

        return {
            "message": "Tariff updated successfully",
            "meterId": data.meterId,
            "costPerKWh": data.costPerKWh
        }

    except Exception as e:
        return {
            "message": "Failed to update tariff",
            "error": str(e)
        }


@app.get("/meter-config")
def get_meter_config(meterId: str = None):

    config_path = "data/meter_configs.txt"

    try:
        with open(config_path, "r") as file:
            lines = file.readlines()

        for line in lines:

            line = line.strip()

            if not line:
                continue

            parts = line.split(",")

            if len(parts) != 4:
                continue

            configured_meter_id = parts[0].strip()

            if meterId and meterId == configured_meter_id:

                return {
                    "meterId": configured_meter_id,
                    "impulseConstant": float(parts[1].strip()),
                    "costPerKWh": float(parts[2].strip()),
                    "measurementInterval": float(parts[3].strip())
                }

        return {
            "message": "Configuration not available for this meter",
            "meterId": meterId
        }

    except Exception as e:
        return {
            "message": "Failed to read meter configuration",
            "error": str(e)
        }




@app.get("/power-history")
def get_power_history(meterId: str = None):

    if meterId:
        query = text("""
            SELECT
                timestamp,
                power_w
            FROM meter_data
            WHERE meter_id = :meter_id
            ORDER BY timestamp;
        """)

        query_params = {
            "meter_id": meterId
        }

    else:
        query = text("""
            SELECT
                timestamp,
                power_w
            FROM meter_data
            ORDER BY timestamp;
        """)

        query_params = {}

    with engine.connect() as connection:
        result = connection.execute(query, query_params)
        rows = result.fetchall()

    return {
        "count": len(rows),
        "data": [
            {
                "timestamp": str(row[0]),
                "power": float(row[1])
            }
            for row in rows
        ]
    }


@app.get("/latest-reading")
def get_latest_reading(meterId: str = None):

    if meterId:
        query = text("""
            SELECT
                meter_id,
                timestamp,
                pulse_count,
                energy_kwh,
                power_w
            FROM meter_data
            WHERE meter_id = :meter_id
            ORDER BY id DESC
            LIMIT 1;
        """)

        query_params = {
            "meter_id": meterId
        }

    else:
        query = text("""
            SELECT
                meter_id,
                timestamp,
                pulse_count,
                energy_kwh,
                power_w
            FROM meter_data
            ORDER BY id DESC
            LIMIT 1;
        """)

        query_params = {}

    with engine.connect() as connection:
        result = connection.execute(query, query_params)
        row = result.fetchone()

    if not row:
        return {
            "message": "No meter data available"
        }

    return {
        "meterId": row[0],
        "timestamp": str(row[1]),
        "pulseCount": row[2],
        "energyKWh": float(row[3]),
        "powerW": float(row[4])
    }

@app.get("/forecast")
def get_forecast(meterId: str = None):

    if meterId:
        query = text("""
            SELECT power_w
            FROM meter_data
            WHERE meter_id = :meter_id
            ORDER BY timestamp;
        """)

        query_params = {
            "meter_id": meterId
        }

    else:
        query = text("""
            SELECT power_w
            FROM meter_data
            ORDER BY timestamp;
        """)

        query_params = {}

    with engine.connect() as connection:
        result = connection.execute(query, query_params)
        rows = result.fetchall()

    if not rows:
        return {"message": "No meter data available"}

    power_values = [
        float(row[0])
        for row in rows
        if float(row[0]) > 0
    ]

    if not power_values:
        return {"message": "No valid power data available"}

    forecast = forecast_energy(
        power_values,
        window=5
    )

    return {
        "forecastPower": forecast["forecastPower"],
        "nextHourKWh": forecast["nextHourKWh"],
        "nextDayKWh": forecast["nextDayKWh"],
        "powerUnit": "W",
        "energyUnit": "kWh",
        "method": "5-reading Moving Average",
        "window": 5
    }