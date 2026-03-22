# Lock-Free Queue 

[🇨🇳 中文版 (Chinese)](./README_zh.md) | [🇺🇸 English](./README.md)

---

A high-performance, header-only lock-free queue implementation library for C++.

If you are in need of a lock-free queue and your use case is clearly defined (i.e., you know the exact number of data producers and consumers), you definitely want to take a look at this project!

## Features
- **Blazing Fast**: Using specific queues tailored for specific scenarios unlocks ultimate speed.
- **Tiny**: Header-only. Just drop the header file into your project and you're good to go.
- **Minimalist & Hackable**: The logic is completely transparent and straightforward. If you know your way around lock-free programming, you can easily tweak the source code to optimize it for your exact needs.
- **Fully Thread-Safe**: 100% lock-free queues.
- **Modern C++**: Requires only C++11 to run.
- **Zero Dependencies**: Pure C++, no third-party baggage.

## Core Functions

This project implements 6 specialized queues:
- `lfree_queue`: The most basic, general-purpose lock-free queue.
- `lfree_queue_spsc`: Exclusively for **Single-Producer Single-Consumer** scenarios.
- `lfree_queue_spmc`: Designed for **Single-Producer Multi-Consumer** scenarios.
- `lfree_queue_mpsc`: Designed for **Multi-Producer Single-Consumer** scenarios.
- `lfree_queue_mpmc`: Designed for **Multi-Producer Multi-Consumer** scenarios.


## Queue Design

The standard `lockfree_queue` is a ring buffer that relies on the CAS (Compare-And-Swap) concept to achieve lock-free access. However, when the crowd of producers and consumers gets too large, CAS contention turns into a bloodbath. Only one thread can win the CAS lottery at a time, leading to a noticeable drop in performance. (The chart below shows the average benchmark results for producing/consuming 5,000,000 items, looped 5 times under varying thread counts).

![lfree_queue performance comparison](test/result/plots/5000000/5000000_lfree_queue.png)

This sparked an idea: why not design specialized queues separately for different scenarios? If the battlefield is predictable, the design becomes a breeze. Of course, these scenario-specific queues must be used sensibly. If you don't honor the exact producer and consumer counts you promised upon creation, the program will mercilessly `assert` and terminate.

### SPSC
For `lfree_queue_spsc`, we only need to ensure the operations are atomic. There's no need for same-side CAS contention at all.

### MPSC
The design of `lfree_queue_mpsc` allocates one dedicated queue for each producer. The consumer simply polls them to consume. Under the hood, it is essentially N `spsc` queues, and that's exactly how I implemented it.

### SPMC
For `lfree_queue_spmc`, my design philosophy is to assign one dedicated queue to each consumer. When consuming, they only pull from their own turf.

Initially, I planned to reuse the `spsc` queue here too. But thinking ahead about potentially adding a consumer "work-stealing" feature later, I decided to implement the most primitive lock-free queue (which became `lfree_queue`) and reused it inside `lfree_queue_spmc`. This way, we don't need CAS operations *on* the `lfree_queue_spmc` itself. Since each consumer has a 1-to-1 simple lock-free queue, having multiple consumers actually disperses the CAS pressure of a single `lfree_queue`, thereby boosting performance.

I did toy with the idea of designing work-stealing from the get-go. But since I was just starting, I figured I'd write a complete functional queue first and optimize later. *Plot twist:* during testing, I realized that without work-stealing, if a consumer goes AWOL (exits or stops coming to consume), the tasks stuck in its exclusive queue would gather dust forever! So, I slapped on a simple work-stealing patch. My thought process is that, under normal circumstances, you don't want excessive work-stealing anyway, as it drags down performance.

### MPMC
The `lfree_queue_mpmc` is also based on the "one queue per producer" ideology. Consumers poll the producers' queues to get their data.

So, I chose `lfree_queue_spmc` to act as the producer's internal queue. Why this specific design? Well, this was the exact approach I used in a very old version of `lfree_queue` when I was writing my own logging system... haha, I digress! Basically, this idea was born a year ago, but my code back then was pretty messy. Recently, I finally had some free time and wanted to refactor my lock-free queues—hence, this project was born.

```txt
        consumer    consumer    consumer    consumer
            \           |         |         /
               \        |         |      /
                    lfree_queue_mpmc
    {          /                        \                  }
    {       /                                \             }
    {    lfree_queue_spmc  --        -- lfree_queue_spmc   }
    {   {       |         } |        | {       |         } }
    {   {   lfree_queue   } |        | {   lfree_queue   } }
    ========================|========|======================
            producer   <-----        ----->  producer
```

The producer inserts items round-robin into the N consumer sub-queues *within its own queue*. Consumers pull exclusively from their designated slots assigned by the producers. Result? Both producers and consumers aggressively avoid synchronization contention.

*The Catch*: This design spawns *a lot* of queues. Producers have queues, and consumers have sub-queues inside every producer's queue. Therefore, when using this specific queue, I highly recommend keeping the specified queue length as small as reasonably possible.

### `concurrent_queue`

The performance of the previous four exclusive scenario queues was stable in their respective scenarios, but there were certain limitations. If consumers and producers are uncertain in some scenarios, the above queues cannot be used

For the lfree_queue queue, although it can be used in situations where the number of producers and consumers is uncertain, it is only limited to situations where the quantity is relatively small. If the quantity increases, its performance will become very poor

So 'concurrent queue' was born, which is also a reuse of 'lfree_queue', but the design concept is completely different from the above queues
In the mpsc, spmc, and mpmc queues, the way to improve the success rate of cas operations is to give each thread a local queue. However, when designing the concurrent queue, the original intention was to improve performance without relying on the number of threads, so the approach of having a local queue for each thread cannot be adopted  

So in the design of 'concurrent queue', the strategy of 'segmented cas' is adopted. I replaced each' segment 'with' lfree_queue ', and for each segment, if you want to produce or consume in this segment, you must first compete for the right to use this segment  

It is worth noting that in the competition of segments, it is not strict competition, but rather a relatively relaxed rule, which may result in multiple threads still running cas on the same segment. Therefore, the number of segments N is quite important. If the size of N is set appropriately, it can greatly reduce the success rate of cas on the same segment, but it cannot be too large

In implementation, the size of N is the number of cores in the machine, but you can pass parameters to it when constructing the queue to set the number of segments. In my preliminary testing process, I found that if the number of segments is 2 to 3 times that of the access thread, the performance is optimal. During use, actual testing should prevail. If any colleagues test the best value of N, they can also give feedback

## API Design

All six defined queues share the exact same interface:

```C++
construction(/*mp*/int producer_n, /*mc*/int consumer_n, size_t queue_size);
template<typename U>
bool try_put(U&& data);
bool try_get(T& data);
size_t size_approx();
```

Usage examples for each queue can be found in the `simple/` directory. You can build and run them like this:
```bash
cd simple; mkdir build; cd build;
cmake ..; make
./queue/queue_test
./spsc/spsc_test
./spmc/spmc_test
./mpsc/mpsc_test
./mpmc/mpmc_test
```

`lfree_queue_mpmc` Usage Example:

```C++
#include <lockfree.hh>

int main() {
    lockfree::lfree_queue_mpmc<int> q(2,2);
    std::atomic<size_t> count_p { 0 };
    std::atomic<size_t> count_c { 0 };
    auto producer_back = [&q,&count_p](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(i)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_p += i;
        }
    };
    std::thread producer1(producer_back);
    std::thread producer2(producer_back);
    auto consumer_back = [&q,&count_c](){
        int data = 0;
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += data;
        }
    };

    std::thread consumer1(consumer_back);
    std::thread consumer2(consumer_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    consumer2.join();
    
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
    return 0;
}
```

## Performance Benchmark

### Environment

For the performance benchmarks, I ran the tests on my trusty little mini-PC. Here are the specs:

- **OS**: Ubuntu 22.04 LTS
- **RAM**: 16 GB
- **CPU**: 12 Cores

Since this is a performance test, and the project is built around specialized lock-free queues for different scenarios, I ran tests across the following configurations:

- 1P1C
- 1P4C
- 1P8C
- 4P1C
- 8P1C
- 2P2C
- 4P4C
- 8P8C

The queues participating in the showdown are listed below. I brought in `cameron314/concurrentqueue`—an industrial-grade C++ lock-free queue library—with a learning mindset, just to see how my queues stack up against the boss:

- `lfree_queue_spsc`
- `lfree_queue_spmc`
- `lfree_queue_mpsc`
- `lfree_queue_mpmc`
- `lfree_queue`
- `concurrent_queue`
- `cameron314/concurrentqueue`

**Test Conditions:**

Produce and consume `5,000,000` items, record the time. Repeat 5 times and take the average.


### Test Results

I've uploaded all my raw test data to the project in the [Test Results Directory](./test/result/), and converted them into tables and line charts.

This document only highlights a few charts. If you're interested in the deep dive, feel free to check out the [Plot Results Directory](./test/result/plots/).

1P1C Performance Comparison

![1p1c](./test/result/plots/5000000/comparison_1P1C.png)

~4P4C Performance Comparison

![4p4c](./test/result/plots/5000000/comparison_4P4C.png)

~8P8C Performance Comparison

![8p8c](./test/result/plots/5000000/comparison_8P8C.png)

Aggregate Summary

![all](./test/result/plots/5000000/all_queues.png)

### Benchmark Notes
* **Baseline Target**: `ConcurrentQueue` (Developed by [cameron314](https://github.com/cameron314))
* **Methodology**: Unified the interfaces using [`src/adapter.hh`](./src/adapter.hh) and executed stress tests under identical hardware conditions. *(Note: Fixed the adapter path link here for you!)*
* **Data Source**: All raw comparison data is logged in the [`test/result/`](./test/result/) directory.

## Acknowledgements

This project utilized the following outstanding open-source library during testing:

* **[cameron314/concurrentqueue](https://github.com/cameron314/concurrentqueue)**: An industry-leading, high-performance lock-free queue implementation. This project used it as the ultimate performance benchmark.

## License

This project is licensed under the [Apache License 2.0](./LICENSE).