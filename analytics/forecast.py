def moving_average_forecast(power_values, window=5):
    if not power_values:
        return 0

    recent_values = power_values[-window:]

    forecast = sum(recent_values) / len(recent_values)

    return round(forecast, 2)


def forecast_energy(power_values, window=5):
    if not power_values:
        return {
            "forecastPower": 0,
            "nextHourKWh": 0,
            "nextDayKWh": 0
        }

    recent_values = power_values[-window:]

    average_power_w = sum(recent_values) / len(recent_values)

    # Convert average power from watts to kWh for one hour
    next_hour_kwh = average_power_w / 1000

    # Estimate energy consumption for 24 hours
    next_day_kwh = next_hour_kwh * 24

    return {
        "forecastPower": round(average_power_w, 2),
        "nextHourKWh": round(next_hour_kwh, 4),
        "nextDayKWh": round(next_day_kwh, 4)
    }