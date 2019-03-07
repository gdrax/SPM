#include <iostream>
#include <thread>
#include <ctime>
#include <ratio>
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

chrono::high_resolution_clock::time_point start, finish;

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
      start = chrono::high_resolution_clock::now();
      for (int i=0; i<n+1; i++) {
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
      while(1) {
        data = queues[type-1].pop();
        if (data != 0) {
          this_thread::sleep_for(10ms);
          cout << data << "\n";
        }
        else {
          finish = chrono::high_resolution_clock::now();
          break;
        }
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
        else {
          queues[type].push(0);
          break;
        }
      }
      break;
  }
  return;
}

;int main(int argc, char const *argv[]) {
  int n = atoi(argv[1]);

  vector<thread*> threads;

  chrono::high_resolution_clock::time_point tstart = chrono::high_resolution_clock::now();
  threads.push_back(new thread(body, add, n));
  threads.push_back(new thread(body, square, n));
  threads.push_back(new thread(body, sub, n));
  threads.push_back(new thread(body, print, n));
  chrono::high_resolution_clock::time_point tend = chrono::high_resolution_clock::now();
  //avvio il primo stadio dopo 1 sec così tutti gli altri si sono sospesi sulle code
  this_thread::sleep_for(1s);
  chrono::high_resolution_clock::time_point tstart2 = chrono::high_resolution_clock::now();
  threads.push_back(new thread(body, produce, n));
  chrono::high_resolution_clock::time_point tend2 = chrono::high_resolution_clock::now();

  for (auto &t: threads) {
    t->join();
  }

  chrono::duration<double> elapsed = chrono::duration_cast<chrono::duration<double>>(finish - start);
  chrono::duration<double> telapsed = chrono::duration_cast<chrono::duration<double>>(tend - tstart + tend2 - tstart2);

  cout << "ELapsed time: " << elapsed.count() << "\n";
  cout << "Thread creation time: " << telapsed.count() << "\n";
}
