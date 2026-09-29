#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <thread>
#include <chrono>
#include <cstdio>
#include <cstdlib>
using namespace std;

string loadApiKey()
{
    ifstream envFile(".env");
    string line;

    while (getline(envFile, line))
    {
        if (line.rfind("API_KEY=", 0) == 0)
        {
            return line.substr(8);
        }
    }

    const char *envKey = getenv("API_KEY");

    if (envKey != nullptr)
    {
        return string(envKey);
    }

    return "";
}

void savePendingReading(const string &jsonData)
{
    if (jsonData.empty())
    {
        cout << "Error: Cannot buffer empty JSON data." << endl;
        return;
    }

    // Make sure the JSON object is complete before buffering it.
    if (jsonData.front() != '{' || jsonData.back() != '}')
    {
        cout << "Error: JSON data is incomplete. Reading was not buffered."
             << endl;
        return;
    }

    ofstream pendingFile(
        "data/pending_readings.txt",
        ios::app);

    if (pendingFile)
    {
        pendingFile << jsonData << '\n';
        pendingFile.flush();
        pendingFile.close();

        cout << "Reading stored in local buffer." << endl;
    }
    else
    {
        cout << "Error: Could not open local buffer file." << endl;
    }
}

void forwardPendingReadings()
{
    ifstream pendingFile("data/pending_readings.txt");

    if (!pendingFile)
    {
        return;
    }

    string jsonData;
    string remainingData = "";

    while (getline(pendingFile, jsonData))
    {
        if (jsonData.empty())
        {
            continue;
        }

        ofstream tempFile("data/forward.json");

        if (!tempFile)
        {
            remainingData += jsonData + "\n";
            continue;
        }

        tempFile << jsonData;
        tempFile.close();

        string apiKey = loadApiKey();

        if (apiKey.empty())
        {
            cout << "Error: API_KEY is not configured." << endl;
            remainingData += jsonData + "\n";
            continue;
        }

        string curlCommand =
            "curl -f -s -o NUL "
            "-X POST http://127.0.0.1:8000/meter-data "
            "-H \"Content-Type: application/json\" "
            "-H \"X-API-Key: " +
            apiKey + "\" "
                     "--data-binary @data/forward.json";

        int result = system(curlCommand.c_str());

        if (result != 0)
        {
            remainingData += jsonData + "\n";
        }
        else
        {
            cout << "Pending reading forwarded successfully." << endl;
        }
    }

    pendingFile.close();

    ofstream updateFile("data/pending_readings.txt");

    if (updateFile)
    {
        updateFile << remainingData;
        updateFile.close();
    }

    remove("data/forward.json");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Usage: pulse_processor.exe M001" << endl;
        cout << "       pulse_processor.exe M002" << endl;
        return 1;
    }

    string selectedMeterId = argv[1];

    if (selectedMeterId != "M001" &&
        selectedMeterId != "M002")
    {
        cout << "Error: Unsupported meter ID: "
             << selectedMeterId << endl;

        return 1;
    }
    // Print the actual working directory so relative paths are visible
    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    cout << "Working directory: " << cwd << endl;

    // Meter configuration
    double impulseConstant;
    double measurementSeconds;
    double costPerKWh;
    string configuredMeterId = selectedMeterId;
    string meterJsonFile = "tests/meter_data_" + configuredMeterId + ".json";

    ifstream configFile("data/meter_configs.txt");

    if (!configFile)
    {
        cout << "Error: Could not open meter configurations file." << endl;
        return 1;
    }

    string configLine;
    bool configFound = false;

    while (getline(configFile, configLine))
    {
        if (configLine.empty())
            continue;

        size_t pos1 = configLine.find(',');
        size_t pos2 = configLine.find(',', pos1 + 1);
        size_t pos3 = configLine.find(',', pos2 + 1);

        if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos)
            continue;

        string meterId = configLine.substr(0, pos1);

        if (meterId == configuredMeterId)
        {
            impulseConstant = stod(configLine.substr(pos1 + 1, pos2 - pos1 - 1));
            costPerKWh = stod(configLine.substr(pos2 + 1, pos3 - pos2 - 1));
            measurementSeconds = stod(configLine.substr(pos3 + 1));
            configFound = true;
            break;
        }
    }

    configFile.close();

    if (!configFound)
    {
        cout << "Error: Configuration not found for meter " << configuredMeterId << endl;
        return 1;
    }

    cout << "Meter Configuration Loaded" << endl;
    cout << "Meter ID: " << configuredMeterId << endl;
    cout << "Impulse Constant: " << impulseConstant << endl;
    cout << "Tariff: Rs. " << costPerKWh << " per kWh" << endl;
    cout << "Measurement Interval: "
         << measurementSeconds << " seconds" << endl;

    string meterId;
    int pulseCount = 0;
    int previousPulseCount = 0;
    bool firstReading = true;

    string timestamp;
    string previousTimestamp = "";

    auto previousTime = chrono::steady_clock::now();

    while (true)
    {
        forwardPendingReadings();

        // Open latest pulse data
        // Open latest pulse data based on configured meter
        string pulseDataFile;

        if (configuredMeterId == "M001")
        {
            pulseDataFile = "tests/pulse_data.txt";
        }
        else if (configuredMeterId == "M002")
        {
            pulseDataFile = "tests/pulse_data_m002.txt";
        }
        else
        {
            cout << "Error: No pulse data file configured for meter "
                 << configuredMeterId << endl;

            Sleep(2000);
            continue;
        }

        ifstream file(pulseDataFile);
        if (!file)
        {
            cout << "Error: Could not open pulse data file. Retrying..." << endl;
            Sleep(2000);
            continue;
        }

        // Read Meter ID
        getline(file, meterId, ',');

        // Read pulse count
        file >> pulseCount;

        // Remove comma after pulse count
        file.ignore(1);

        // Read timestamp
        getline(file, timestamp);

        // Software debounce:
        // Ignore duplicate meter readings with the same timestamp.
        if (timestamp == previousTimestamp)
        {
            file.close();
            Sleep(1000); // <-- prevent busy loop
            continue;
        }

        previousTimestamp = timestamp;

        file.close();

        // Calculate new pulses since the previous reading
        int newPulses = 0;

        if (!firstReading)
        {
            newPulses = pulseCount - previousPulseCount;

            if (newPulses < 0)
            {
                newPulses = 0;
            }
        }

        previousPulseCount = pulseCount;
        firstReading = false;

        // Calculate cumulative energy
        double cumulativeEnergyKWh =
            pulseCount / impulseConstant;

        // Calculate estimated cost
        double estimatedCost =
            cumulativeEnergyKWh * costPerKWh;

        // Calculate power consumption
        double intervalEnergyKWh = newPulses / impulseConstant;

        auto currentTime = chrono::steady_clock::now();

        double elapsedSeconds = chrono::duration<double>(currentTime - previousTime).count();

        previousTime = currentTime;

        double powerW = 0;

        // Require a minimum elapsed time to avoid absurd power spikes
        if (newPulses > 0 && elapsedSeconds > 0.5)
        {
            double timeHours = elapsedSeconds / 3600.0;
            powerW = (intervalEnergyKWh / timeHours) * 1000;
        }

        string status = "ACTIVE";

        cout << "\n=================================" << endl;
        cout << "       SMART METER READING       " << endl;
        cout << "=================================" << endl;

        cout << "Meter ID: " << meterId << endl;
        cout << "Status: " << status << endl;
        cout << "Total pulses: " << pulseCount << endl;
        cout << "Timestamp: " << timestamp << endl;
        cout << "Cumulative energy: "
             << cumulativeEnergyKWh << " kWh" << endl;
        cout << "Estimated cost: Rs. "
             << estimatedCost << endl;
        cout << "Power consumption: "
             << powerW << " W" << endl;

        // Create JSON file
        ofstream jsonFile(meterJsonFile);

        if (jsonFile)
        {
            jsonFile << "{"
                     << "\"meterId\":\"" << meterId << "\","
                     << "\"timestamp\":\"" << timestamp << "\","
                     << "\"pulseCount\":" << pulseCount << ","
                     << "\"impulseConstant\":" << impulseConstant << ","
                     << "\"energyKWh\":" << cumulativeEnergyKWh << ","
                     << "\"powerW\":" << powerW
                     << "}" << endl;

            jsonFile.close();

            // Save a local copy for store-and-forward
            ifstream savedJson(meterJsonFile);

            cout << "Meter data saved to JSON." << endl;

            // Send data to FastAPI.
            // -s : silent progress
            // -S : still show errors
            // -f : fail (non-zero exit) on HTTP >= 400
            // -o NUL : discard the response body
            // 2> tests/curl_error.txt : capture curl's error output
            string apiKey = loadApiKey();

            string curlCommand =
                "curl -s -S -f -o NUL "
                "-X POST http://127.0.0.1:8000/meter-data "
                "-H \"Content-Type: application/json\" "
                "-H \"X-API-Key: " +
                apiKey + "\" "
                         "--data-binary \"@" +
                meterJsonFile + "\" "
                                "2> tests/curl_error.txt";

            int result = system(curlCommand.c_str());

            if (result == 0)
            {
                cout << "Meter data sent to FastAPI successfully."
                     << endl;
            }
            else
            {
                cout << "Error: Failed to send meter data (raw code = "
                     << result << ")." << endl;

                // Save failed reading for store-and-forward
                ifstream failedJson(meterJsonFile);

                if (failedJson)
                {
                    string jsonData(
                        (istreambuf_iterator<char>(failedJson)),
                        istreambuf_iterator<char>());

                    failedJson.close();

                    savePendingReading(jsonData);

                    cout << "Reading stored in local buffer." << endl;
                }
                else
                {
                    cout << "Error: Could not read JSON for local buffering."
                         << endl;
                }

                // Show what curl actually said
                ifstream errFile("tests/curl_error.txt");
                if (errFile)
                {
                    string line;
                    while (getline(errFile, line))
                    {
                        if (!line.empty())
                            cout << "  curl: " << line << endl;
                    }
                    errFile.close();
                }
            }
        }
        else
        {
            cout << "Error: Could not create JSON file." << endl;
        }

        // Wait 5 seconds before next reading
        cout << "Waiting " << measurementSeconds << " seconds for next reading..." << endl;

        Sleep(static_cast<DWORD>(measurementSeconds * 1000));
    }

    return 0;
}
