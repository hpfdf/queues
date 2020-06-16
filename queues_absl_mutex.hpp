#ifndef _QUEUES_ABSL_MUTEX_HPP_
#define _QUEUES_ABSL_MUTEX_HPP_

#include <array>
#include <new>

#include "absl/synchronization/mutex.h"
#include "common.hpp"

// absl::Mutex Buffer Multi-Producer-Multi-Consumer Unblocking Queue
template <size_t B>
class AbslMutexBufferUnblockingMPMCQueue {
 public:
  static_assert(B > 0 && (B & B - 1) == 0, "Buffer size must be a power of 2.");

  bool Push(QueueNode* node) noexcept {
    absl::MutexLock l(&mu);
    if (head == tail + B) return false;
    buffer[head++ % B] = node;
    return true;
  }

  QueueNode* Pop() noexcept {
    absl::MutexLock l(&mu);
    if (head == tail) return nullptr;
    return buffer[tail++ % B];
  }

 private:
  std::array<QueueNode*, B> buffer;
  size_t head = 0;
  size_t tail = 0;
  absl::Mutex mu;
};

// absl::Mutex Buffer Multi-Producer-Multi-Consumer Blocking Queue
template <size_t B>
class AbslMutexBufferBlockingMPMCQueue {
 public:
  static_assert(B > 0 && (B & B - 1) == 0, "Buffer size must be a power of 2.");
  void Push(QueueNode* node) noexcept {
    {
      absl::MutexLock l(&mu);
      while (head == tail + B) pcv.Wait(&mu);
      buffer[head++ % B] = node;
    }
    ccv.Signal();
  }

  QueueNode* Pop() noexcept {
    QueueNode* result;
    {
      absl::MutexLock l(&mu);
      while (head == tail) ccv.Wait(&mu);
      result = buffer[tail++ % B];
    }
    pcv.Signal();
    return result;
  }

 private:
  std::array<QueueNode*, B> buffer;
  size_t head = 0;
  size_t tail = 0;
  absl::Mutex mu;
  absl::CondVar pcv, ccv;
};

// absl::Mutex Node Multi-Producer-Single-Consumer Unblocking Queue
class AbslMutexNodeUnblockingMPSCQueue {
 public:
  bool Push(QueueNode* node) noexcept {
    node->next_sync = nullptr;
    absl::MutexLock l(&mu);
    head->next_sync = node;
    head = node;
    return true;
  }

  QueueNode* Pop() noexcept {
    QueueNode* t = tail;
    if (t == &stub && t->next_sync == nullptr) return nullptr;
    if (t == &stub) t = t->next_sync;
    if (t->next_sync == nullptr) {
      absl::MutexLock l(&mu);
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
  absl::Mutex mu;
};

// absl::Mutex Node Multi-Producer-Multi-Consumer Unblocking Queue
class AbslMutexNodeUnblockingMPMCQueue {
 public:
  bool Push(QueueNode* node) noexcept {
    node->next_sync = nullptr;
    absl::MutexLock l(&mu);
    head->next_sync = node;
    head = node;
    return true;
  }

  QueueNode* Pop() noexcept {
    absl::MutexLock l(&mu);
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
  absl::Mutex mu;
};

// absl::Mutex Node Multi-Producer-Multi-Consumer Blocking Queue
class AbslMutexNodeBlockingMPMCQueue {
 public:
  void Push(QueueNode* node) noexcept {
    node->next_sync = nullptr;
    {
      absl::MutexLock l(&mu);
      head->next_sync = node;
      head = node;
    }
    ccv.Signal();
  }

  QueueNode* Pop() noexcept {
    absl::MutexLock l(&mu);
    while (tail == &stub && tail->next_sync == nullptr) ccv.Wait(&mu);
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
  absl::Mutex mu;
  absl::CondVar ccv;
};

#endif