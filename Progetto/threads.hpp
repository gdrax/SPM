#include "utils.hpp"
#include "queue.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class Coordinator {
private:
    int n_work;
    int n_threads;
    int epochs;
    swarm_t *swarm;
    condition_variable work_done;
    condition_variable new_work;
    mutex w_mutex;
    mutex c_mutex;
    string target_func;

public:
    Coordinator(int nt, int e, swarm_t *s, string tf) {
        this->n_work = 0;
        this->epochs = e;
        this->n_threads = nt;
        this->swarm = s;
        this->target_func = tf;
    }

    void add_work() {
        unique_lock<mutex> lock(this->w_mutex);
        this->n_work++;
//        cout << this->n_work << endl;
        if (this->n_work == this->n_threads) {
            this->epochs--;
            this->n_work = 0;
            update_global(this->swarm, this->target_func);
//            print_swarm(swarm, target_func);
            this->new_work.notify_all();
        }
        else {
            this->new_work.wait(lock);
        }
    }

    int getEpochs() {
        return this->epochs;
    }
};

class Worker_pool {
private:
    int id;
    particle_set_t *particle_set;
    swarm_t *swarm;
    Coordinator *coordinator;
    string target_func;

public:
    Worker_pool(int i, particle_set_t *ps, swarm_t *s, Coordinator *c, string f) {
        this->id = i;
        this->particle_set = ps;
        this->swarm = s;
        this->coordinator = c;
        this->target_func = f;
    }

    thread *run() {
        auto body = [&] () {
            while(this->coordinator->getEpochs() > 0) {
                for (int i = this->particle_set->start; i <= this->particle_set->end; i++) {
                    update_velocity(&(this->swarm->particles[i]), this->swarm->global_min, this->target_func);
                    update_local(&(this->swarm->particles[i]), this->target_func);
                }
            //cout << "Worker " << this->id << ": work done." << endl;
            this->coordinator->add_work();
            }
            return;
        };
        return new thread(body);
    }
};

class Worker_farm {
private:
	g_queue<particle_set_t*>& input_queue;
	int id;
	swarm_t *swarm;
	string target_func;
	int work;

public:
	Worker_farm(int i, g_queue<particle_set_t*>& q, swarm_t *s, string tf, int w) {
		this->input_queue = q;
		this->id = i;
		this->swarm = s;
		this->target_func = tf;
		this->work = w;
	}

	thread *run() {
		auto body = [&] () {
			particle_set_t *particle_set = this->input_queue.pop();
			while(particle_set->start != -1 && particle_set->end != -1) {
				for (int i = particle_set->start; i <= particle_set->end; i++) {
					update_velocity(&(this->swarm->particles[i]), this->swarm->global_min, this->target_func);
					update_local(&(this->swarm->particles[i]), this->target_func);
				}
				work++;
			}
			return;
		};
		return new thread(body);
	}
};

class Server_farm {
private:
	swarm_t *swarm;
	int epochs;
	string target_func;
	int work;
	int n_threads;
	g_queue<particle_set_t*> work_queue;
	int n_particles;

public:
	Server_farm(swarm_t *s, int e, string tf, int w, int nt, g_queue<particle_set_t*> wq, int np) {
		this->swarm = s;
		this->epochs = e;
		this->target_func = tf;
		this->work = w;
		this->n_threads = nt;
		this->work_queue = wq;
		this->n_particles = np;
	}

	thread *run() {
		auto body = [&] () {
			while (this->epochs > 0) {
				if (this->work == 0) {
					for (int i = 0; i < this->n_threads; i++) {
						particle_set_t *particle_set_i = get_particles_set(this->n_threads, this->n_particles, i);
						this->work_queue.push(particle_set_i);
					}
				}
				if (this->work == this->n_threads) {
					update_global(this->swarm, this->target_func);
					this->work = 0;
					this->epochs--;
				}
			}
			particle_set_t *end_set = new particle_set_t;
			end_set->start = -1;
			end_set->end = -1;
			for (int i=0; i<this->n_threads; i++) {
				this->work_queue.push(end_set);
			}
			return;
		};
		return new thread(body);
	}

};