#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include <cstdlib>
#include "queue.hpp"

using namespace std;

typedef enum {
  produce = 0,
  add = 1,
  square = 2,
  sub = 3,
  print = 4
} TaskType;

g_queue<int> queues [4];

int compute(TaskType type, int val) {
  switch(type) {
    case add:
      val++;
      break;
    case square:
      val = val*val;
      break;
    case sub:
      val--;
      break;
    }
    return val;
}

auto execute(TaskType type, int n) {
  int data;
  switch(type) {
    case produce:
      for (int i=0; i<n; i++) {
        queues[0].push(rand());
        this_thread::sleep_for(10ms);
      }
      break;
    case print:
      while() {
        data = queues[type-1].pop();
        this_thread::sleep_for(10ms);
        cout << data << "\n";
      }
      break;
    default:
      while() {
        data = queues[type-1].pop();
        this_thread::sleep_for(10ms);
        queues[type].push(compute(type, data));
      }
      break;
  }
}

int main(int argc, char const *argv[]) {
  int n = atoi(argv[1]);

  thread*[5] threads;

  threads[0] = new thread(Produce, n);
  threads[1] = new thread(Add, n);
  threads[2] = new thread(Square, n);
  threads[3] = new thread(Sub, n);
  threads[4] = new thread(Print, n);
}
