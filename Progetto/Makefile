CXX	= g++
CXXFLAGS = -std=c++17 -lX11
LDFLAGS = -pthread
EPOCHS = 10000

EXECUTABLE   = sequential \
		  	   thread_pool\
		  	   prova\

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

#compile all
compile:
	make $(EXECUTABLE)

#clear all executable
clear_all:
	rm $(EXECUTABLE)