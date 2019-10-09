#include <iostream>
#include <thread>
#include <vector>
#include "threads.hpp"
#include "utimer.cpp"

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
	Queue<particle_set_t*> work_queue = new Queue<particle_set_t *>(n_threads);
	int work = 0;

	//initialize threads
	vector<thread*> threads;
	vector<Worker_farm*> workers;
	Server_farm *server = new Server_farm(swarm, epochs, target_func, work, n_threads, work_queue, n_particles);

	for (int i=0; i<n_threads; i++) {
		workers.push_back(new Worker_farm(i, work_queue, swarm, target_func, work));
	}

	{
		utimer u("thread_farm");

		//run threads
		for (auto w: workers) {
			threads.push_back(w->run());
		}
//
//		threads.push_back(server->run());
//
//		//join threads
		for (auto t: threads) {
			t->join();
		}
	//		print_swarm(swarm, target_func);
	}
	return 0;
}
