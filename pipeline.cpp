#include <iostream>
#include <thread>
#include <ctime>
#include <ratio>
#include <chrono>
#include <mutex>
#include <vector>
#include <cstdlib>
#include <sched.h>
#include "queue.hpp"

using namespace std;

//tipi di task che vengono svolti dai thread
typedef enum {
  produce = 0,
  add = 1,
  square = 2,
  sub = 3,
  print = 4
} TaskType;

//code tra gli stadi della pipeline
g_queue<int> queues [4];

//0: auto scheduling, 1: stick threads to cores
int mode = 0, isf = 1;

chrono::high_resolution_clock::time_point finish, ti1, ti2;

//funzione calcolata dagli stati intermedi della pipeline
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

//codice eseguito dai thread
void body(TaskType type, int n) {
  int data;
  switch(type) {
    //primo stadio: genera un numero tra 1 e 10 e lo inserisce nella prima coda
    case produce:
      for (int i=0; i<n+1; i++) {
        cout << "Thread #" << type << " on CPU n." << sched_getcpu() << "\n";
        if (i==n) {
          //inserisco 0 per terminare i thread
          queues[0].push(0);
          //momento in cui si conclude il setup
          ti2 = chrono::high_resolution_clock::now();
        }
        else
          queues[0].push(rand()%10+1);
      }
      break;
    //ultimo stadio: prende i numeri dall'ultima coda e li stampa
    case print:
      while(1) {
        if (isf)
          //momento in cui si conclude la computazione
          finish = chrono::high_resolution_clock::now();
        cout << "Thread #" << type << " on CPU n." << sched_getcpu() << "\n";
        data = queues[type-1].pop();
        if (data != 0) {
          cout << data << "\n";
        }
        else
          break;
      }
      break;
    //stadio intermedio: prende i numeri dalla coda precedente applica la funzione e li inserisce nella coda successiva
    default:
      while(1) {
        cout << "Thread #" << type << " on CPU n." << sched_getcpu() << "\n";
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

int main(int argc, char const *argv[]) {

  if (argc != 3) {
    cout << "pipeline [MODE] [INPUTS]\nMODE->\n   0: Auto scheduling\n   1: Stick threads to cores\nINPUTS->\n   number of inputs for the pipeline\n";
    return -1;
  }

  mode = atoi(argv[1]);
  int n = atoi(argv[2]);

  vector<thread*> threads;

  ti1 = chrono::high_resolution_clock::now();
  threads.push_back(new thread(body, produce, n));
  threads.push_back(new thread(body, add, n));
  threads.push_back(new thread(body, square, n));
  threads.push_back(new thread(body, sub, n));
  threads.push_back(new thread(body, print, n));

  cpu_set_t cpuset;
  if (mode == 1) {
    //il 5o thread non è attaccato a nessuna CPU
    for (int i=0; i<4; i++) {
      CPU_ZERO(&cpuset);
      CPU_SET(i, &cpuset);
      pthread_setaffinity_np(threads[i]->native_handle(), sizeof(cpu_set_t), &cpuset);
    }
  }

  for (auto &t: threads) {
    t->join();
  }

  //tempo di completamento da quando viene inserito il primo numero a quando viene prodotto l'ultimo risultato
  chrono::duration<double> elapsed = chrono::duration_cast<chrono::duration<double>>(finish - ti2);
  //tempo speso per inserire gli input
  chrono::duration<double> tsetup = chrono::duration_cast<chrono::duration<double>>(ti2 - ti1);
  cout << "Computing time: " << elapsed.count() << "\n";
  cout << "Setup time: " << tsetup.count() << "\n";
}
