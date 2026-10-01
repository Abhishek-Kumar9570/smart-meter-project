#include <chrono>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

using namespace std;

static const char* DEVICE = "/dev/virtual_meter";

bool writePulseCount(long count)
{
    int fd = open(DEVICE, O_WRONLY);

    if (fd < 0) {
        return false;
    }

    string value = to_string(count);
    ssize_t written = write(fd, value.c_str(), value.size());

    close(fd);

    return written == static_cast<ssize_t>(value.size());
}

long readPulseCount()
{
    int fd = open(DEVICE, O_RDONLY);

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
    }
    catch (...) {
        return -1;
    }
}

int main()
{
    cout << "========================================\n";
    cout << " Simulated-Real Smart Meter - M002\n";
    cout << " Device: " << DEVICE << "\n";
    cout << "========================================\n";

    long pulseCount = readPulseCount();

    if (pulseCount < 0) {
        cerr << "ERROR: Cannot access " << DEVICE << "\n";
        return 1;
    }

    cout << "Starting pulse count: " << pulseCount << "\n";
    cout << "Simulating variable real-world load.\n";
    cout << "Press Ctrl+C to stop.\n\n";

    int cycle = 0;

    while (true) {
        int pulsesThisSecond;

        switch (cycle % 12) {
            case 0:
            case 1:
            case 2:
                pulsesThisSecond = 1;   // Low load
                break;

            case 3:
            case 4:
            case 5:
            case 6:
                pulsesThisSecond = 3;   // Normal load
                break;

            case 7:
            case 8:
                pulsesThisSecond = 6;   // High load
                break;

            default:
                pulsesThisSecond = 2;   // Medium load
                break;
        }

        pulseCount += pulsesThisSecond;

        if (!writePulseCount(pulseCount)) {
            cerr << "ERROR: Failed to update pulse count.\n";
            return 1;
        }

        cout << "M002 load update -> +"
             << pulsesThisSecond
             << " pulses, total: "
             << pulseCount << "\n";

        ++cycle;
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
