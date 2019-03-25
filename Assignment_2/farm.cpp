#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <functional>

#include "queue.hpp"

using namespace std;

/*
Calculate if a number is prime
*/
bool is_prime(int n) {
  for (int i=2; i<= n/2; i++) {
    if (n%i == 0)
      return false;
  }
  return true;
}

/*
Structure of a worker thread
*/
template<typename input, typename output> class Worker {
private:
  function<output(input)> work;
  g_queue<input>& inQ;
  g_queue<output>& outQ;
  string name;

public:
  Worker(string name, function<output(input)> f, g_queue<input>& inQ, g_queue<output>& outQ):
    name(name), work(f), inQ(inQ), outQ(outQ) {}

  thread *run() {
    auto body = [&] () {

      auto item = inQ->pop();

      while(item > 0) {
        auto result = work(item);

        outQ->push(result);

        //this_thread::sleep_for(100ms);

        item = inQ->pop();
      }
      return;
    };

    auto tid = new thread(body);
    return tid;
  }
};

int main(int argc, char const *argv[]) {

  if (argc != 3) {
    cout << "farm [N_WORKERS] [N_INPUTS]\n";
    return -1;
  }

  int nw = atoi(argv[1]);
  int n = atoi(argv[2]);

  g_queue<int> inputQ;
  g_queue<int> outputQ;

  /*
  Counts number of prime numbers in range [2, n]
  */
  auto count_primes = [] (int n) {
    int count = 0;
    for (int i=n; i>1; i--) {
      if (is_prime(i))
        count++;
    }
    return count;
  };

  for (int i=0; i<n; i++) {
    inputQ.push(50);
  }

  vector<Worker<int, int>*> workers;
  vector<thread*> threads;

  for (int i=0; i<nw; i++) {
    workers.push_back(new Worker<int, int>("worker_"+i, count_primes, inputQ, outputQ));
  }

  for (int i=0; i<nw; i++) {
    inputQ.push(0);
  }

  for (auto &t: threads) {
    t->join();
  }

  return 0;
}
