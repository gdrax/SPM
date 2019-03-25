#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <functional>

#include "queue.hpp"
#include "utimer.cpp"

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
  int name;

public:
  Worker(int name, function<output(input)> f, g_queue<input>& inQ, g_queue<output>& outQ):
    name(name), work(f), inQ(inQ), outQ(outQ) {}

  thread *run() {
    auto body = [&] () {

      auto item = inQ.pop();

      while(item > 0) {
        auto result = work(item);

        outQ.push(result);

        //this_thread::sleep_for(100ms);
        cout << item << " computed by worker " << name << endl;

        item = inQ.pop();
      }
      return;
    };

    auto tid = new thread(body);
    return tid;
  }
};

int main(int argc, char const *argv[]) {

  if (argc != 3) {
    cout << "farm [N. WORKERS] [N. ITEM TO COMPUTE] [NUMBER TO COMPUTE]\n";
    return -1;
  }

  int nw = atoi(argv[1]);
  int n = atoi(argv[2]);

  g_queue<int> inputQ;
  g_queue<int> outputQ;

  /*
  Counts number of prime numbers in range [2, n]
  */
  auto count_primes = [] (int num) {
    int count = 0;
    for (int i=num; i>1; i--) {
      if (is_prime(i))
        count++;
    }
    cout << "result = " << count << endl;
    return count;
  };

  for (int i=0; i<n; i++) {
    inputQ.push(100);
  }
  inputQ.push(0);

  vector<Worker<int, int>*> workers;
  vector<thread*> threads;

  for (int i=0; i<nw; i++) {
    workers.push_back(new Worker<int, int>(i, count_primes, inputQ, outputQ));
  }

  {
    utimer u("total");

    for (auto w: workers) {
      threads.push_back(w->run());
    }

    for (auto t: threads) {
      t->join();
    }
  }

  return 0;
}
