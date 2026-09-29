import time
from datetime import datetime

pulse_file = "tests/m002_pulse_count.txt"

try:
    with open(pulse_file, "r") as file:
        pulse_count = int(file.read().strip())
except (FileNotFoundError, ValueError):
    pulse_count = 0

print("Simulated Real Smart Meter M002 Started")
print("Generating realistic load readings...\n")

while True:

    # Simulate a realistic household load pattern
    pulse_count += 1
    with open(pulse_file, "w") as file:
      file.write(str(pulse_count))

    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    with open("tests/pulse_data_m002.txt", "w") as file:
        file.write(f"M002,{pulse_count},{timestamp}")

    print(
        f"Meter: M002 | "
        f"Pulse: {pulse_count} | "
        f"Time: {timestamp}"
    )

    # Simulate approximately one pulse every 2 seconds
    time.sleep(2)