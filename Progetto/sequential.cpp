#include<iostream>
#include "utils.hpp"

using namespace std;

int main(int argc, char const *argv[]) {

    if (check_arg(argc, argv) == -1)
        return -1;
    
    string target_func = argv[1];
    string init_type = argv[2];
    int n_particles = atoi(argv[3]);
    int epochs = atoi(argv[4]);


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
