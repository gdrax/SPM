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
      unique_lock<mutex> lock(this->q_mutex);
      queue.push_front(value);
      this->isEmpty.notify_one();
    }

    T pop() {
      unique_lock<mutex> lock(this->q_mutex);
      this->isEmpty.wait(lock, [=] {return !queue.empty();});
      T ret = this->queue.back();
      this->queue.pop_back();
      return ret;
    }

    bool is_empty() {
      unique_lock<mutex> lock(this->q_mutex);
      return(this->queue.empty());
    }
};
