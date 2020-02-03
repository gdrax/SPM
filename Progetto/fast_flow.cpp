#include <thread>
#include <vector>
#include <pthread.h>
#include <mutex>
#include "utils.hpp"

using namespace std;

int main(int argc, char *argv[]) {

    if (check_arg(argc, argv) == -1)
        return -1;
    if (argc < 6) {
        cout << "USAGE: " << argv[0] << " [function_name] [init_type] [n. of particles] [n. iterations] [n. threads]\n";
        return -1;
    }
    if (atoi(argv[5]) < 1) {
        cout << "Number of threads must be positive\n";
        return -1;
    }

    string target_func = argv[1];
    string init_type = argv[2];
    int n_particles = atoi(argv[3]);
    int epochs = atoi(argv[4]);
    int n_threads = atoi(argv[5]);

    //initialize swarm
    swarm_t *swarm = init_swarm(n_particles, target_func, init_type);

    {
        utimer u("thread_barrier");

		compute_swarm_fast_flow(swarm, epochs, target_func, n_threads, n_particles);

//      print_swarm(swarm, target_func);
		print_global_min(swarm, target_func);
    }
    return 0;
}