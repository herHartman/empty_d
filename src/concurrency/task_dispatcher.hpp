#pragma once

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

namespace e_game::concurrency {

// TODO(make task pipeline)

class TaskWrapper {
 private:
  struct ImplBase {
    virtual void Call() = 0;
    virtual ~ImplBase() {}
  };

  template <typename F>
  struct ImplType : public ImplBase {
    F function_;
    ImplType(F&& function) : function_(std::move(function)) {}
    void Call() { function_(); }
  };

 public:
  template <typename F>
  TaskWrapper(F&& function) : impl_(new ImplType<F>(std::move(function))) {}

  TaskWrapper() = default;
  TaskWrapper(TaskWrapper&& other) : impl_(std::move(other.impl_)) {}
  TaskWrapper& operator=(TaskWrapper&& other) {
    impl_ = std::move(other.impl_);
    return *this;
  }

  void operator()() { impl_->Call(); }

  TaskWrapper(const TaskWrapper&) = delete;
  TaskWrapper(TaskWrapper&) = delete;
  TaskWrapper& operator=(const TaskWrapper&) = delete;

 private:
  std::unique_ptr<ImplBase> impl_;
};

class NotificationQueue {
 public:
  bool TryPop(std::optional<TaskWrapper>& task) {
    std::unique_lock lock{main_lock_, std::try_to_lock};
    if (!lock || tasks_queue_.empty()) return false;
    task = std::move(tasks_queue_.front());
    tasks_queue_.pop_front();
    return true;
  }

  template <typename T>
  bool TryPush(T&& task) {
    {
      std::unique_lock lock{main_lock_, std::try_to_lock};
      if (!lock) return false;
      tasks_queue_.emplace_back(std::forward<T>(task));
    }
    task_awaiter_.notify_one();
  }

  bool Pop(std::optional<TaskWrapper>& task) {
    std::unique_lock lock{main_lock_};
    while (tasks_queue_.empty() && !done_) task_awaiter_.wait(lock);
    if (tasks_queue_.empty()) return false;
    task = std::move(tasks_queue_.front());
    tasks_queue_.pop_front();
    return true;
  }

  template <typename T>
  void Push(T&& task) {
    std::unique_lock lock_{main_lock_};
    tasks_queue_.emplace_back(std::forward<T>(task));
    task_awaiter_.notify_one();
  }

 private:
  std::deque<TaskWrapper> tasks_queue_;
  std::mutex main_lock_;
  std::condition_variable task_awaiter_;
  bool done_{false};
};

class AsyncTaskExecutor {
 public:
 private:
  void run() {
    while (true) {
      std::optional<TaskWrapper> f;
      for (int i = 0; i < workers_.size(); ++i) {
        if (queues_[(index_ + i) % workers_.size()].TryPop(f)) break;
      }

      if (!(f || queues_[index_ % workers_.size()].Pop(f))) break;

      f.value()();
    }
  }

  template <typename FunctionType>
  std::future<typename std::result_of_t<FunctionType()>> RunTaskAsync(
      FunctionType&& f) {
    typedef typename std::result_of_t<FunctionType()> ResultType;
    std::packaged_task<ResultType> task(std::move(f));
    std::future<ResultType> result(task.get_future());

    for (int i = 0; i < workers_.size(); ++i) {
      if (queues_[(index_ + i) % workers_.size()].TryPush(std::move(task)))
        break;
    }

    queues_[index_ % workers_.size()].Push(std::move(task));
    return result;
  }

  std::vector<std::thread> workers_;
  std::atomic<int> index_;
  std::vector<NotificationQueue> queues_;
};
}  // namespace e_game::concurrency