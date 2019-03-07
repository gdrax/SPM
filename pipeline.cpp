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

void body(TaskType type, int n) {
  int data;
  switch(type) {
    //primo stadio: genera un numero tra 1 e 10 e lo inserisce nella prima coda
    case produce:
      for (int i=0; i<n+1; i++) {
        cout << "producing\n";
        if (i==n)
          //inserisco 0 per terminare i thread
          queues[0].push(0);
        else
          queues[0].push(rand()%10+1);
        this_thread::sleep_for(10ms);
      }
      break;
    //ultimo stadio: prende i numeri dall'ultima coda e li stampa
    case print:
      cout << "printer working\n";
      while(1) {
        data = queues[type-1].pop();
        if (data != 0) {
          this_thread::sleep_for(10ms);
          cout << data << "\n";
        }
        else
          break;
      }
      break;
    //stadio intermedio: prende i numeri dalla coda precedente applica la funzione e li inserisce nella coda successiva
    default:
      while(1) {
        data = queues[type-1].pop();
        if (data != 0) {
          this_thread::sleep_for(10ms);
          queues[type].push(compute(type, data));
        }
        else
          break;
      }
      break;
  }
  return;
}

;int main(int argc, char const *argv[]) {
  int n = atoi(argv[1]);

  vector<thread*> threads;

  threads.push_back(new thread(body, add, n));
  threads.push_back(new thread(body, square, n));
  threads.push_back(new thread(body, sub, n));
  threads.push_back(new thread(body, print, n));
  this_thread::sleep_for(1s);
  threads.push_back(new thread(body, produce, n));

  for (int i=0; i<2; i++) {
    threads[i]->join();
  }
}
