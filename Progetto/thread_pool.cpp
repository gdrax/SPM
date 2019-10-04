#include <iostream>
#include <thread>
#include <vector>
#include "threads.hpp"
#include "utimer.cpp"

int main(int argc, char const *argv[]) {

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
    vector<Worker_pool*> workers;
    Coordinator *coordinator = new Coordinator(n_threads, epochs, swarm, target_func);

    for (int i=0; i<n_threads; i++) {
        particle_set_t *particle_set_i = get_particles_set(n_threads, n_particles, i);
//        cout << particle_set_i->start << "    " << particle_set_i->end << endl;
        workers.push_back(new Worker_pool(i, particle_set_i, swarm, coordinator, target_func));
    }

    {
        utimer u("thread_pool");

        //run threads
        for (auto w: workers) {
            threads.push_back(w->run());
        }

        //join threads
        for (auto t: threads) {
            t->join();
        }
        print_swarm(swarm, target_func);
    }
    return 0;
}
