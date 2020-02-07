CXX	= g++
CXXFLAGS = -std=c++17 -lX11
LDFLAGS = -pthread
FFFLAGS = -I ./fastflow
EPOCHS = 10000

EXECUTABLE   = sequential \
		  	   multi_thread  \
		  	   fast_flow \
		  	   prova

#estimate the computing time of the different function making the average of 10000 runs
func_times:
	make sequential
	./function_time.sh $(EPOCHS)

#compile the sequential version
sequential: utils.hpp utimer.cpp sequential.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(FFFLAGS) $@.cpp -o $@

multi_thread: multi_thread.cpp utimer.cpp utils.hpp threads.hpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(FFFLAGS) $@.cpp -o $@

prova: prova.cpp utimer.cpp utils.hpp threads.hpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(FFFLAGS) $@.cpp -o $@

fast_flow: fast_flow.cpp utimer.cpp utils.hpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(FFFLAGS) $@.cpp -o $@

measure_time_sequential:
	make sequential
	./measure_sequential.sh 1000 100

measure_time_local:
	make multi_thread
	./measure_time.sh 1000 100 4

measure_time_phi:
	make multi_thread
	./measure_time_phi.sh 10000 1000 256

measure_time_phi_ff:
	make fast_flow
	./measure_time_phi_ff.sh 10000 1000 256

#compile all
compile:
	make $(EXECUTABLE)

#clear all executable
clear_all:
	rm $(EXECUTABLE)
