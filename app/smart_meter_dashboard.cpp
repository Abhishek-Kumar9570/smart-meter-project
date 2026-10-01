#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    string requestedMeterId = "M001";

    if (argc >= 2) {
        requestedMeterId = argv[1];
    }

    const string filename = "data/meter_readings.csv";

    ifstream file(filename);

    if (!file) {
        cerr << "ERROR: Unable to open " << filename << "\n";
        return 1;
    }

    string line;
    string lastTimestamp;
    string lastMeterId;
    long lastPulses = 0;
    double lastEnergy = 0.0;
    double lastPower = 0.0;
    double lastCost = 0.0;

    getline(file, line); // Skip header.

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        string timestamp;
        string meterId;
        string pulseText;
        string energyText;
        string powerText;
        string costText;

        stringstream ss(line);

        if (!getline(ss, timestamp, ',') ||
            !getline(ss, meterId, ',') ||
            !getline(ss, pulseText, ',') ||
            !getline(ss, energyText, ',') ||
            !getline(ss, powerText, ',') ||
            !getline(ss, costText)) {
            continue;
        }

        try {
            if (meterId != requestedMeterId) {
                continue;
            }

            lastTimestamp = timestamp;
            lastMeterId = meterId;
            lastPulses = stol(pulseText);
            lastEnergy = stod(energyText);
            lastPower = stod(powerText);
            lastCost = stod(costText);
        }
        catch (...) {
            continue;
        }
    }

    if (lastMeterId.empty()) {
        cerr << "ERROR: No valid meter readings found.\n";
        return 1;
    }

    cout << "\n====================================================\n";
    cout << "           SMART ENERGY METER DASHBOARD\n";
    cout << "====================================================\n";
    cout << "Meter ID          : " << lastMeterId << "\n";
    cout << "Last Timestamp    : " << lastTimestamp << "\n";
    cout << "Pulse Count       : " << lastPulses << "\n";
    cout << fixed << setprecision(6);
    cout << "Energy             : " << lastEnergy << " kWh\n";
    cout << fixed << setprecision(2);
    cout << "Power              : " << lastPower << " W\n";
    cout << "Estimated Cost     : Rs. " << lastCost << "\n";
    cout << "Device             : /dev/virtual_meter\n";
    cout << "Platform           : Linux\n";
    cout << "Implementation     : C++\n";
    cout << "Status             : DATA AVAILABLE\n";
    cout << "====================================================\n";

    return 0;
}
