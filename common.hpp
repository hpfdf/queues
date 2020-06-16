#ifndef _QUEUES_COMMON_HPP_
#define _QUEUES_COMMON_HPP_

#include <atomic>
#include <cstdint>

// intrusive queue element
struct QueueNode {
  union {
    std::atomic<QueueNode*> next;
    QueueNode* next_sync;
  };

  QueueNode() { next_sync = nullptr; }
};

#endif