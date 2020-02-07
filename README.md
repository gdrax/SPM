# SPM
Project and assignments solutions for the Parallel and Distribuited System course.

# Project (Application: PSO)
**Implementation of the Particle Swarm Optimization algorithm (https://it.wikipedia.org/wiki/Particle_Swarm_Optimization).**

The following default parameters for the algorithm has been choosen:
+ Inertia weight = 0.9
+ Cognitive coefficient = 2
+ Social coefficient = 2

Three version of the algorithm are provided:
+ Sequential version: sequential.cpp
+ Parallel version with native C++: multi_thread.cpp
+ Parallel version with Fast_flow: fast_flow.cpp

The code can be compiled with:
```
make compile
```
then the three versions can be executed calling:
```
./sequential [target function] [initialization type] [# particles] [# iterations]
```
```
./multi_thread [target function] [initialization type] [# particles] [# iterations] [# threads]
```
```
./fast_flow [target function] [initialization type] [# particles] [# iterations] [# threads]
```
Two target functions are available:
+ *sphere*
+ *himmel*

(see https://en.wikipedia.org/wiki/Test_functions_for_optimization for the formulas).

Two initialization types are available:
+ *random*
+ *uniform*

It's possible to get an estimate of the computing times using 1 to 4 threads calling:
```
make measure_time_local
```
or the computing time using 1 to 256 thread proceeding by powers of two, with:
```
make measure_time_phi
```
The results will be stored respectively in *local_times.txt* and *phi_times.txt*.

The execution of the experiments can take quite some time since the scripts runs the algorithm more times to better estimate the computation times.
