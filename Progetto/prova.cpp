#include <thread>
#include <vector>
#include <pthread.h>
#include <mutex>
#include "utils.hpp"

mutex global_min_mutex;
pthread_barrier_t barrier;

using namespace std;

void compute_swarm(swarm_t *swarm, particle_set_t *particle_set, int epochs, string target_func, int id) {
    for (int j=0; j<epochs; j++) {
        {
            utimer u("Worker" + id);
            for (int i = particle_set->start; i <= particle_set->end; i++) {
                update_velocity(&(swarm->particles[i]), swarm->global_min, target_func);
            }

            for (int i = particle_set->start; i <= particle_set->end; i++) {
                update_position(&(swarm->particles[i]));
                float func_value = compute_bench_fun(swarm->particles[i].position, target_func);
                if (compute_bench_fun(swarm->particles->local_min, target_func) > func_value) {
                    swarm->particles->local_min.x = swarm->particles[i].position.x;
                    swarm->particles->local_min.y = swarm->particles[i].position.y;
                }
                if (compute_bench_fun(swarm->particles[i].local_min, target_func) < compute_bench_fun(swarm->global_min, target_func)) {
                    unique_lock <mutex> lock(global_min_mutex);
                    update_single_global(swarm, target_func, i);
                }
            }
        }
        pthread_barrier_wait(&barrier);
    }
    return;
}


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
    vector<thread> threads;

    pthread_barrier_init(&barrier, NULL, n_threads);

    {
        utimer u("thread_barrier");

        for (int i=0; i<n_threads; i++) {
            particle_set_t *particle_set_i = get_particles_set(n_threads, n_particles, i);
//        cout << particle_set_i->start << "    " << particle_set_i->end << endl;
            threads.push_back(thread(compute_swarm, swarm, particle_set_i, epochs, target_func, i));
        }

        //join threads
        for (thread &t: threads) {
            t.join();
        }

//        print_swarm(swarm, target_func);
        print_global_min(swarm, target_func);
    }
    pthread_barrier_destroy(&barrier);
    return 0;
}

