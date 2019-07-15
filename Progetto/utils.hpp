#include <c
#include <cmath>
#include <string>

using namespace std;

typedef struct {
    float x;
    float y;
} position;

float const sphere_domain_bound = 100;
float const himmel_domain_bound = 5;

float const cognitive_parameter = 2;
float const social_parameter = 2;
float const inertia_weight = 0.9;

/**
 * compute a benchmark function
 */
float compute_bench_fun(float x, float y, string func) {
    if (func == "sphere") {
        return pow(x, 2) + pow(y, 2);
    }
    if (func == "himmel") {
        return pow(pow(x, 2) + y + 11, 2) + pow(x + pow(y, 2) + 7, 2);
    }
    return 0;
}

/*
 * check if the bechmark function name is valid
 */
bool check_bench_fun(string func) {
    if (func == "sphere" || func == "himmel")
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
    return 0;
}

/**
 * Compute the new velocity of a particle
**/
float compute_velocity(float old_v, position current_p, float local_min, float global_min, string func) {
    float current_out = compute_bench_fun(current_p.x, current_p.y, func);
    float local_diff = current_out - local_min;
    float global_diff = current_out - global_min;
    return inertia_weight*old_v + cognitive_parameter*random01()*local_diff + social_parameter*random01()*global_diff;
}

/**
 * Generates a random number in [0, 1]
**/
float random01() {
    return float(((double) rand() / (RAND_MAX)) + 1);
}