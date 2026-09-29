import psycopg

DATABASE_URL = "postgresql://postgres:12345678@localhost:5432/smart_meter_db"

with psycopg.connect(DATABASE_URL) as connection:

    with connection.cursor() as cursor:

        cursor.execute("""
            SELECT
                meter_id,
                timestamp,
                pulse_count,
                energy_kwh,
                power_w
            FROM meter_data
            ORDER BY timestamp;
        """)

        rows = cursor.fetchall()

        print("Smart Meter Analytics")
        print("=====================")

        power_values = []
        energy_values = []

        for row in rows:

            meter_id, timestamp, pulse_count, energy_kwh, power_w = row

            print(
                f"Meter: {meter_id} | "
                f"Time: {timestamp} | "
                f"Pulses: {pulse_count} | "
                f"Energy: {energy_kwh} kWh | "
                f"Power: {power_w} W"
            )

            power_values.append(power_w)
            energy_values.append(energy_kwh)

        # Calculate statistics
        if power_values:

            average_power = sum(power_values) / len(power_values)
            minimum_power = min(power_values)
            maximum_power = max(power_values)
            total_energy = max(energy_values)

            print("\n--- Analytics Summary ---")
            print(f"Average Power: {average_power:.2f} W")
            print(f"Minimum Power: {minimum_power:.2f} W")
            print(f"Maximum Power: {maximum_power:.2f} W")
            print(f"Total Energy: {total_energy:.6f} kWh")

        # Anomaly detection
        anomaly_threshold = 6000

        print("\n--- Anomaly Detection ---")

        for row in rows:

            meter_id, timestamp, pulse_count, energy_kwh, power_w = row

            if power_w > anomaly_threshold:
                print(
                    f"ANOMALY detected! "
                    f"Meter: {meter_id} | "
                    f"Time: {timestamp} | "
                    f"Power: {power_w} W"
                )
            else:
                print(
                    f"Normal: "
                    f"Meter: {meter_id} | "
                    f"Power: {power_w} W"
                )

        # 3-reading moving average
        window_size = 3

        print("\n--- Power Moving Average ---")

        for i in range(len(power_values)):

            start = max(0, i - window_size + 1)

            window = power_values[start:i + 1]

            moving_average = sum(window) / len(window)

            print(
                f"Reading {i + 1}: "
                f"Power = {power_values[i]:.2f} W | "
                f"Moving Average = {moving_average:.2f} W"
            )