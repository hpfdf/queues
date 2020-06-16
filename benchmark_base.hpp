#ifndef _BENCHMARK_BASE_HPP_
#define _BENCHMARK_BASE_HPP_

#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

#include "benchmark/benchmark.h"
#include "common.hpp"

struct Payload : public QueueNode {
  uint64_t data;
};

template <int64_t S>
class BenchmarkBase : public benchmark::Fixture {
 public:
  constexpr static int64_t kSize = S;
  constexpr static int64_t kSum = kSize * (kSize - 1) / 2;

  BenchmarkBase() {
    if (!payloads.empty()) return;
    payloads.resize(kSize);
    for (int64_t i = 0; i < kSize; ++i) {
      payloads[i] = new Payload();
      payloads[i]->data = i;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(payloads.begin(), payloads.end(), g);
  }

 protected:
  std::vector<Payload*> payloads;
};

// Unblocking queue interfaces

template <class UnblockingQueue>
inline bool UnblockingQueuePush(UnblockingQueue& q, Payload* payload) noexcept {
  return q.Push(payload);
}

template <class UnblockingQueue>
inline void UnblockingQueueWaitPush(UnblockingQueue& q,
                                    Payload* payload) noexcept {
  while (!q.Push(payload)) {
  }
}

template <class UnblockingQueue>
inline Payload* UnblockingQueuePop(UnblockingQueue& q) noexcept {
  return (Payload*)q.Pop();
}

template <class UnblockingQueue>
inline Payload* UnblockingQueueWaitPop(UnblockingQueue& q) noexcept {
  Payload* result = (Payload*)q.Pop();
  while (!result) {
    result = (Payload*)q.Pop();
  }
  return result;
}

// Blocking queue interfaces

template <class BlockingQueue>
inline void BlockingQueuePush(BlockingQueue& q, Payload* payload) noexcept {
  q.Push(payload);
}

template <class BlockingQueue>
inline Payload* BlockingQueuePop(BlockingQueue& q) noexcept {
  return (Payload*)q.Pop();
}

#endif