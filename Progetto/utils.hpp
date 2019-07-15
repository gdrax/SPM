#include <cmath>
#include <string>

using namespace std;

/**
 * compute a benchmark function
 */
float benchmark_function(float x, float y, string func) {
    if (func == "sphere") {
        return pow(x, 2) + pow(y, 2);
    }
    if (func == "himmel") {
        return pow(pow(x, 2) + y + 11, 2) + pow(x + pow(y, 2) + 7, 2);
    }
}