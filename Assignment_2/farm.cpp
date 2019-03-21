#include <iostream>
#include <thread>
#include <mutex>

#include "queue.hpp"

int nw;

boolean is_prime(int n) {
  for (i=2; i<= n/2; i++) {
    if (n%i == 0)
      return false;
  }
  return true;
}

int worker(int n) {
  int count_primes = 0;
  for (i=n; i>1; i++) {
    if (is_prime(i))
      count_primes++;
  }
}

int main(int argc, char const *argv[]) {

  if (argc != 3) {
    cout << "farm [N_WORKERS] [N_INPUTS]\n";
    return -1;
  }

  nw = atoi(argv[1]);
  int n = atoi(argv[2]);

  

  return 0;
}
