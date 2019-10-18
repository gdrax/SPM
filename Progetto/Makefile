CXX	= g++
CXXFLAGS = -std=c++17 -lX11
LDFLAGS = -pthread
EPOCHS = 10000

EXECUTABLE   = sequential \
		  	   thread_pool\
		  	   thread_barrier\

#estimate the computing time of the different function making the average of 10000 runs
func_times:
	make sequential
	./function_time.sh $(EPOCHS)

#compile the sequential version
sequential: utils.hpp utimer.cpp sequential.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp -o $@

#compile the thread_pool version
thread_pool: threads.hpp utimer.cpp utils.hpp thread_pool.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp -o $@

thread_barrier: thread_barrier.cpp utimer.cpp utils.hpp threads.hpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $@.cpp -o $@

measure_time_local:
	make thread_pool
	./measure_time.sh 1000 100 4

mesaure_time_phi:
	make thread_pool
	./measure_time_phi.sh 1000 1000 256

#compile all
compile:
	make $(EXECUTABLE)

#clear all executable
clear_all:
	rm $(EXECUTABLE)