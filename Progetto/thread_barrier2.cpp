#include <iostream>
#include <vector>
#include <pthread.h>
#include "threads.hpp"

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

	swarm_t *swarm = init_swarm(n_particles, target_func, init_type);

	//initialize threads
	vector<thread*> threads;
	vector<Worker_barrier2*> workers;

	pthread_barrier_t work_barrier;
	pthread_barrier_init(&work_barrier, NULL, n_threads+1);

	int work=0;

	for (int i=0; i<n_threads; i++) {
		workers.push_back(new Worker_barrier2(i, swarm, target_func, &work_barrier, n_threads, n_particles, epochs, &work));
	}

	Master *master = new Master(n_threads, swarm, target_func, epochs, &work_barrier, &work);

	{
		utimer u("thread_barrier2");

		//run threads
		for (auto w: workers) {
			threads.push_back(w->run());
		}

		threads.push_back(master->run());

		//join threads
		for (auto t: threads) {
			t->join();
		}

//        print_swarm(swarm, target_func);
		print_global_min(swarm, target_func);
	}
	pthread_barrier_destroy(&work_barrier);
	return 0;
}