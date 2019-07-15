#include<iostream>
#include "utils.hpp"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc < 3) {
        cout << "USAGE: " << argv[0] << " [function_name] [n. of particles]\n";
        return -1;
    }

    string target_func = argv[1];
    if (!check_bench_fun(target_func)) {
        cout << "ERROR: function_name is not valid. The available functions are: \"sphere\", \"himmel\"\n";
        return -1;
    }

    int n_particles = atoi(argv[2]);
    if (n_particles <= 0) {
        cout << "ERROR: n. of particles must be positive\n";
        return -1;
    }

    float domain_max = bench_fun_bound(target_func);
    float domain_min = -domain_max;

    

    return 0;
}
