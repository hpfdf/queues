#ifndef _QUEUES_STD_MUTEX_HPP_
#define _QUEUES_STD_MUTEX_HPP_

#include <array>
#include <condition_variable>
#include <mutex>
#include <new>

#include "common.hpp"

// std::mutex Buffer Multi-Producer-Multi-Consumer Unblocking Queue
template <size_t B>
class StdMutexBufferUnblockingMPMCQueue {
 public:
  static_assert(B > 0 && (B & B - 1) == 0, "Buffer size must be a power of 2.");
  bool Push(QueueNode* node) noexcept {
    std::lock_guard<std::mutex> lg(mu);
    if (head == tail + B) return false;
    buffer[head++ % B] = node;
    return true;
  }

  QueueNode* Pop() noexcept {
    std::lock_guard<std::mutex> lg(mu);
    if (head == tail) return nullptr;
    return buffer[tail++ % B];
  }

 private:
  std::array<QueueNode*, B> buffer;
  size_t head = 0;
  size_t tail = 0;
  std::mutex mu;
};

// std::mutex Buffer Multi-Producer-Multi-Consumer Blocking Queue
template <size_t B>
class StdMutexBufferBlockingMPMCQueue {
 public:
  static_assert(B > 0 && (B & B - 1) == 0, "Buffer size must be a power of 2.");
  void Push(QueueNode* node) noexcept {
    {
      std::unique_lock<std::mutex> ul(mu);
      pcv.wait(ul, [this] { return head != tail + B; });
      buffer[head++ % B] = node;
    }
    ccv.notify_one();
  }

  QueueNode* Pop() noexcept {
    QueueNode* result;
    {
      std::unique_lock<std::mutex> ul(mu);
      ccv.wait(ul, [this] { return head != tail; });
      result = buffer[tail++ % B];
    }
    pcv.notify_one();
    return result;
  }

 private:
  std::array<QueueNode*, B> buffer;
  size_t head = 0;
  size_t tail = 0;
  std::mutex mu;
  std::condition_variable pcv, ccv;
};

// std::mutex Node Multi-Producer-Single-Consumer Unblocking Queue
class StdMutexNodeUnblockingMPSCQueue {
 public:
  bool Push(QueueNode* node) noexcept {
    node->next_sync = nullptr;
    std::lock_guard<std::mutex> lg(mu);
    head->next_sync = node;
    head = node;
    return true;
  }

  QueueNode* Pop() noexcept {
    QueueNode* t = tail;
    if (t == &stub && t->next_sync == nullptr) return nullptr;
    if (t == &stub) t = t->next_sync;
    if (t->next_sync == nullptr) {
      std::lock_guard<std::mutex> lg(mu);
      if (t == head) {
        stub.next_sync = nullptr;
        tail = head = &stub;
        return t;
      }
    }
    tail = t->next_sync;
    return t;
  }

 private:
  QueueNode stub;
  QueueNode* head = &stub;
  QueueNode* tail = &stub;
  std::mutex mu;
};

// std::mutex Node Multi-Producer-Multi-Consumer Unblocking Queue
class StdMutexNodeUnblockingMPMCQueue {
 public:
  bool Push(QueueNode* node) noexcept {
    node->next_sync = nullptr;
    std::lock_guard<std::mutex> lg(mu);
    head->next_sync = node;
    head = node;
    return true;
  }

  QueueNode* Pop() noexcept {
    std::lock_guard<std::mutex> lg(mu);
    QueueNode* t = tail;
    if (t == &stub && t->next_sync == nullptr) return nullptr;
    if (t == &stub) t = t->next_sync;
    if (t == head) {
      stub.next_sync = nullptr;
      tail = head = &stub;
    } else {
      tail = t->next_sync;
    }
    return t;
  }

 private:
  QueueNode stub;
  QueueNode* head = &stub;
  QueueNode* tail = &stub;
  std::mutex mu;
};

// std::mutex Node Multi-Producer-Multi-Consumer Blocking Queue
class StdMutexNodeBlockingMPMCQueue {
 public:
  void Push(QueueNode* node) noexcept {
    node->next_sync = nullptr;
    {
      std::unique_lock<std::mutex> ul(mu);
      head->next_sync = node;
      head = node;
    }
    ccv.notify_one();
  }

  QueueNode* Pop() noexcept {
    std::unique_lock<std::mutex> ul(mu);
    while (tail == &stub && tail->next_sync == nullptr) ccv.wait(ul);
    QueueNode* t = tail;
    if (t == &stub) t = t->next_sync;
    if (t->next_sync == nullptr) {
      stub.next_sync = nullptr;
      tail = head = &stub;
    } else {
      tail = t->next_sync;
    }
    return t;
  }

 private:
  QueueNode stub;
  QueueNode* head = &stub;
  QueueNode* tail = &stub;
  std::mutex mu;
  std::condition_variable ccv;
};

#endif