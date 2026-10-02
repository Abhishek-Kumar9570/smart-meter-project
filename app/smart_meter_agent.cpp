#include <chrono>
#include <curl/curl.h>
#include <ctime>
#include <cmath>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>

using namespace std;

struct MeterReading {
    string meterId;
    long pulseCount;
    double energyKWh;
    double cumulativeEnergyKWh;
    double powerW;
    double cost;
};

struct MeterConfig {
    double impulseConstant;
    double costPerKWh;
    int sampleIntervalSeconds;
    string cloudEndpoint;
};

MeterConfig loadMeterConfig(const string& meterId)
{
    MeterConfig config{1600.0, 15.0, 5, "http://127.0.0.1:8080/"};

    ifstream file("data/meter_configs.txt");

    if (!file) {
        return config;
    }

    string line;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        string id;
        string impulseText;
        string costText;
        string intervalText;
        string endpointText;

        stringstream ss(line);

        if (!getline(ss, id, ',') ||
            !getline(ss, impulseText, ',') ||
            !getline(ss, costText, ',') ||
            !getline(ss, intervalText, ',')) {
            continue;
        }

        getline(ss, endpointText);

        if (id != meterId) {
            continue;
        }

        try {
            double impulse = stod(impulseText);
            double cost = stod(costText);
            int interval = stoi(intervalText);

            if (impulse > 0.0 && cost >= 0.0 && interval > 0) {
                config.impulseConstant = impulse;
                config.costPerKWh = cost;
                config.sampleIntervalSeconds = interval;

                if (!endpointText.empty()) {
                    config.cloudEndpoint = endpointText;
                }
            }
        }
        catch (...) {
            // Keep default configuration if values are invalid.
        }

        break;
    }

    return config;
}

long readPulseCount()
{
    const char* device = "/dev/virtual_meter";

    int fd = open(device, O_RDONLY);

    if (fd < 0) {
        return -1;
    }

    char buffer[64] = {0};

    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);

    close(fd);

    if (bytesRead <= 0) {
        return -1;
    }

    buffer[bytesRead] = '\0';

    try {
        return stol(string(buffer));
    } catch (...) {
        return -1;
    }
}

MeterReading calculateReading(
    const string& meterId,
    long previousPulses,
    long currentPulses,
    double elapsedSeconds,
    const MeterConfig& config)
{
    MeterReading reading{};

    reading.meterId = meterId;

    long newPulses = currentPulses - previousPulses;

    if (newPulses < 0) {
        newPulses = 0;
    }

    reading.pulseCount = newPulses;

    // Energy = pulses / impulse constant
    reading.energyKWh =
        static_cast<double>(newPulses) / config.impulseConstant;

    // Power = energy(kWh) * 3600 / elapsed time(seconds)
    if (elapsedSeconds > 0.0) {
        reading.powerW =
            reading.energyKWh * 3600.0 / elapsedSeconds * 1000.0;
    } else {
        reading.powerW = 0.0;
    }

    reading.cost = reading.energyKWh * config.costPerKWh;

    return reading;
}

double loadCumulativeEnergy(const string& meterId)
{
    const string filename = "data/meter_readings.csv";
    ifstream file(filename);

    if (!file) {
        return 0.0;
    }

    string line;
    getline(file, line); // Skip header.

    double cumulativeEnergy = 0.0;

    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        string timestamp;
        string id;
        string pulseText;
        string energyText;
        string cumulativeText;
        string powerText;
        string costText;

        stringstream ss(line);

        if (!getline(ss, timestamp, ',') ||
            !getline(ss, id, ',') ||
            !getline(ss, pulseText, ',') ||
            !getline(ss, energyText, ',') ||
            !getline(ss, cumulativeText, ',') ||
            !getline(ss, powerText, ',') ||
            !getline(ss, costText)) {
            continue;
        }

        if (id != meterId) {
            continue;
        }

        try {
            cumulativeEnergy = stod(cumulativeText);
        }
        catch (...) {
            continue;
        }
    }

    return cumulativeEnergy;
}

void saveReading(const MeterReading& reading)
{
    const string filename = "data/meter_readings.csv";

    ofstream file(filename, ios::app);

    if (!file) {
        cerr << "ERROR: Unable to open " << filename << "\n";
        return;
    }

    auto now = chrono::system_clock::now();
    time_t nowTime = chrono::system_clock::to_time_t(now);

    tm localTime{};
    localtime_r(&nowTime, &localTime);

    char timestamp[32];
    strftime(timestamp, sizeof(timestamp),
             "%Y-%m-%d %H:%M:%S", &localTime);

    file << timestamp << ","
         << reading.meterId << ","
         << reading.pulseCount << ","
         << fixed << setprecision(6)
         << reading.energyKWh << ","
         << reading.cumulativeEnergyKWh << ","
         << reading.powerW << ","
         << reading.cost << "\n";
    file.flush();
}


bool sendToCloud(
    const MeterReading& reading,
    const MeterConfig& config)
{
    CURL* curl = curl_easy_init();

    if (!curl) {
        cerr << "Cloud Sync      : FAILED (libcurl initialization)\n";
        return false;
    }

    ostringstream json;
    json << fixed << setprecision(6)
         << "{"
         << "\"meterId\":\"" << reading.meterId << "\","
         << "\"pulseCount\":" << reading.pulseCount << ","
         << "\"energyKWh\":" << reading.energyKWh << ","
         << "\"cumulativeEnergyKWh\":" << reading.cumulativeEnergyKWh << ","
         << "\"powerW\":" << reading.powerW << ","
         << "\"costRs\":" << reading.cost
         << "}";

    const string payload = json.str();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, config.cloudEndpoint.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);

    CURLcode result = curl_easy_perform(curl);

    long httpCode = 0;

    if (result == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result == CURLE_OK && httpCode >= 200 && httpCode < 300) {
        cout << "Cloud Sync      : SENT (HTTP " << httpCode << ")\n";
        return true;
    }

    cout << "Cloud Sync      : FAILED";

    if (result != CURLE_OK) {
        cout << " (" << curl_easy_strerror(result) << ")";
    } else {
        cout << " (HTTP " << httpCode << ")";
    }

    cout << "\n";
    return false;
}

void checkAnomalies(
    long previousPulses,
    long currentPulses,
    double previousPowerW,
    bool hasPreviousReading,
    const MeterReading& reading)
{
    long pulseChange = currentPulses - previousPulses;

    cout << "Anomaly Status : ";

    if (currentPulses < previousPulses) {
        cout << "TAMPER/COUNTER RESET DETECTED";
    }
    else if (pulseChange == 0) {
        cout << "NO-PULSE CONDITION";
    }
    else if (reading.powerW > 5000.0) {
        cout << "POWER SPIKE DETECTED";
    }
    else if (hasPreviousReading &&
             previousPowerW > 1000.0 &&
             reading.powerW < previousPowerW * 0.5) {
        cout << "SUDDEN POWER DROP DETECTED";
    }
    else if (pulseChange >= 20) {
        cout << "SUDDEN PULSE INCREASE";
    }
    else {
        cout << "NORMAL";
    }

    cout << "\n";
}

void printReading(const MeterReading& reading)
{
    cout << "\n========== SMART METER READING ==========\n";
    cout << "Meter ID       : " << reading.meterId << "\n";
    cout << "New Pulses     : " << reading.pulseCount << "\n";
    cout << "Energy         : "
         << fixed << setprecision(6)
         << reading.energyKWh << " kWh\n";
    cout << "Cumulative     : "
         << fixed << setprecision(6)
         << reading.cumulativeEnergyKWh << " kWh\n";
    cout << "Power          : "
         << fixed << setprecision(2)
         << reading.powerW << " W\n";
    cout << "Estimated Cost : Rs. "
         << fixed << setprecision(2)
         << reading.cost << "\n";
    cout << "==========================================\n";
}

int main(int argc, char* argv[])
{
    string meterId = "M001";

    if (argc >= 2) {
        meterId = argv[1];
    }

    cout << "Smart Energy Smart-Meter Agent\n";
    cout << "Meter ID: " << meterId << "\n";
    cout << "Linux device: /dev/virtual_meter\n";

    MeterConfig config = loadMeterConfig(meterId);

    cout << "Impulse Constant: " << config.impulseConstant << "\n";
    cout << "Cost per kWh    : Rs. " << config.costPerKWh << "\n";
    cout << "Sample Interval : " << config.sampleIntervalSeconds
         << " seconds\n";
    cout << "Cloud Endpoint  : " << config.cloudEndpoint << "\n";

    int deviceFd = open("/dev/virtual_meter", O_RDONLY);

    if (deviceFd < 0) {
        cerr << "\nERROR: Could not open /dev/virtual_meter\n";
        cerr << "The Linux kernel driver must be loaded and the device must exist.\n";
        return 1;
    }

    long previousPulses = readPulseCount();

    if (previousPulses < 0) {
        cerr << "ERROR: Unable to read initial pulse count.\n";
        close(deviceFd);
        return 1;
    }

    auto previousTime = chrono::steady_clock::now();

    double cumulativeEnergyKWh = loadCumulativeEnergy(meterId);
    double previousPowerW = 0.0;
    bool hasPreviousReading = false;

    cout << "Initial pulse count: "
         << previousPulses << "\n";
    cout << "Previous cumulative energy: "
         << fixed << setprecision(6)
         << cumulativeEnergyKWh << " kWh\n";

    cout << "Starting monitoring...\n";

    while (true) {

        this_thread::sleep_for(
            chrono::seconds(config.sampleIntervalSeconds));

        long currentPulses = readPulseCount();

        if (currentPulses < 0) {
            cerr << "ERROR: Failed to read pulse count.\n";
            continue;
        }

        auto currentTime = chrono::steady_clock::now();

        double elapsedSeconds =
            chrono::duration<double>(
                currentTime - previousTime).count();

        MeterReading reading =
            calculateReading(
                meterId,
                previousPulses,
                currentPulses,
                elapsedSeconds,
                config);

        cumulativeEnergyKWh += reading.energyKWh;
        reading.cumulativeEnergyKWh = cumulativeEnergyKWh;

        printReading(reading);
        checkAnomalies(
            previousPulses,
            currentPulses,
            previousPowerW,
            hasPreviousReading,
            reading);
        saveReading(reading);
        sendToCloud(reading, config);

        previousPowerW = reading.powerW;
        hasPreviousReading = true;
        previousPulses = currentPulses;
        previousTime = currentTime;
    }

    close(deviceFd);

    return 0;
}
