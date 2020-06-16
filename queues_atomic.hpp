#ifndef _QUEUES_ATOMIC_HPP_
#define _QUEUES_ATOMIC_HPP_

#include <atomic>
#include <mutex>
#include <thread>

#include "common.hpp"

constexpr int kMaxCachelineBytes = 64;

class AtomicNodeBlockingMPSCQueue {
 public:
  AtomicNodeBlockingMPSCQueue() : head(&stub), tail(&stub), stub_used(true) {}

  void Push(QueueNode* node) noexcept {
    node->next.store(nullptr, std::memory_order_relaxed);
    QueueNode* old_head = head.load(std::memory_order_relaxed);
    while (!head.compare_exchange_weak(
        old_head, node, std::memory_order_relaxed, std::memory_order_relaxed)) {
    }
    old_head->next.store(node, std::memory_order_release);
  }

  QueueNode* Pop() noexcept {
    QueueNode* t = tail;
    QueueNode* next = t->next.load(std::memory_order_acquire);
    while (t == &stub && next == nullptr) {
      std::this_thread::yield();
      next = t->next.load(std::memory_order_acquire);
    }
    if (t == &stub) {
      t = next;
      next = t->next.load(std::memory_order_acquire);
      stub_used = false;
    }
    if (next == nullptr) {
      if (!stub_used) {
        Push(&stub);
        stub_used = true;
      }
      next = t->next.load(std::memory_order_acquire);
      while (next == nullptr) {
        std::this_thread::yield();
        next = t->next.load(std::memory_order_acquire);
      }
    }

    tail = next;
    return t;
  }

 private:
  union {
    std::atomic<QueueNode*> head;
    char tail_spacer[kMaxCachelineBytes];
  };
  union {
    QueueNode* tail;
    char head_spacer[kMaxCachelineBytes];
  };
  QueueNode stub;
  bool stub_used;
};

class AtomicNodeV2BlockingMPSCQueue {
 public:
  AtomicNodeV2BlockingMPSCQueue() : head(&stub), tail(&stub), stub_used(true) {}

  void Push(QueueNode* node) noexcept {
    node->next.store(nullptr, std::memory_order_relaxed);
    QueueNode* old_head = head.exchange(node, std::memory_order_acq_rel);
    old_head->next.store(node, std::memory_order_release);
  }

  QueueNode* Pop() noexcept {
    QueueNode* t = tail;
    QueueNode* next = t->next.load(std::memory_order_acquire);
    while (t == &stub && next == nullptr) {
      std::this_thread::yield();
      next = t->next.load(std::memory_order_acquire);
    }
    if (t == &stub) {
      t = next;
      next = t->next.load(std::memory_order_acquire);
      stub_used = false;
    }
    if (next == nullptr) {
      if (!stub_used) {
        Push(&stub);
        stub_used = true;
      }
      next = t->next.load(std::memory_order_acquire);
      while (next == nullptr) {
        std::this_thread::yield();
        next = t->next.load(std::memory_order_acquire);
      }
    }

    tail = next;
    return t;
  }

 private:
  union {
    std::atomic<QueueNode*> head;
    char tail_spacer[kMaxCachelineBytes];
  };
  union {
    QueueNode* tail;
    char head_spacer[kMaxCachelineBytes];
  };
  QueueNode stub;
  bool stub_used;
};

class AtomicNodeV3BlockingMPSCQueue {
 public:
  AtomicNodeV3BlockingMPSCQueue() : head(&stub), tail(&stub) {}

  void Push(QueueNode* node) noexcept {
    node->next.store(nullptr, std::memory_order_relaxed);
    QueueNode* old_head = head.exchange(node, std::memory_order_acq_rel);
    old_head->next.store(node, std::memory_order_release);
  }

  QueueNode* Pop() noexcept {
    QueueNode* t = tail;
    QueueNode* next = t->next.load(std::memory_order_acquire);
    while (t == &stub && next == nullptr) {
      std::this_thread::yield();
      next = t->next.load(std::memory_order_acquire);
    }
    if (t == &stub) {
      t = next;
      next = t->next.load(std::memory_order_acquire);
    }
    if (next != nullptr) {
      tail = next;
      return t;
    }
    QueueNode* h = head.load(std::memory_order_acquire);
    if (h != t) {
      do {
        std::this_thread::yield();
        next = t->next.load(std::memory_order_acquire);
      } while (next == nullptr);
      tail = next;
      return t;
    }
    Push(&stub);
    next = t->next.load(std::memory_order_acquire);
    while (next == nullptr) {
      std::this_thread::yield();
      next = t->next.load(std::memory_order_acquire);
    }
    tail = next;
    return t;
  }

 private:
  union {
    std::atomic<QueueNode*> head;
    char tail_spacer[kMaxCachelineBytes];
  };
  union {
    QueueNode* tail;
    char head_spacer[kMaxCachelineBytes];
  };
  QueueNode stub;
};

#endif