#include <iostream>
#include <cmath>

using namespace std;

int main()
{
    const int expectedPulses = 1000;

    // Debounce interval in milliseconds
    const int debounceTimeMs = 5;

    int acceptedPulses = 0;

    // Time of the last accepted pulse
    int lastAcceptedTime = -debounceTimeMs;

    // Simulate 1000 valid pulses.
    // After every valid pulse, an unwanted noise pulse
    // occurs only 1 ms later.
    for (int i = 0; i < expectedPulses; i++)
    {
        // Valid pulse arrives every 10 ms
        int validPulseTime = i * 10;

        // Accept valid pulse if debounce interval has passed
        if (validPulseTime - lastAcceptedTime >= debounceTimeMs)
        {
            acceptedPulses++;
            lastAcceptedTime = validPulseTime;
        }

        // Unwanted/noise pulse arrives 1 ms later
        int noisePulseTime = validPulseTime + 1;

        // This should be rejected by debounce
        if (noisePulseTime - lastAcceptedTime >= debounceTimeMs)
        {
            acceptedPulses++;
            lastAcceptedTime = noisePulseTime;
        }
    }

    double errorPercentage =
        (abs(acceptedPulses - expectedPulses) /
         (double)expectedPulses) *
        100.0;

    cout << "========================================" << endl;
    cout << "   PULSE ACCURACY + DEBOUNCE TEST" << endl;
    cout << "========================================" << endl;

    cout << "Expected valid pulses : "
         << expectedPulses << endl;

    cout << "Accepted pulses       : "
         << acceptedPulses << endl;

    cout << "Error                 : "
         << errorPercentage << "%" << endl;

    cout << "Debounce interval     : "
         << debounceTimeMs << " ms" << endl;

    if (errorPercentage < 1.0 &&
        acceptedPulses == expectedPulses)
    {
        cout << "Result                : PASS" << endl;
        cout << "Noise pulses were rejected successfully."
             << endl;
    }
    else
    {
        cout << "Result                : FAIL" << endl;
        cout << "Debounce test failed."
             << endl;
    }

    cout << "========================================" << endl;

    return 0;
}