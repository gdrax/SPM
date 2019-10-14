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
		compute_swarm_sequential(swarm, epochs, target_func);
        print_global_min(swarm, target_func);
	}
	return 0;
}