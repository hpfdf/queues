#ifndef _QUEUES_SYNCHRONIZED_HPP_
#define _QUEUES_SYNCHRONIZED_HPP_

#include <deque>
#include <list>
#include <queue>

#include "common.hpp"

class StdDequeSyncQueue {
 public:
  bool Push(QueueNode* node) noexcept {
    q.push(node);
    return true;
  }

  QueueNode* Pop() noexcept {
    if (q.empty()) return nullptr;
    QueueNode* result = q.front();
    q.pop();
    return result;
  }

 private:
  std::queue<QueueNode*, std::deque<QueueNode*>> q;
};

class StdListSyncQueue {
 public:
  bool Push(QueueNode* node) noexcept {
    q.push(node);
    return true;
  }

  QueueNode* Pop() noexcept {
    if (q.empty()) return nullptr;
    QueueNode* result = q.front();
    q.pop();
    return result;
  }

 private:
  std::queue<QueueNode*, std::list<QueueNode*>> q;
};

template <size_t B>
class BufferSyncQueue {
 public:
  static_assert(B > 0 && (B & B - 1) == 0, "Buffer size must be a power of 2.");

  bool Push(QueueNode* node) noexcept {
    if (head == tail + B) return false;
    buffer[head++ % B] = node;
    return true;
  }

  QueueNode* Pop() noexcept {
    if (head == tail) return nullptr;
    return buffer[tail++ % B];
  }

 private:
  std::array<QueueNode*, B> buffer;
  size_t head = 0, tail = 0;
};

class NodeSyncQueue {
 public:
  NodeSyncQueue() : head(&stub), tail(&stub) {}

  bool Push(QueueNode* node) noexcept {
    node->next_sync = nullptr;
    head->next_sync = node;
    head = node;
    return true;
  }

  QueueNode* Pop() noexcept {
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
  QueueNode* head;
  QueueNode* tail;
  QueueNode stub;
};

#endif