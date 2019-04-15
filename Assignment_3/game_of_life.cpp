#define cimg_display 1

#include<iostream>
#include<vector>
#include<thread>

#include "CImg.h"

using namespace cimg_library;
using namespace std;

int n = 0;
/*
Structure of a worker thread
*/

class Worker {
private:
  bool **matrices;
  int start;
  int end;
  int name;

public:
  Worker(int name, bool **matrices, int start, int end):
    name(name), matrices(matrices), start(start), end(end) {}

  thread *run() {
    auto body = [&] () {
      n=0;

      return;
      };

    return new thread(body);
  }
};


int main(int argc, char const *argv[]) {
  if (argc != 4) {
    cout << argv[0] << " [N. WORKERS] [SIZE OF MATRICES] [INITIALIZATION]\n[INITIALIZATION]\n0 : random\n1 : arrows";
    return -1;
  }

  int nw = atoi(argv[1]);
  n = atoi(argv[2]);
  int init = atoi(argv[3]);

  bool matrices[n][n][2];
  for (int i=0; i<n; i++) {
    for (int j=0; j<100; j++) {
      matrices[i][j][0];
      matrices[i][j][1];
    }
  }


  CImg<unsigned char> visu(200,200,1,3,0);
  const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
  CImgDisplay draw_disp(visu,"Intensity profile");
  int i=0;
  int j=0;
  while (!draw_disp.is_closed()) {
    if (i<200)
      i+=4;
    else {
      i=0;
      j+=3;
    }
    visu(j, i, 1) = 255;
    visu.display(draw_disp);
  }
  return 0;
}
