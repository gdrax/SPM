#include "utils.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

//pthread_barrier_t barrier;

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

public:
    Worker_barrier(int i, swarm_t *s, string tf, pthread_barrier_t *b, int nt , int np, int e):
            id(i), swarm(s), target_func(tf), barrier(b), epochs(e) {}

    thread *run() {
        auto body = [&] () {
            particle_set = get_particles_set(n_threads, n_particles, id);

            pthread_barrier_wait(barrier);
            for (int j=0; j<epochs; j++) {
				for (int i = particle_set->start; i <= particle_set->end; i++) {
					update_velocity(&(swarm->particles[i]), swarm->global_min, target_func);
				}
				for (int i = particle_set->start; i <= particle_set->end; i++) {
					update_position(&(swarm->particles[i]), target_func);
					float func_value = compute_bench_fun(swarm->particles[i].position, target_func);
					//update local minimum
					if (compute_bench_fun(swarm->particles->local_min, target_func) > func_value) {
						swarm->particles->local_min.x = swarm->particles[i].position.x;
						swarm->particles->local_min.y = swarm->particles[i].position.y;
					}
					//update global minimum with lock
					if (compute_bench_fun(swarm->particles[i].local_min, target_func) < compute_bench_fun(swarm->global_min, target_func)) {
						unique_lock <mutex> lock(global_min_mutex);
						update_single_global(swarm, target_func, i);
					}
				}
                pthread_barrier_wait(barrier);
            }
            return;
        };
        return new thread(body);
    }
};


class Barrier {
private:
    int work;
    mutex master_mutex;
    mutex worker_mutex;
    condition_variable work_done;
    condition_variable barrier;
    int n_threads;

public:
    Barrier(int nt):
    n_threads(nt), work(0) {}

    void wait() {
        unique_lock<mutex> lock(worker_mutex);
        work++;
        if (work == n_threads) {
            work_done.notify_one();
            barrier.wait(lock);
            work_done.notify_all();
        }
        else
            barrier.wait(lock);
    }

    void compute_optimum(swarm_t *swarm, string target_func) {
        unique_lock<mutex> lock(master_mutex);
        if (work == n_threads) {
            update_global(swarm, target_func);
            work = 0;
            barrier.notify_all();
            cout << work << "master suspend\n";
        }
        else {
            work_done.wait(lock);
        }
    }

    void free_workers() {
        barrier.notify_all();
    }
};

class Worker_barrier_new {
private:
    int id;
    swarm_t *swarm;
    string target_func;
    int n_threads;
    int epochs;
    Barrier *barrier;
    particle_set_t *particle_set;
    int n_particles;

public:
    Worker_barrier_new(int i, swarm_t *s, string tf, int e, int nt, Barrier *b, int np):
            id(i), swarm(s), target_func(tf), epochs(e), n_threads(nt), barrier(b), n_particles(np) {}

    thread *run() {
        auto body = [&] () {
            particle_set = get_particles_set(n_threads, n_particles, id);
            cout << "1st barrier\n";
            barrier->wait();
            for (int j=0; j<epochs; j++) {
                for (int i = particle_set->start; i <= particle_set->end; i++) {
                    update_velocity(&(swarm->particles[i]), swarm->global_min, target_func);
                }
                for (int i = particle_set->start; i <= particle_set->end; i++) {
                    update_position(&(swarm->particles[i]), target_func);
                    float func_value = compute_bench_fun(swarm->particles[i].position, target_func);
                    //update local minimum
                    if (compute_bench_fun(swarm->particles->local_min, target_func) > func_value) {
                        swarm->particles->local_min.x = swarm->particles[i].position.x;
                        swarm->particles->local_min.y = swarm->particles[i].position.y;
                    }
                    //update global minimum with lock
//                    if (compute_bench_fun(swarm->particles[i].local_min, target_func) < compute_bench_fun(swarm->global_min, target_func)) {
//                        unique_lock <mutex> lock(global_min_mutex);
//                        update_single_global(swarm, target_func, i);
//                    }
                }
                cout << "2nd barrier\n";
                barrier->wait();
            }
            return;
        };
        return new thread(body);
    }
};

class Master {
private:
	int *work;
	int n_threads;
	swarm_t *swarm;
	string target_func;
	int epochs;
	pthread_barrier_t *barrier2;

public:
	Master(int nt, swarm_t *s, string tf, int e, pthread_barrier_t *b, int *w):
		n_threads(nt), swarm(s), target_func(tf), epochs(e), barrier2(b), work(w){}

	thread *run() {
		auto body = [&] () {
			for (int i=0; i<epochs; i++) {
				//cout << "master iteration\n";
				while(*work != n_threads) {/*cout << *work << endl;*/}
				*work = 0;
				update_global(swarm, target_func);
				pthread_barrier_wait(barrier2);
//				pthread_barrier_init(barrier2, NULL, n_threads+1);
//				cout << i << endl;
			}
//			cout << "master ends\n";
			return;
		};
		return new thread(body);
	}
};

class Worker_barrier2 {
private:
	int id;
	swarm_t *swarm;
	string target_func;
	pthread_barrier_t *barrier2;
	int n_threads;
	int n_particles;
	int epochs;
	particle_set_t *particle_set;
	int *work;

public:
	Worker_barrier2(int i, swarm_t *s, string tf, pthread_barrier_t *b, int nt , int np, int e, int *w):
			id(i), swarm(s), target_func(tf), barrier2(b), n_threads(nt), n_particles(np), epochs(e), work(w) {}

	thread *run() {
		auto body = [&] () {
			particle_set = get_particles_set(n_threads, n_particles, id);
			*work = *work + 1;
//			cout << *work << "first\n";
			pthread_barrier_wait(barrier2);
			for (int j=1; j<epochs; j++) {
				for (int i = particle_set->start; i <= particle_set->end; i++) {
					update_velocity(&(swarm->particles[i]), swarm->global_min, target_func);
				}
				for (int i = particle_set->start; i <= particle_set->end; i++) {
					update_position(&(swarm->particles[i]), target_func);
					float func_value = compute_bench_fun(swarm->particles[i].position, target_func);
					//update local minimum
					if (compute_bench_fun(swarm->particles->local_min, target_func) > func_value) {
						swarm->particles->local_min.x = swarm->particles[i].position.x;
						swarm->particles->local_min.y = swarm->particles[i].position.y;
					}
					//update global minimum with lock
					if (compute_bench_fun(swarm->particles[i].local_min, target_func) < compute_bench_fun(swarm->global_min, target_func)) {
						unique_lock <mutex> lock(global_min_mutex);
						update_single_global(swarm, target_func, i);
					}
				}
				*work = *work +1;
//				cout << *work << "second\n";
				pthread_barrier_wait(barrier2);
			}
			return;
		};
		return new thread(body);
	}
};