#include <cstdint>
#include <memory>
#include <thread>

#include "benchmark/benchmark.h"
#include "benchmark_base.hpp"
#include "gtest/gtest.h"
#include "queues_absl_mutex.hpp"
#include "queues_atomic.hpp"
#include "queues_std_mutex.hpp"
#include "queues_synchronized.hpp"

namespace {

// 65536 pushes and pops by two producer threads and two consumer threads.
class MultiThread_2p2c_64k : public BenchmarkBase<1 << 16> {
 public:
  void SetUp(const ::benchmark::State& state) {}

  void TearDown(const ::benchmark::State& state) {}

  // should return kSum
  template <class Q>
  int64_t RunUnblocking() {
    std::unique_ptr<Q> q{new Q()};
    std::thread producer1([&] {
      for (int64_t i = 0; i < kSize / 2; ++i) {
        UnblockingQueueWaitPush(*q, payloads[i]);
      }
    });
    std::thread producer2([&] {
      for (int64_t i = kSize / 2; i < kSize; ++i) {
        UnblockingQueueWaitPush(*q, payloads[i]);
      }
    });
    int64_t sum1 = 0, sum2 = 0;
    std::thread consumer2([&] {
      int64_t sum = 0;
      for (int64_t i = 0; i < kSize / 2; ++i)
        sum += UnblockingQueueWaitPop(*q)->data;
      sum2 = sum;
    });
    // consumer1
    for (int64_t i = 0; i < kSize / 2; ++i)
      sum1 += UnblockingQueueWaitPop(*q)->data;
    producer1.join();
    producer2.join();
    consumer2.join();
    return sum1 + sum2;
  }

  // should return kSum
  template <class Q>
  int64_t RunBlocking() {
    std::unique_ptr<Q> q{new Q()};
    std::thread producer1([&] {
      for (int64_t i = 0; i < kSize / 2; ++i) {
        BlockingQueuePush(*q, payloads[i]);
      }
    });
    std::thread producer2([&] {
      for (int64_t i = kSize / 2; i < kSize; ++i) {
        BlockingQueuePush(*q, payloads[i]);
      }
    });
    int64_t sum1 = 0, sum2 = 0;
    std::thread consumer2([&] {
      int64_t sum = 0;
      for (int64_t i = 0; i < kSize / 2; ++i) sum += BlockingQueuePop(*q)->data;
      sum2 = sum;
    });
    // consumer1
    for (int64_t i = 0; i < kSize / 2; ++i) sum1 += BlockingQueuePop(*q)->data;
    producer1.join();
    producer2.join();
    consumer2.join();
    return sum1 + sum2;
  }
};

#define ADD_F_UNBLOCKING(T)                                     \
  BENCHMARK_F(MultiThread_2p2c_64k, T)(benchmark::State & st) { \
    for (auto _ : st) {                                         \
      int64_t result = RunUnblocking<T##Queue>();               \
      ASSERT_EQ(result, kSum);                                  \
    }                                                           \
  }

#define ADD_F_BLOCKING(T)                                       \
  BENCHMARK_F(MultiThread_2p2c_64k, T)(benchmark::State & st) { \
    for (auto _ : st) {                                         \
      int64_t result = RunBlocking<T##Queue>();                 \
      ASSERT_EQ(result, kSum);                                  \
    }                                                           \
  }

using StdMutexBuffer16BlockingMPMCQueue = StdMutexBufferBlockingMPMCQueue<16>;
using StdMutexBuffer4096BlockingMPMCQueue =
    StdMutexBufferBlockingMPMCQueue<4096>;

using StdMutexBuffer16UnblockingMPMCQueue =
    StdMutexBufferUnblockingMPMCQueue<16>;
using StdMutexBuffer4096UnblockingMPMCQueue =
    StdMutexBufferUnblockingMPMCQueue<4096>;

using AbslMutexBuffer16BlockingMPMCQueue = AbslMutexBufferBlockingMPMCQueue<16>;
using AbslMutexBuffer4096BlockingMPMCQueue =
    AbslMutexBufferBlockingMPMCQueue<4096>;

using AbslMutexBuffer16UnblockingMPMCQueue =
    AbslMutexBufferUnblockingMPMCQueue<16>;
using AbslMutexBuffer4096UnblockingMPMCQueue =
    AbslMutexBufferUnblockingMPMCQueue<4096>;

ADD_F_BLOCKING(StdMutexBuffer16BlockingMPMC);
ADD_F_BLOCKING(StdMutexBuffer4096BlockingMPMC);
ADD_F_BLOCKING(StdMutexNodeBlockingMPMC);
ADD_F_UNBLOCKING(StdMutexBuffer16UnblockingMPMC)
ADD_F_UNBLOCKING(StdMutexBuffer4096UnblockingMPMC)
ADD_F_UNBLOCKING(StdMutexNodeUnblockingMPMC);

ADD_F_BLOCKING(AbslMutexBuffer16BlockingMPMC)
ADD_F_BLOCKING(AbslMutexBuffer4096BlockingMPMC)
ADD_F_BLOCKING(AbslMutexNodeBlockingMPMC)
ADD_F_UNBLOCKING(AbslMutexBuffer16UnblockingMPMC)
ADD_F_UNBLOCKING(AbslMutexBuffer4096UnblockingMPMC)
ADD_F_UNBLOCKING(AbslMutexNodeUnblockingMPMC)

}  // namespace