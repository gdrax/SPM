#include "utils.hpp"
#include "queue.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex global_min_mutex;

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

class Worker_barrier {
private:
    int id;
    swarm_t *swarm;
    string target_func;
    pthread_barrier_t *barrier;
    particle_set_t *particle_set;
    int epochs;

public:
    Worker_barrier(int i, swarm_t *s, string tf, pthread_barrier_t *b, particle_set_t *ps, int e):
            id(i), swarm(s), target_func(tf), barrier(b) , particle_set(ps), epochs(e) {}

    thread *run() {
        auto body = [&] () {
//            cout << this->particle_set->start << "-" << this->particle_set->end << endl;
            for (int j=0; j<epochs; j++) {
                {
                    utimer u("Worker" + id);
                    for (int i = this->particle_set->start; i <= this->particle_set->end; i++) {
                        update_local(&(this->swarm->particles[i]), this->target_func);
                        if (compute_bench_fun(this->swarm->particles[i].local_min, this->target_func) < compute_bench_fun(this->swarm->global_min, this->target_func)) {
                            unique_lock <mutex> lock(global_min_mutex);
                            update_single_global(this->swarm, this->target_func, i);
                        }
                    }
                    for (int i = this->particle_set->start; i <= this->particle_set->end; i++) {
                        update_velocity(&(this->swarm->particles[i]), this->swarm->global_min, this->target_func);
                    }
                }
                pthread_barrier_wait(this->barrier);
            }
            return;
        };
        return new thread(body);
    }
};