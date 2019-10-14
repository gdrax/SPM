#include <thread>
#include <vector>
#include <pthread.h>
#include <mutex>
#include "utils.hpp"

using namespace std;

void compute_swarm(int epochs) {
	for (int i=0; i<epochs; i++)
	float a = random01();
	return;
}


int main(int argc, char *argv[]) {
	int epochs = atoi(argv[1]);
	int n_threads = atoi(argv[2]);

	//initialize threads
	vector<thread> threads;

	{
		utimer u("random_numbers");

		for (int i=0; i<n_threads; i++) {
			threads.push_back(thread(compute_swarm, epochs));
		}

		//join threads
		for (thread &t: threads) {
			t.join();
		}

	}
	return 0;
}

