#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>

using namespace std;

template <typename T>g_queue
class Queue {
  private:
    mutex q_mutex;
    condition_variable isEmpty;
    deque<T> queue;
    int work;

  public:
    Queue() {}

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

    void reset_work() {
    	work = 0;
    }

    void add_work() {
    	work++;
    	if (work == n_threads) {
    		this.workDone.notify_one();
    	}
    }

};
