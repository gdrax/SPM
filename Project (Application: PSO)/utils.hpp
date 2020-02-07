#include <iostream>
#include <functional>
#include <random>
#include <mutex>
#include <ff/ff.hpp>
#include <ff/parallel_for.hpp>
#include "utimer.cpp"

using namespace std;

unsigned int seed = 6;

mutex global_min_mutex;
pthread_barrier_t barrier;

int work=0;
int hits = 0;

/**
 * Structure of a position in the search space
 */
typedef struct {
    float x;
    float y;
} position_t;

/**
 * Structure of a particle of the swarm
 */
typedef struct {
    position_t position; //current position of the particle in the search space
    position_t local_min; //best position found by the particle so far
    float vx; //x component of the velocity
    float vy; //y component of the velocity
} particle_t;

/**
 * Indexes of the particles assigned to a thread
 */
typedef struct {
    int start;
    int end;
} particle_set_t;

/**
 * Structure of a swarm of particles
 */
typedef struct {
    particle_t *particles; //particles data
    position_t global_min; //best position found by the swarm so far
    int n_particles; //number of particles in the swarm
} swarm_t;

/* Domain bounds for the benchmark functions */
float const sphere_domain_bound = 100;
float const himmel_domain_bound = 5;
float const sum_domain_bound = 100;

/* Default parameters for the computation of the velocity */
float const cognitive_parameter = 2;
float const social_parameter = 2;
float const inertia_weight = 0.9;
float const velocity_clamp = 0.5;

/**
 * Thread safe random number generator in the interval [0, 1)
 */
float random01() {
	static thread_local mt19937 generator(seed);
	uniform_real_distribution<float> distribution(0.0, 1.0);
	return distribution(generator);
}

/**
 * Generates a random number in the interval [a, b)
 */
float randAB(float a, float b) {
    return a + random01() * (b-a);
}

/**
 * compute a benchmark function
 */
float compute_bench_fun(position_t p, string func) {
    if (func == "sphere") {
        return pow(p.x, 2) + pow(p.y, 2);
    }
    if (func == "himmel") {
        return pow(pow(p.x, 2) + p.y + 11, 2) + pow(p.x + pow(p.y, 2) + 7, 2);
    }
//    if (func == "matyas") {
//        return 0.26 * (pow(p.x, 2) + pow(p.y, 2)) - 0.48 * p.x * p.y;
//    }
//	if (func == "sum") {
//		return p.x+p.y;
//	}
    return 0;
}

/**
 * check if the bechmark function name is valid
 */
bool check_bench_fun(string func) {
    if (func == "sphere" || func == "himmel" || func == "sum")
        return true;
    else
        return false;
}

/**
 * check if the init type is valid
 */
bool check_init_type(string init) {
    if (init == "uniform" || init == "random")
        return true;
    else
        return false;    
}

/**
 * returns the domain bound of a benchmark function
**/
float bench_fun_bound(string func) {
    if (func == "sphere")
        return sphere_domain_bound;
    if (func == "himmel")
        return himmel_domain_bound;
//    if (func == "matyas")
//        return matyas_domain_bound;
//	if (func == "sum")
//		return sum_domain_bound;
    return 0;
}

/**
 * Compute the new X component of the velocity of a particle
**/
void compute_velocity_X(particle_t *particle, position_t global_min, string func) {
    float local_diff = particle->local_min.x - particle->position.x;
    float global_diff = global_min.x - particle->position.x;
    particle->vx = inertia_weight*particle->vx + cognitive_parameter*random01()*local_diff + social_parameter*random01()*global_diff;
    if (particle->vx > velocity_clamp*bench_fun_bound(func))
        particle->vx = velocity_clamp*bench_fun_bound(func);
}

/**
 * Compute the new Y component of the velocity of a particle
**/
void compute_velocity_Y(particle_t *particle, position_t global_min, string func) {
    float local_diff = particle->local_min.y - particle->position.y;
    float global_diff = global_min.y - particle->position.y;
    particle->vy =  inertia_weight*particle->vy + cognitive_parameter*random01()*local_diff + social_parameter*random01()*global_diff;
    if (particle->vy > velocity_clamp*bench_fun_bound(func))
        particle->vy = velocity_clamp*bench_fun_bound(func);
}

/**
 * Update the velocity of a particle
 */
void update_velocity(particle_t *particle, position_t global_min, string func) {
    compute_velocity_X(particle, global_min, func);
    compute_velocity_Y(particle, global_min, func);
}

/**
 * Update the position of a particle
 */
void update_position(particle_t *particle, string func) {
    particle->position.x += particle->vx;
    particle->position.y += particle->vy;
    if (particle->position.x > bench_fun_bound(func))
        particle->position.x = bench_fun_bound(func);
    if (particle->position.y > bench_fun_bound(func))
        particle->position.y = bench_fun_bound(func);
	if (particle->position.x < -bench_fun_bound(func))
		particle->position.x = -bench_fun_bound(func);
	if (particle->position.y < -bench_fun_bound(func))
		particle->position.y = -bench_fun_bound(func);
}

/**
 * Update the position and the local minimun of a particle
 */
void update_local(particle_t *particle, string func) {
    update_position(particle, func);
    if (compute_bench_fun(particle->position, func) <  compute_bench_fun(particle->local_min, func)) {
        particle->local_min.x = particle->position.x;
        particle->local_min.y = particle->position.y;
    }
}

/**
 * Update the position of the global minimum
 */
void update_global(swarm_t *swarm, string func) {
    for (int i=0; i<swarm->n_particles; i++) {
        if (compute_bench_fun(swarm->particles[i].position, func) < compute_bench_fun(swarm->global_min, func)) {
            swarm->global_min.x = swarm->particles[i].position.x;
            swarm->global_min.y = swarm->particles[i].position.y;
        }
    }
}

/**
 * Update the position of the global minimum
 */
void update_single_global(swarm_t *swarm, position_t position) {
    swarm->global_min.x = position.x;
    swarm->global_min.y = position.y;
}

/**
 * Computes the initial position of the particles in the search space
 */
swarm_t *init_swarm(int n_particles, string func, string init_type) {
    swarm_t *swarm = new swarm_t;
    float domain_max = bench_fun_bound(func);
    float domain_min = -domain_max;
    swarm->particles = new particle_t[n_particles];
    swarm->n_particles = n_particles;
    for (int i=0; i<(int)sqrt(n_particles); i++) {
        for (int j=0; j<(int)sqrt(n_particles); j++) {
            int index = i*(int)sqrt(n_particles)+j;
            //set the initial position
            if (init_type == "uniform") {
                swarm->particles[index].position.x = domain_min + i*2*domain_max/(float)sqrt(n_particles);
                swarm->particles[index].position.y = domain_min + j*2*domain_max/(float)sqrt(n_particles);
            }
            else if (init_type == "random") {
                swarm->particles[index].position.x = randAB(domain_min, domain_max);
                swarm->particles[index].position.y = randAB(domain_min, domain_max);
            }
            swarm->particles[index].vx = 0;
            swarm->particles[index].vy = 0;
            //init local minima
            swarm->particles[index].local_min = swarm->particles[index].position;
            //compute global minima
            if (i==0 && j==0)
                swarm->global_min = swarm->particles[index].local_min;
            else
                if (compute_bench_fun(swarm->particles[index].local_min, func) < compute_bench_fun(swarm->global_min, func)) {
                    swarm->global_min = swarm->particles[index].position;
                }
        }
    }
    return swarm;
}

/**
 * Print the information of the swarm   
 */
void print_swarm(swarm_t *swarm, string func) {
    for (int i=0; i<swarm->n_particles; i++) {
        cout << "Particle n." << i << ": ";
        cout << "x=" << swarm->particles[i].position.x;
        cout << " y=" << swarm->particles[i].position.y;
        cout << " local_min=" << compute_bench_fun(swarm->particles[i].local_min, func) << endl;
    }
    cout << "global min x: " << swarm->global_min.x << endl;
    cout << "global min y: " << swarm->global_min.y << endl;
    cout << "global min:" << compute_bench_fun(swarm->global_min, func) << endl;
}

/**
 * Print the global min
 */
void print_global_min(swarm_t *swarm, string func) {
    cout << "Global min: " << compute_bench_fun(swarm->global_min, func) << endl;
}

/**
 * Check the arguments
**/
int check_arg(int argc, char *argv[]) {
    if (argc < 5) {
        cout << "USAGE: " << argv[0] << " [function_name] [init_type] [n. of particles] [n. iterations]\n";
        return -1;
    }
    if (!check_bench_fun(argv[1])) {
        cout << "ERROR: function_name is not valid. The available functions are: \"sphere\", \"himmel\"\n";
        return -1;
    }
    if (!check_init_type(argv[2])) {
        cout << "ERROR: init_type is not valid. The available initialization are: \"uniform\", \"random\"\n";
        return -1;
    }
    int s = sqrt(atoi(argv[3]));
    if (argv[3] <= 0 || floor(s) - s != 0) {
        cout << "ERROR: n. of particles must be positive\n";
        return -1;
    }
    if (argv[4] <= 0) {
        cout << "ERROR: n. of iterations must be positive\n";
        return -1;
    }
    return 0;
}

/**
 * Compute the particle's indexes of the i-th thread of the thread pool
 * @param n_threads: number of threads in the thread pool
 * @param n_particles: number of particles to be splitted
 * @param thread_index: position of the thread
 * @return
 */
particle_set_t *get_particles_set(int n_threads, int n_particles, int thread_index) {
    particle_set_t *particle_set = new particle_set_t;
    int block_size = floor(n_particles / n_threads);
    int resto = n_particles%n_threads;
    int start = thread_index * block_size;
    int end = (thread_index + 1) * block_size -1;
    if (resto > 0) {
        start += min(thread_index, resto);
        end += min(thread_index, resto);
        if (thread_index < resto)
            end++;
    }
    particle_set->start = start;
    particle_set->end = end;
    return particle_set;
}

/**
 * Computes the iteration phase of the PSO
 * @param particle_set: set of particles of the swarm assigned to the thread
 * @param epochs: number of iterations to be computed
 * @param target_func: function to be optimized (sphere or himmel)
 * @param id: id of the thread
 */
void compute_swarm(swarm_t *swarm, int epochs, string target_func, int id, int n_particles, int n_threads) {
    particle_set_t *particle_set = get_particles_set(n_threads, n_particles, id);
    pthread_barrier_wait(&barrier);
    for (int j=0; j<epochs; j++) {
		//update velocities
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
				update_single_global(swarm, swarm->particles[i].local_min);
			}
		}
		pthread_barrier_wait(&barrier);
	}
	return;
}


/**
 * Computed the iteration phase of the PSO sequentially
 * @param swarm: swarm to update
 * @param epochs: number of iterations
 * @param target_func: function to minimize
 * @param n_particles: number of particle in the swarm
 */
void compute_swarm_sequential(swarm_t *swarm, int epochs, string target_func) {
	for (int j=0; j<epochs; j++) {
		for (int i=0; i<swarm->n_particles; i++) {
			update_velocity(&(swarm->particles[i]), swarm->global_min, target_func);
		}
		for (int i=0; i<swarm->n_particles; i++) {
			update_position(&(swarm->particles[i]), target_func);
			float func_value = compute_bench_fun(swarm->particles[i].position, target_func);
			if (compute_bench_fun(swarm->particles->local_min, target_func) > func_value) {
				swarm->particles->local_min.x = swarm->particles[i].position.x;
				swarm->particles->local_min.y = swarm->particles[i].position.y;
			}
			if (compute_bench_fun(swarm->particles[i].local_min, target_func) < compute_bench_fun(swarm->global_min, target_func)) {
				update_single_global(swarm, swarm->particles[i].local_min);
			}
		}
	}
	return;
}

/**
 * Computes the iteration phase of the PSO
 * @param particle_set: set of particles of the swarm assigned to the thread
 * @param epochs: number of iterations to be computed
 * @param target_func: function to be optimized (sphere or himmel)
 * @param id: id of the thread
 */
void compute_swarm_multi_thread(swarm_t *swarm, int epochs, string target_func, int id, int n_threads, int n_particles) {
    particle_set_t *particle_set = get_particles_set(n_threads, n_particles, id);
    work++;
    //wait for all other threads
    if (work == n_threads) {
        update_global(swarm, target_func);
        work=0;
    }
    pthread_barrier_wait(&barrier);
    for (int j=0; j<epochs; j++) {
        //update velocities
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
        }
        work++;
        if (work == n_threads) {
            update_global(swarm, target_func);
            hits++;
//            cout << "barrier hitted: " << j;
            work=0;
        }
        pthread_barrier_wait(&barrier);
    }
    return;
}

/**
 * Computes the iteration phase of the PSO
 * @param particle_set: set of particles of the swarm assigned to the thread
 * @param epochs: number of iterations to be computed
 * @param target_func: function to be optimized (sphere or himmel)
 * @param id: id of the thread
 */
void compute_swarm_fast_flow(swarm_t *swarm, int epochs, string target_func, int n_threads, int n_particles) {
	ff::ParallelFor pf(n_threads);
	vector < particle_set_t * > sets;

	for (int i = 0; i < n_threads; i++) {
		sets.push_back(get_particles_set(n_threads, n_particles, i));
	}
//	cout << sets.size() << endl;

	for (int j=0; j<epochs; j++) {
		pf.parallel_for(0, sets.size(), 1, 0, [&](const long k) {
			for (int i=sets.at(k)->start; i<sets.at(k)->end; i++) {
				//update velocity
				update_velocity(&(swarm->particles[i]), swarm->global_min, target_func);
				update_position(&(swarm->particles[i]), target_func);
				float func_value = compute_bench_fun(swarm->particles[i].position, target_func);
				//update local minimum
				if (compute_bench_fun(swarm->particles->local_min, target_func) > func_value) {
					swarm->particles->local_min.x = swarm->particles[i].position.x;
					swarm->particles->local_min.y = swarm->particles[i].position.y;
				}
			}
		});
		update_global(swarm, target_func);
	}
	return;
}

void compute_swarm_fast_flow_2(swarm_t *swarm, int epochs, string target_func, int n_threads, int n_particles) {
	ff::ParallelFor pf(n_threads);
	vector < particle_set_t * > sets;

	for (int i = 0; i < n_threads; i++) {
		sets.push_back(get_particles_set(n_threads, n_particles, i));
	}
//	cout << sets.size() << endl;

	for (int j=0; j<epochs; j++) {
		pf.parallel_for(0, sets.size(), 1, 0, [&](const long k) {
			position_t partial_min = swarm->particles[sets.at(k)->start].position;
			for (int i=sets.at(k)->start; i<sets.at(k)->end; i++) {
				//update velocity
				update_velocity(&(swarm->particles[i]), swarm->global_min, target_func);
				update_position(&(swarm->particles[i]), target_func);
				float func_value = compute_bench_fun(swarm->particles[i].position, target_func);
				//update local minimum
				if (compute_bench_fun(swarm->particles->local_min, target_func) > func_value) {
					swarm->particles->local_min.x = swarm->particles[i].position.x;
						swarm->particles->local_min.y = swarm->particles[i].position.y;
				}
				if (compute_bench_fun(swarm->particles[i].local_min, target_func) < compute_bench_fun(partial_min, target_func)) {
					partial_min = swarm->particles[i].local_min;
				}
			}
			unique_lock <mutex> lock(global_min_mutex);
			update_single_global(swarm, partial_min);
		});
	}
	return;
}

void printHits() {
	cout << "Hits: " << hits << endl;
}
//TODO: randomly initialize velocities, interval?