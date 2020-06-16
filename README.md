# queues
 Implement queues in different ways and do benchmarks.

# Windows x86_64

```
Run on (8 X 4008 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)

bazel run -c opt :benchmark_sync
---------------------------------------------------------------------------------------------
Benchmark                                                   Time             CPU   Iterations
---------------------------------------------------------------------------------------------
SingleThread_64k/Buffer16Sync                          163274 ns       161122 ns         4073
SingleThread_64k/Buffer4096Sync                        167201 ns       167411 ns         4480
SingleThread_64k/StdDequeSync                          345216 ns       345346 ns         2036
SingleThread_64k/StdListSync                          3140331 ns      3138951 ns          224
SingleThread_64k/NodeSync                              165633 ns       167411 ns         4480
SingleThread_64k/StdMutexBuffer16BlockingMPMC         2892534 ns      2886546 ns          249
SingleThread_64k/StdMutexBuffer4096BlockingMPMC       3287247 ns      3310381 ns          236
SingleThread_64k/StdMutexNodeBlockingMPMC             3134725 ns      3138951 ns          224
SingleThread_64k/StdMutexBuffer16UnblockingMPMC       3168744 ns      3180804 ns          280
SingleThread_64k/StdMutexBuffer4096UnblockingMPMC     2715003 ns      2722538 ns          264
SingleThread_64k/StdMutexNodeUnblockingMPMC           3234593 ns      3244174 ns          236
SingleThread_64k/StdMutexNodeUnblockingMPSC           3144769 ns      3136837 ns          264
SingleThread_64k/AbslMutexBuffer16BlockingMPMC        2112432 ns      2099609 ns          320
SingleThread_64k/AbslMutexBuffer4096BlockingMPMC      2105457 ns      2128623 ns          345
SingleThread_64k/AbslMutexNodeBlockingMPMC            2356083 ns      2351589 ns          299
SingleThread_64k/AbslMutexBuffer16UnblockingMPMC      2051340 ns      2038043 ns          345
SingleThread_64k/AbslMutexBuffer4096UnblockingMPMC    2058973 ns      2038043 ns          345
SingleThread_64k/AbslMutexNodeUnblockingMPMC          2378448 ns      2351589 ns          299
SingleThread_64k/AbslMutexNodeUnblockingMPSC          1677269 ns      1674107 ns          448
SingleThread_64k/AtomicNodeBlockingMPSC                974525 ns       976563 ns          640
SingleThread_64k/AtomicNodeV2BlockingMPSC             1005617 ns      1000977 ns          640
SingleThread_64k/AtomicNodeV3BlockingMPSC             1095842 ns      1098633 ns          640

bazel run -c opt :benchmark_1p1c
-------------------------------------------------------------------------------------------------
Benchmark                                                       Time             CPU   Iterations
-------------------------------------------------------------------------------------------------
MultiThread_1p1c_64k/StdMutexBuffer16BlockingMPMC        12909520 ns     12834821 ns           56
MultiThread_1p1c_64k/StdMutexBuffer4096BlockingMPMC       5750778 ns      5781250 ns          100
MultiThread_1p1c_64k/StdMutexNodeBlockingMPMC             8506884 ns      8333333 ns           90
MultiThread_1p1c_64k/StdMutexBuffer16UnblockingMPMC      67396182 ns     66761364 ns           11
MultiThread_1p1c_64k/StdMutexBuffer4096UnblockingMPMC    10330600 ns     10416667 ns           75
MultiThread_1p1c_64k/StdMutexNodeUnblockingMPMC          17611076 ns     17530488 ns           41
MultiThread_1p1c_64k/StdMutexNodeUnblockingMPSC           4911537 ns      4743304 ns          112
MultiThread_1p1c_64k/AbslMutexBuffer16BlockingMPMC       12677481 ns     12695313 ns           64
MultiThread_1p1c_64k/AbslMutexBuffer4096BlockingMPMC     11840604 ns     11997768 ns           56
MultiThread_1p1c_64k/AbslMutexNodeBlockingMPMC           14890880 ns     15066964 ns           56
MultiThread_1p1c_64k/AbslMutexBuffer16UnblockingMPMC     24294260 ns     24375000 ns           25
MultiThread_1p1c_64k/AbslMutexBuffer4096UnblockingMPMC   25586245 ns     25568182 ns           22
MultiThread_1p1c_64k/AbslMutexNodeUnblockingMPMC         14585987 ns     14583333 ns           45
MultiThread_1p1c_64k/AbslMutexNodeUnblockingMPSC          9016149 ns      8750000 ns           75
MultiThread_1p1c_64k/AtomicNodeBlockingMPSC               1566539 ns      1569475 ns          448
MultiThread_1p1c_64k/AtomicNodeV2BlockingMPSC             1582592 ns      1569475 ns          448
MultiThread_1p1c_64k/AtomicNodeV3BlockingMPSC             1573508 ns      1569475 ns          448

bazel run -c opt :benchmark_2p1c
-------------------------------------------------------------------------------------------------
Benchmark                                                       Time             CPU   Iterations
-------------------------------------------------------------------------------------------------
MultiThread_2p1c_64k/StdMutexBuffer16BlockingMPMC        15663764 ns     15972222 ns           45
MultiThread_2p1c_64k/StdMutexBuffer4096BlockingMPMC       8205939 ns      8159722 ns           90
MultiThread_2p1c_64k/StdMutexNodeBlockingMPMC             8269805 ns      7916667 ns           75
MultiThread_2p1c_64k/StdMutexBuffer16UnblockingMPMC     101642614 ns    100446429 ns            7
MultiThread_2p1c_64k/StdMutexBuffer4096UnblockingMPMC    20678240 ns     20833333 ns           45
MultiThread_2p1c_64k/StdMutexNodeUnblockingMPMC          12121886 ns     12187500 ns           50
MultiThread_2p1c_64k/StdMutexNodeUnblockingMPSC           5200014 ns      5161830 ns          112
MultiThread_2p1c_64k/AbslMutexBuffer16BlockingMPMC       20034341 ns     20220588 ns           34
MultiThread_2p1c_64k/AbslMutexBuffer4096BlockingMPMC     19801995 ns     19847973 ns           37
MultiThread_2p1c_64k/AbslMutexNodeBlockingMPMC           17018132 ns     17149390 ns           41
MultiThread_2p1c_64k/AbslMutexBuffer16UnblockingMPMC     33486955 ns     34090909 ns           22
MultiThread_2p1c_64k/AbslMutexBuffer4096UnblockingMPMC   23468033 ns     23437500 ns           30
MultiThread_2p1c_64k/AbslMutexNodeUnblockingMPMC         14063694 ns     14062500 ns           50
MultiThread_2p1c_64k/AbslMutexNodeUnblockingMPSC         10336767 ns     10253906 ns           64
MultiThread_2p1c_64k/AtomicNodeBlockingMPSC               2490235 ns      2343750 ns          280
MultiThread_2p1c_64k/AtomicNodeV2BlockingMPSC             1830238 ns      1842752 ns          407
MultiThread_2p1c_64k/AtomicNodeV3BlockingMPSC             1879668 ns      1801273 ns          373

bazel run -c opt :benchmark_2p2c
-------------------------------------------------------------------------------------------------
Benchmark                                                       Time             CPU   Iterations
-------------------------------------------------------------------------------------------------
MultiThread_2p2c_64k/StdMutexBuffer16BlockingMPMC        17247993 ns     16387195 ns           41
MultiThread_2p2c_64k/StdMutexBuffer4096BlockingMPMC       7876133 ns      7465278 ns           90
MultiThread_2p2c_64k/StdMutexNodeBlockingMPMC            10966522 ns     10986328 ns           64
MultiThread_2p2c_64k/StdMutexBuffer16UnblockingMPMC      68450073 ns     56818182 ns           11
MultiThread_2p2c_64k/StdMutexBuffer4096UnblockingMPMC    15464414 ns     13916016 ns           64
MultiThread_2p2c_64k/StdMutexNodeUnblockingMPMC          19984507 ns     17708333 ns           45
MultiThread_2p2c_64k/AbslMutexBuffer16BlockingMPMC       22186010 ns     21722561 ns           41
MultiThread_2p2c_64k/AbslMutexBuffer4096BlockingMPMC     17212323 ns     16183036 ns           56
MultiThread_2p2c_64k/AbslMutexNodeBlockingMPMC           20829216 ns     19042969 ns           32
MultiThread_2p2c_64k/AbslMutexBuffer16UnblockingMPMC     29130488 ns     27113971 ns           34
MultiThread_2p2c_64k/AbslMutexBuffer4096UnblockingMPMC   22317900 ns     20270270 ns           37
MultiThread_2p2c_64k/AbslMutexNodeUnblockingMPMC         26780127 ns     25000000 ns           30
```