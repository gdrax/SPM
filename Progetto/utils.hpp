#include <cmath>
#include <string>
#include <iostream>

using namespace std;

unsigned int seed = 6;

/**
 * Structure for a position in the search space
 */
typedef struct {
    float x;
    float y;
} position_t;

/**
 * Structure for a particle of the swarm
 */
typedef struct {
    position_t position; //current position of the particle in the search space
    position_t local_min; //best position found by the particle so far
    float vx; //x component of the velocity
    float vy; //y component of the velocity
} particle_t;

/**
 * Structure for a set of particles assigned to one thread
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
float const matyas_domain_bound = 10;
float const sum_doman_bound = 10;

/* Default parameters to compute the velocity */
float const cognitive_parameter = 2;
float const social_parameter = 2;
float const inertia_weight = 0.9;
float const velocity_clamp = 0.5;

/**
 * Generates a random number in [0, 1] sargio
**/
float random01() {
//    srand(seed);
    return (float)((float)rand() / (float)(RAND_MAX));
}

/**
 * Generates a random number in [a, b] sargio
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
    if (func == "matyas") {
        return 0.26 * (pow(p.x, 2) + pow(p.y, 2)) - 0.48 * p.x * p.y;
    }
    if (func == "sum") {
        return p.x + p.y;
    }
    return 0;
}

/*
 * check if the bechmark function name is valid
 */
bool check_bench_fun(string func) {
    if (func == "sphere" || func == "himmel" || func == "matyas" || func == "sum")
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
    if (func == "matyas")
        return matyas_domain_bound;
    if (func == "sum")
        return sum_doman_bound;
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
void update_position(particle_t *particle) {
    particle->position.x += particle->vx;
    particle->position.y += particle->vy;
}

/**
 * Update the position of the local minimun of a particle
 */
void update_local(particle_t *particle, string func) {
    update_position(particle);
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

void update_single_global(swarm_t *swarm, string func, int index) {
    swarm->global_min.x = swarm->particles[index].position.x;
    swarm->global_min.y = swarm->particles[index].position.y;
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
        cout << "ERROR: function_name is not valid. The available functions are: \"sphere\", \"himmel\", \"matyas\", \"sum\"\n";
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
//TODO: randomly initialize velocities, interval?