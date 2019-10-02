#include "utils.hpp"
#include <thread>

use namespace std;

class Worker {
private:
    int id;
    particle_set_t *particle_set;
    swarm_t *swarm;
    threads_coordinator_t *coordinator;
    string target_func;

public:
    Worker(int i, particle_set_t *ps, swarm_t *s, threads_coordinator_t *c, string f) {
        id = i;
        particle_set = ps;
        swarm = s;
        coordinator = c;
        target_func = f;
    }

    thread *run() {
        auto thread_body = [&] () {
            while(coordinator->epochs > 0) {
                if (coordinator->gate[id] == 1) {
                    for (int i = ps->start; i <= ps->end; i) {
                        update_velocity(&(swarm->particles[i]), swarm->global_min, target_func);
                        update_local(&(swarm->particles[i]), target_func);
                    }
                    coordinator->work_done[id] = 0;
                }
            }
            return;
        };
        return new thread(body);
    }
};


class Coordinator {
private:
    int n_threads;
    string target_func;
    swarm_t swarm;
    threads_coordinator_t *coordinator;

public:
    Coordinator(int nt, swarm_t *s, threads_coordinator_t *c, string tf) {
        n_threads = nt;
        swarm = s;
        coordinator = c;
        target_func = tf;
    }

    thread *run() {
        auto thread_body = [&] () {
            while (coordinator->epochs > 0) {
                int c=0;
                for (int i=0; i<n_threads; i++) {
                    if (coordinator->work_done[i] == 1)
                        c++;
                }
                if (c == n_threads) {
                    for (int i=0; i<n_threads; i++) {
                        coordinator->work_done[i] = 0;
                    }
                    update_global(swarm, target_func);
                    for (int i=0; i<n_threads; i++) {
                        coordinator->gate[i] = 1;
                    }
                }
            }
        return;
        };
    return new thread(body);
    }
};