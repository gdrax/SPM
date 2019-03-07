#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>

using namespace std;

template <typename T>
class g_queue {
  private:
    mutex q_mutex;
    condition_variable isEmpty;
    deque<T> queue;

  public:
    g_queue(int s) {
      cout << "Queue " << s <<" created\n";
    }

    g_queue() {}

    void push(T const& value) {
      cout << "hfosih";
      unique_lock<mutex> lock(this->q_mutex);
      queue.push_front(value);
    }

    T pop() {
      unique_lock<mutex> lock(this->q_mutex);
      this->isEmpty.wait(lock, [=]{ return !this->is_empty(); });
      this->queue.pop_back();
      T rc(move(this->queue.back()));
      return rc;
    }

    bool is_empty() {
      unique_lock<mutex> lock(this->q_mutex);
      return(this->queue.empty());
    }
};

int main(int argc, char * argv[]) {

  g_queue<int> Q = g_queue<int>(1);

  for (int i=0; i<10; i++) {
    Q.push(i);
  }

  for (int i=0; i<10; i++) {
    cout << Q.pop();
  }

  return(0);
}
