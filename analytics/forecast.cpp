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
    long pulseCount;
    double energyKWh;
};

int main(int argc, char* argv[])
{
    string meterId = "M001";

    if (argc >= 2) {
        meterId = argv[1];
    }

    const double sampleIntervalSeconds = 10.0;
    const string filename = "data/meter_readings.csv";

    ifstream file(filename);

    if (!file) {
        cerr << "ERROR: Unable to open " << filename << "\n";
        return 1;
    }

    string line;
    getline(file, line); // Skip CSV header.

    vector<Reading> readings;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        string timestamp;
        string id;
        string pulseText;
        string energyText;
        string powerText;
        string costText;

        stringstream ss(line);

        if (!getline(ss, timestamp, ',') ||
            !getline(ss, id, ',') ||
            !getline(ss, pulseText, ',') ||
            !getline(ss, energyText, ',') ||
            !getline(ss, powerText, ',') ||
            !getline(ss, costText)) {
            continue;
        }

        if (id != meterId) {
            continue;
        }

        try {
            Reading r;
            r.timestamp = timestamp;
            r.meterId = id;
            r.pulseCount = stol(pulseText);
            r.energyKWh = stod(energyText);
            readings.push_back(r);
        }
        catch (...) {
            continue;
        }
    }

    if (readings.empty()) {
        cerr << "ERROR: No readings found for meter " << meterId << "\n";
        return 1;
    }

    double totalEnergy = 0.0;

    for (const auto& reading : readings) {
        totalEnergy += reading.energyKWh;
    }

    double averageEnergyPerSample =
        totalEnergy / static_cast<double>(readings.size());

    double samplesPerHour = 3600.0 / sampleIntervalSeconds;
    double samplesPerDay = 86400.0 / sampleIntervalSeconds;

    double nextHourEnergy =
        averageEnergyPerSample * samplesPerHour;

    double nextDayEnergy =
        averageEnergyPerSample * samplesPerDay;

    cout << "\n==========================================\n";
    cout << " SMART METER ENERGY FORECAST\n";
    cout << "==========================================\n";
    cout << "Meter ID              : " << meterId << "\n";
    cout << "Historical samples    : " << readings.size() << "\n";
    cout << fixed << setprecision(6);
    cout << "Average energy/sample : "
         << averageEnergyPerSample << " kWh\n";
    cout << "Next 1-hour forecast  : "
         << nextHourEnergy << " kWh\n";
    cout << "Next 24-hour forecast : "
         << nextDayEnergy << " kWh\n";
    cout << "==========================================\n";

    return 0;
}
