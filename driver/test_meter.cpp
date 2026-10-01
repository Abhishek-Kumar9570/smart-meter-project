#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
    const char* device = "/dev/virtual_meter";

    // Write a meter value
    {
        ofstream meter(device);
        if (!meter) {
            cerr << "Error: Cannot open device for writing.\n";
            return 1;
        }

        meter << "50";
        meter.close();

        cout << "Written value: 50" << endl;
    }

    // Read the meter value
    {
        ifstream meter(device);
        if (!meter) {
            cerr << "Error: Cannot open device for reading.\n";
            return 1;
        }

        string value;
        getline(meter, value);
        meter.close();

        cout << "Read value: " << value << endl;
    }

    return 0;
}
