#include "utils.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class Worker_barrier {
private:
	int id;
	swarm_t *swarm;
	string target_func;
	pthread_barrier_t *barrier;
	int n_threads;
	int n_particles;
	int epochs;
	particle_set_t *particle_set;
	int *work;

public:
	Worker_barrier(int i, swarm_t *s, string tf, pthread_barrier_t *b, int nt , int np, int e):
			id(i), swarm(s), target_func(tf), barrier(b), n_threads(nt), n_particles(np), epochs(e) {}

	thread *run() {
		auto body = [&] () {
			particle_set = get_particles_set(n_threads, n_particles, id);
			for (int j=1; j<epochs; j++) {
				position_t partial_min = swarm->particles[particle_set->start].position;
				for (int i = particle_set->start; i <= particle_set->end; i++) {
					update_velocity(&(swarm->particles[i]), swarm->global_min, target_func);
					update_position(&(swarm->particles[i]), target_func);
					float func_value = compute_bench_fun(swarm->particles[i].position, target_func);
					//update local minimum
					if (compute_bench_fun(swarm->particles[i].local_min, target_func) > func_value) {
						swarm->particles[i].local_min.x = swarm->particles[i].position.x;
						swarm->particles[i].local_min.y = swarm->particles[i].position.y;
					}
					//update global minimum with lock
					if (compute_bench_fun(swarm->particles[i].local_min, target_func) < compute_bench_fun(partial_min, target_func)) {
						partial_min = swarm->particles[i].local_min;
					}
				};
				pthread_barrier_wait(barrier);
				unique_lock <mutex> lock(global_min_mutex);
				update_single_global(swarm, partial_min);
			}
			return;
		};
		return new thread(body);
	}
};