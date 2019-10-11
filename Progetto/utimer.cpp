#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>

using namespace std;

class utimer {
    chrono::system_clock::time_point start;
    chrono::system_clock::time_point stop;
    string message;
    using usecs = chrono::microseconds;
    using msecs = chrono::milliseconds;

public:
    utimer(const string m) : message(m) {
        start = chrono::system_clock::now();
    }

    ~utimer() {
        stop = chrono::system_clock::now();
        chrono::duration<double> elapsed = stop - start;
        auto musec = chrono::duration_cast<chrono::microseconds>(elapsed).count();
        cout << message << ": " << musec << endl;
    }
};
