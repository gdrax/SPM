#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <vector>
#include "queue.hpp"

using namespace std;

typedef enum {
  Produce,
  Add,
  Square,
  Sub,
  Print
} TaskType;

void body(TaskType type) {
  switch(type) {
    case Produce:
      break;
    case Add:
      break;
    case Square:
      break;
    case Sub:
      break;
    case Print:
      break;
  }
}


int main(int argc, char const *argv[]) {
  int n = atoi(argv[1]);
  vector<g_queue>

}
