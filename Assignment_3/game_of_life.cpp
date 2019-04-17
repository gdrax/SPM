#define cimg_display 1

#include<iostream>
#include<vector>
#include<thread>

#include "CImg.h"

using namespace cimg_library;
using namespace std;

int n = 0;
int stop = 0;

bool isCompleted(bool *completed, int nw) {
  int count = 0;
  for (int i=0; i<nw; i++) {
    if (completed[i])
      count++;
  }
  if (count == nw)
    return true;
  else
    return false;
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
  bool life = source_mat[row][col];
  int neighbours_alive = 0;
  for (int i=-1; i<2; i++) {
    for (int j=-1; j<2; j++) {
      if (source_mat[real_index(row + i)][real_index(col + j)])
        neighbours_alive++;
    }
  }
  if (life && (neighbours_alive > 4 || neighbours_alive < 3)) {
    dest_mat[row][col] = false;
    //cout << "dead\n";
  }
  else if (!life && neighbours_alive == 3) {
    dest_mat[row][col] = true;
    //cout << "born\n";
  }
  else
    dest_mat[row][col] = source_mat[row][col];
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
  bool *which_mat;

public:
  Worker(int name, bool ***matrices, int start, int end, bool *completed, int nw, bool *which_mat):
    name(name), matrices(matrices), start(start), end(end), completed(completed), nw(nw), which_mat(which_mat) {cout << start << "  " << end << endl;}

  thread *run() {
    auto body = [&] () {
      while (!stop) {
        if (!completed[name]) {
          for (int i=start; i<end+1; i++) {
            compute_life(matrices[*which_mat], matrices[!(*which_mat)], i);
          }
          completed[name] = true;
        }
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
  bool *which_mat;

public:
  Drawer(bool ***matrices, bool *completed, int nw, bool *which_mat):
    matrices(matrices), completed(completed), nw(nw), which_mat(which_mat) {}

  thread *run() {
    auto body = [&] () {
      CImg<unsigned char> board(n,n,1,3,0);
      CImgDisplay draw_disp(board,"Click a point");
      while(!draw_disp.is_closed()) {
        for (int i=0; i<n; i++) {
          for (int j=0; j<n; j++) {
            if (matrices[*which_mat][i][j] == 1)
              board(i, j, 1) = 255;
            else
              board(i, j, 1) = 0;
            board.display(draw_disp);
          }
        }
        if (isCompleted(completed, nw)) {
          cout << "hipwebfpewibfpweifbwpebfwfbw\n";
          *which_mat = !(*which_mat);
          for (int i=0; i<nw; i++) {
            completed[nw] = false;
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

  bool *completed = new bool [nw];

  for (int i=0; i<nw; i++) {
    completed[i] = true;
  }


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
  bool which_mat = 0;
  Drawer *drawer = new Drawer(matrices, completed, nw, &which_mat);
  for (int i=0; i<nw; i++) {
    workers.push_back(new Worker(i, matrices, 0+n/nw*i, (n/nw-1)+n/nw*i, completed, nw, &which_mat));
  }

  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++) {
      matrices[0][i][j] = rand() % 2;
    }
  }

  //avvio i thread
  for (auto w: workers) {
    threads.push_back(w->run());
  }
  threads.push_back(drawer->run());

/*
  int which = 0;
  while(!stop) {
    if (!isCompleted(completed, nw)) {
      for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
          matrices[which][i][j] = rand() % 2;
        }
      }
      for (int i=0; i<n*n; i++) {
        compute_life(matrices[which], matrices[!which], i);
      }
      completed[0] = 1;
    }
    which = !which;
  }
*/
  //join
  for (auto t: threads) {
    t->join();
  }
  return 0;
}
