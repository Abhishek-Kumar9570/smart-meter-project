#include <iostream>
#include <cmath>

using namespace std;

int main()
{
    const int expectedPulses = 1000;

    int countedPulses = 0;

    // Simulate receiving exactly 1000 valid pulses
    for (int i = 0; i < expectedPulses; i++)
    {
        countedPulses++;
    }

    double errorPercentage =
        (abs(countedPulses - expectedPulses) /
         (double)expectedPulses) *
        100.0;

    cout << "=====================================" << endl;
    cout << "     SMART METER PULSE ACCURACY TEST" << endl;
    cout << "=====================================" << endl;

    cout << "Expected pulses : "
         << expectedPulses << endl;

    cout << "Counted pulses  : "
         << countedPulses << endl;

    cout << "Error           : "
         << errorPercentage << "%" << endl;

    if (errorPercentage < 1.0)
    {
        cout << "Result          : PASS" << endl;
        cout << "Pulse counting error is below 1%."
             << endl;
    }
    else
    {
        cout << "Result          : FAIL" << endl;
        cout << "Pulse counting error is 1% or higher."
             << endl;
    }

    cout << "=====================================" << endl;

    return 0;
}