#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Reading {
    string timestamp;
    string meterId;
    long pulses;
    double energy;
    double cumulative;
    double power;
    double cost;
};

string getAnomalyStatus(
    const Reading& current,
    const Reading* previous)
{
    if (previous != nullptr && current.pulses < 0) {
        return "TAMPER/COUNTER RESET";
    }

    if (current.pulses == 0) {
        return "NO-PULSE";
    }

    if (current.power > 5000.0) {
        return "POWER SPIKE";
    }

    if (previous != nullptr &&
        previous->power > 1000.0 &&
        current.power < previous->power * 0.5) {
        return "POWER DROP";
    }

    if (current.pulses >= 20) {
        return "PULSE INCREASE";
    }

    return "NORMAL";
}

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
    getline(file, line); // Skip header.

    vector<Reading> readings;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        string timestamp;
        string meterId;
        string pulseText;
        string energyText;
        string cumulativeText;
        string powerText;
        string costText;

        stringstream ss(line);

        if (!getline(ss, timestamp, ',') ||
            !getline(ss, meterId, ',') ||
            !getline(ss, pulseText, ',') ||
            !getline(ss, energyText, ',') ||
            !getline(ss, cumulativeText, ',') ||
            !getline(ss, powerText, ',') ||
            !getline(ss, costText)) {
            continue;
        }

        if (meterId != requestedMeterId) {
            continue;
        }

        try {
            Reading reading{};
            reading.timestamp = timestamp;
            reading.meterId = meterId;
            reading.pulses = stol(pulseText);
            reading.energy = stod(energyText);
            reading.cumulative = stod(cumulativeText);
            reading.power = stod(powerText);
            reading.cost = stod(costText);

            readings.push_back(reading);
        }
        catch (...) {
            continue;
        }
    }

    if (readings.empty()) {
        cerr << "ERROR: No valid meter readings found for "
             << requestedMeterId << ".\n";
        return 1;
    }

    const Reading& latest = readings.back();

    string latestStatus = "NORMAL";

    if (readings.size() >= 2) {
        latestStatus = getAnomalyStatus(
            latest,
            &readings[readings.size() - 2]);
    } else {
        latestStatus = getAnomalyStatus(latest, nullptr);
    }

    size_t alertCount = 0;

    for (size_t i = 0; i < readings.size(); ++i) {
        const Reading* previous = nullptr;

        if (i > 0) {
            previous = &readings[i - 1];
        }

        if (getAnomalyStatus(readings[i], previous) != "NORMAL") {
            ++alertCount;
        }
    }

    cout << "\n====================================================================\n";
    cout << "                  SMART ENERGY METER DASHBOARD\n";
    cout << "====================================================================\n";

    cout << "Meter ID           : " << latest.meterId << "\n";
    cout << "Last Timestamp     : " << latest.timestamp << "\n";
    cout << "Pulse Count        : " << latest.pulses << "\n";

    cout << fixed << setprecision(6);
    cout << "Energy             : " << latest.energy << " kWh\n";
    cout << "Cumulative Energy  : " << latest.cumulative << " kWh\n";

    cout << fixed << setprecision(2);
    cout << "Power              : " << latest.power << " W\n";
    cout << "Estimated Cost     : Rs. " << latest.cost << "\n";
    cout << "Current Status     : " << latestStatus << "\n";
    cout << "Alert Records      : " << alertCount << "\n";
    cout << "Historical Samples : " << readings.size() << "\n";
    cout << "Device             : /dev/virtual_meter\n";
    cout << "Platform           : Linux\n";
    cout << "Implementation     : C++\n";

    cout << "\n------------------------ RECENT HISTORY ----------------------------\n";
    cout << left
         << setw(20) << "Timestamp"
         << setw(10) << "Pulses"
         << setw(12) << "Energy"
         << setw(14) << "Cumulative"
         << setw(12) << "Power"
         << setw(18) << "Status"
         << "\n";

    cout << string(86, '-') << "\n";

    size_t start =
        readings.size() > 5 ? readings.size() - 5 : 0;

    for (size_t i = start; i < readings.size(); ++i) {
        const Reading* previous = nullptr;

        if (i > 0) {
            previous = &readings[i - 1];
        }

        cout << left
             << setw(20) << readings[i].timestamp
             << setw(10) << readings[i].pulses
             << setw(12) << fixed << setprecision(6) << readings[i].energy
             << setw(14) << readings[i].cumulative
             << setw(12) << fixed << setprecision(2) << readings[i].power
             << setw(18) << getAnomalyStatus(readings[i], previous)
             << "\n";
    }

    cout << "====================================================================\n";

    return 0;
}
