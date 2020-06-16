#include <cstdint>
#include <memory>

#include "benchmark/benchmark.h"
#include "benchmark_base.hpp"
#include "gtest/gtest.h"
#include "queues_absl_mutex.hpp"
#include "queues_atomic.hpp"
#include "queues_std_mutex.hpp"
#include "queues_synchronized.hpp"

namespace {

// 65536 Synchronized pushes and pops.
class SingleThread_64k : public BenchmarkBase<1 << 14> {
 public:
  void SetUp(const ::benchmark::State& state) {}

  void TearDown(const ::benchmark::State& state) {}

  // should return kSum * 4
  template <class Q>
  int64_t RunUnblocking() {
    std::unique_ptr<Q> q{new Q()};
    int64_t sum = 0;
    for (int64_t i = 0; i < kSize; ++i) {
      UnblockingQueueWaitPush(*q, payloads[i]);
      sum += UnblockingQueueWaitPop(*q)->data;
    }
    for (int64_t i = 0, j; i < kSize; ++i) {
      UnblockingQueueWaitPush(*q, payloads[j++ % kSize]);
      UnblockingQueueWaitPush(*q, payloads[j++ % kSize]);
      UnblockingQueueWaitPush(*q, payloads[j++ % kSize]);
      sum += UnblockingQueueWaitPop(*q)->data;
      sum += UnblockingQueueWaitPop(*q)->data;
      sum += UnblockingQueueWaitPop(*q)->data;
    }
    return sum;
  }

  // should return kSum * 4
  template <class Q>
  int64_t RunBlocking() {
    std::unique_ptr<Q> q{new Q()};
    int64_t sum = 0;
    for (int64_t i = 0; i < kSize; ++i) {
      BlockingQueuePush(*q, payloads[i]);
      sum += BlockingQueuePop(*q)->data;
    }
    for (int64_t i = 0, j; i < kSize; ++i) {
      BlockingQueuePush(*q, payloads[j++ % kSize]);
      BlockingQueuePush(*q, payloads[j++ % kSize]);
      BlockingQueuePush(*q, payloads[j++ % kSize]);
      sum += BlockingQueuePop(*q)->data;
      sum += BlockingQueuePop(*q)->data;
      sum += BlockingQueuePop(*q)->data;
    }
    return sum;
  }
};

#define ADD_F_UNBLOCKING(T)                                 \
  BENCHMARK_F(SingleThread_64k, T)(benchmark::State & st) { \
    for (auto _ : st) {                                     \
      int64_t result = RunUnblocking<T##Queue>();           \
      ASSERT_EQ(result, kSum * 4);                          \
    }                                                       \
  }

#define ADD_F_BLOCKING(T)                                   \
  BENCHMARK_F(SingleThread_64k, T)(benchmark::State & st) { \
    for (auto _ : st) {                                     \
      int64_t result = RunBlocking<T##Queue>();             \
      ASSERT_EQ(result, kSum * 4);                          \
    }                                                       \
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

using Buffer16SyncQueue = BufferSyncQueue<16>;
using Buffer4096SyncQueue = BufferSyncQueue<4096>;

ADD_F_UNBLOCKING(Buffer16Sync)
ADD_F_UNBLOCKING(Buffer4096Sync)
ADD_F_UNBLOCKING(StdDequeSync)
ADD_F_UNBLOCKING(StdListSync)
ADD_F_UNBLOCKING(NodeSync)

ADD_F_BLOCKING(StdMutexBuffer16BlockingMPMC)
ADD_F_BLOCKING(StdMutexBuffer4096BlockingMPMC)
ADD_F_BLOCKING(StdMutexNodeBlockingMPMC)
ADD_F_UNBLOCKING(StdMutexBuffer16UnblockingMPMC)
ADD_F_UNBLOCKING(StdMutexBuffer4096UnblockingMPMC)
ADD_F_UNBLOCKING(StdMutexNodeUnblockingMPMC)
ADD_F_UNBLOCKING(StdMutexNodeUnblockingMPSC)

ADD_F_BLOCKING(AbslMutexBuffer16BlockingMPMC)
ADD_F_BLOCKING(AbslMutexBuffer4096BlockingMPMC)
ADD_F_BLOCKING(AbslMutexNodeBlockingMPMC)
ADD_F_UNBLOCKING(AbslMutexBuffer16UnblockingMPMC)
ADD_F_UNBLOCKING(AbslMutexBuffer4096UnblockingMPMC)
ADD_F_UNBLOCKING(AbslMutexNodeUnblockingMPMC)
ADD_F_UNBLOCKING(AbslMutexNodeUnblockingMPSC)

ADD_F_BLOCKING(AtomicNodeBlockingMPSC)
ADD_F_BLOCKING(AtomicNodeV2BlockingMPSC)
ADD_F_BLOCKING(AtomicNodeV3BlockingMPSC)

}  // namespace