#include<iostream>
#include <unistd.h>
#include <cstdlib>
#include "utils.hpp"

using namespace std;

int print = 1;

int main(int argc, char *argv[]) {

	if (check_arg(argc, argv) == -1)
		return -1;

	int c;
	while ((c = getopt(argc, argv, "s")) != -1) {
	    switch(c) {
            case 's':
                print = 0;
                break;
	    }
	}

	string target_func = argv[1];
	string init_type = argv[2];
	int n_particles = atoi(argv[3]);
	int epochs = atoi(argv[4]);


	swarm_t *swarm = init_swarm(n_particles, target_func, init_type);

	{
		utimer u("sequential");

		for (int i = 0; i < epochs; i++) {
//            cout << "Iteration" << i << ": global_min = " << compute_bench_fun(swarm->global_min, target_func) << "\n";
			for (int j = 0; j < swarm->n_particles; j++) {
				update_local(&(swarm->particles[j]), target_func);
				if (compute_bench_fun(swarm->particles[j].local_min, target_func) < compute_bench_fun(swarm->global_min, target_func))
				    update_single_global(swarm, target_func, j);
                update_velocity(&(swarm->particles[j]), swarm->global_min, target_func);
            }
		}

//		if (print)
//		    print_swarm(swarm, target_func);
        print_global_min(swarm, target_func);
	}
	return 0;
}