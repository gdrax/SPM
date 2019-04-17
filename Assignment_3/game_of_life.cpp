#define cimg_display 1

#include<iostream>
#include<vector>
#include<thread>

#include "CImg.h"

using namespace cimg_library;
using namespace std;

int n = 0;
int stop = 0;

bool swap(bool *completed, int nw) {
  int count = 0;
  for (int i=0; i<nw; i++) {
    if (completed[i])
      count++;
  }
  if (count == nw) {
    completed = new bool [nw] ();
    return true;
  }
  else {
    return false;
  }
}

/*
Return the real index of the mash
*/
int real_index(int index) {
  if (index >= n) return index - n;
  if (index < 0) return n + index;
  return index;
}

/*
Calcola lo stato delle celle alla prossima iterazione
*/
void compute_life(bool **source_mat, bool **dest_mat, int index) {
  int row = (int)index / n;
  int col = index % n;
  int life = source_mat[row][col];
  int neighbours_alive = 0;
  for (int i=-1; i<2; i++) {
    for (int j=-1; j<2; j++) {
      if (source_mat[real_index(row + i)][real_index(col + j)])
        neighbours_alive++;
    }
  }
  if (life && (neighbours_alive > 4 || neighbours_alive < 3))
    dest_mat[row][col] = false;
  else if (!life && neighbours_alive == 3)
    dest_mat[row][col] = true;
}

/*
Structure of a worker thread
*/
class Worker {
private:
  bool ***matrices;
  int start;
  int end;
  int name;
  bool *completed;
  int nw;
  int witch_mat = 0;

public:
  Worker(int name, bool ***matrices, int start, int end, bool *completed, int nw):
    name(name), matrices(matrices), start(start), end(end), completed(completed), nw(nw) {}

  thread *run() {
    auto body = [&] () {
      while (!stop && swap(completed, nw)) {
        for (int i=start; i<end+1; i++) {
          compute_life(matrices[witch_mat], matrices[!witch_mat], i);
        }
        completed[name] = 1;
      }
      return;
      };

    return new thread(body);
  }
};

/*
Struttura del thread che disegna la matrices
*/
class Drawer {
private:
  bool ***matrices;
  bool *completed;
  int nw;
  int witch_mat = 0;

public:
  Drawer(bool ***matrices, bool *completed, int nw):
    matrices(matrices), completed(completed), nw(nw) {}

  thread *run() {
    auto body = [&] () {
      CImg<unsigned char> board(n,n,1,3,0);
      CImgDisplay draw_disp(board,"Click a point");
      while(!draw_disp.is_closed()) {
        if (swap(completed, nw)) {
          witch_mat = !witch_mat;
        }
        for (int i=0; i<n; i++) {
          for (int j=0; j<n; j++) {
            if (matrices[witch_mat][i][j] == true) {
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
  int init = atoi(argv[3]);

  bool *completed = new bool [nw] ();


  //creo due matrici di bool e le inizializzo con 0
  bool ***matrices = new bool**[2];
  matrices[0] = new bool *[n];
  matrices[1] = new bool *[n];
  for (int i=0; i<n; i++) {
    matrices[0][i] = new bool [n] ();
    matrices[1][i] = new bool [n] ();
  }

  //creo i thread
  vector<thread *> threads;
  vector<Worker *> workers;
  Drawer *drawer = new Drawer(matrices, completed, nw);
  for (int i=0; i<nw; i++) {
    workers.push_back(new Worker(i, matrices, 0+n/nw*i, (n/nw-1)+n/nw*i, completed, nw));
  }

  //avvio i thread
  threads.push_back(drawer->run());
  for (auto w: workers) {
    //threads.push_back(w->run());
  }

  //join
  for (auto t: threads) {
    t->join();
  }
  return 0;
}
