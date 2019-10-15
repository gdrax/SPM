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

	vector<thread> threads;
	pthread_barrier_init(&barrier, NULL, n_threads);

	{
		utimer u("thread_barrier");

		for (int i=0; i<n_threads; i++) {
			particle_set_t *particle_set_i = get_particles_set(n_threads, n_particles, i);
			threads.push_back(thread(compute_swarm, swarm, particle_set_i, epochs, target_func, i));
		}

		//join threads
		for (thread &t: threads) {
			t.join();
		}

//        print_swarm(swarm, target_func);
//		print_global_min(swarm, target_func);
	}
	pthread_barrier_destroy(&barrier);
	return 0;
}

