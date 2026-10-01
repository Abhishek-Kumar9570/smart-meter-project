#include <fcntl.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>

using namespace std;

static const char* DEVICE = "/dev/virtual_meter";

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
    } catch (...) {
        return -1;
    }
}

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

int main()
{
    cout << "========================================\n";
    cout << " Linux Smart Meter Pulse Simulator\n";
    cout << " Device: " << DEVICE << "\n";
    cout << "========================================\n";

    long pulseCount = readPulseCount();

    if (pulseCount < 0) {
        cerr << "ERROR: Cannot read " << DEVICE << "\n";
        cerr << "Make sure the virtual meter driver is loaded.\n";
        return 1;
    }

    cout << "Starting pulse count: " << pulseCount << "\n";
    cout << "Generating 1 pulse every second.\n";
    cout << "Press Ctrl+C to stop.\n\n";

    while (true) {
        ++pulseCount;

        if (!writePulseCount(pulseCount)) {
            cerr << "ERROR: Failed to update pulse count.\n";
            return 1;
        }

        cout << "Pulse generated -> Total pulses: "
             << pulseCount << "\n";

        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}
