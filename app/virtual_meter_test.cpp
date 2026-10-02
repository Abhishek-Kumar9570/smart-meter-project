#include <fcntl.h>
#include <iostream>
#include <unistd.h>

using namespace std;

int main()
{
    const char* device = "/dev/virtual_meter";

    cout << "Opening Linux virtual meter device...\n";

    int fd = open(device, O_RDWR);

    if (fd < 0) {
        cerr << "ERROR: Could not open " << device << "\n";
        return 1;
    }

    cout << "Device opened successfully.\n";

    const char* pulseValue = "10";

    ssize_t written = write(fd, pulseValue, 2);

    if (written < 0) {
        cerr << "ERROR: Failed to write pulse count.\n";
        close(fd);
        return 1;
    }

    cout << "Pulse count written: 10\n";


    char buffer[64] = {0};

    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);

    if (bytesRead < 0) {
        cerr << "ERROR: Failed to read pulse count.\n";
        close(fd);
        return 1;
    }

    buffer[bytesRead] = '\0';

    cout << "Pulse count read from driver: "
         << buffer;

    close(fd);

    cout << "Driver read/write test completed.\n";

    return 0;
}
