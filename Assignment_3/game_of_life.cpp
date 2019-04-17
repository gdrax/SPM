#define cimg_display 1

#include<iostream>
#include<vector>
#include<thread>

#include "CImg.h"

using namespace cimg_library;
using namespace std;

int n = 0;
int stop = 0;
/*
Structure of a worker thread
*/

class Worker {
private:
  bool ***matrices;
  int start;
  int end;
  int name;

public:
  Worker(int name, bool ***matrices, int start, int end):
    name(name), matrices(matrices), start(start), end(end) {}

  thread *run() {
    auto body = [&] () {
      while (!stop) {

      }
      return;
      };

    return new thread(body);
  }
};

class Drawer {
private:
  bool ***matrices;

public:
  Drawer(bool ***matrices):
    matrices(matrices) {}

  thread *run() {
    auto body = [&] () {
      CImg<unsigned char> board(n,n,1,3,0);
      CImgDisplay draw_disp(board,"Game of life");
      while(!draw_disp.is_closed()) {
        for (int i=0; i<n; i++) {
          for (int j=0; j<n; j++) {
            if (matrices[i][j][0] == true) {
              board(i, j, 1) = 255;
              board.display(draw_disp);
            }
          }
        }
      }
      stop = 1;
      return;
    };
    return new thread(body);
  }
};


int main(int argc, char const *argv[]) {
  if (argc != 4) {
    cout << argv[0] << " [N. WORKERS] [SIZE OF MATRICES] [INITIALIZATION]\n[INITIALIZATION] --> 0 : random, 1 : arrows\n";
    return -1;
  }

  int nw = atoi(argv[1]);
  n = atoi(argv[2]);
  int const dim = n;
  int init = atoi(argv[3]);

  bool ***matrices = new bool[dim][dim][2];
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      matrices[i][j][0] = 0;
      matrices[i][j][1] = 0;
    }
  }

  vector<thread *> threads;
  vector<Worker *> workers;
  Drawer *drawer = new Drawer(matrices);
  for (int i=0; i<nw; i++) {
    workers.push_back(new Worker(i, matrices, 0+n/nw*i, (n/nw-1)+n/nw*i));
  }

  threads.push_back(drawer->run());
  for (auto w: workers) {
    threads.push_back(w->run());
  }

  for (auto t: threads) {
    t->join();
  }
  return 0;
}
