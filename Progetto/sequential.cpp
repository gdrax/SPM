#include<iostream>
#include "utils.hpp"

using namespace std;

int main(int argc, char const *argv[]) {
    if (argc < 5) {
        cout << "USAGE: " << argv[0] << " [function_name] [init_type] [n. of particles] [n. iterations]\n";
        return -1;
    }

    string target_func = argv[1];
    if (!check_bench_fun(target_func)) {
        cout << "ERROR: function_name is not valid. The available functions are: \"sphere\", \"himmel\"\n";
        return -1;
    }

    string init_type = argv[2];
    if (!check_init_type(init_type)) {
        cout << "ERROR: init_type is not valid. The available initialization are: \"uniform\", \"random\"\n";
        return -1;
    }

    int n_particles = atoi(argv[3]);
    if (n_particles <= 0) {
        cout << "ERROR: n. of particles must be positive\n";
        return -1;
    }

    //in realtà must be quadrato perfetto sargio
    int epochs = atoi(argv[4]);
    if (epochs <= 0) {
        cout << "ERROR: n. of iterations must be positive\n";
    }

    swarm_t *swarm = init_swarm(n_particles, target_func, init_type);

    print_swarm(swarm, target_func);

    for (int i=0; i<epochs; i++) {
        cout << "Iteration" << i << ": global_min = " << compute_bench_fun(swarm->global_min, target_func) << "\n";
        for (int j=0; j<swarm->n_particles; j++) {
            update_velocity(&(swarm->particles[j]), swarm->global_min, target_func);
            update_local(&(swarm->particles[j]), target_func);
        }
        update_global(swarm, target_func);
    }

    print_swarm(swarm, target_func);
    return 0;
}
