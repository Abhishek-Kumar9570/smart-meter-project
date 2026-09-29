import time
import random
from datetime import datetime

pulse_file = "tests/m001_pulse_count.txt"

try:
    with open(pulse_file, "r") as file:
        pulse_count = int(file.read().strip())
except (FileNotFoundError, ValueError):
    pulse_count = 0

print("Virtual Smart Meter Started")
print("Generating readings continuously...\n")

while True:

    # Simulate different electricity loads
    load = random.choice(["LOW", "MEDIUM", "HIGH"])

    if load == "LOW":
        delay = 3
    elif load == "MEDIUM":
        delay = 1
    else:
        delay = 0.5

    # Generate a new pulse
    pulse_count += 1
    with open(pulse_file, "w") as file:
      file.write(str(pulse_count))

    # Generate current timestamp
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    # Save latest meter reading
    with open("tests/pulse_data.txt", "w") as file:
        file.write(f"M001,{pulse_count},{timestamp}")

    print(
        f"Load: {load} | "
        f"Pulse: {pulse_count} | "
        f"Time: {timestamp}"
    )

    time.sleep(delay)