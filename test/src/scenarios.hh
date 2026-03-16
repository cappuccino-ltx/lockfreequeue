#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include "timer.hh"

template<typename Queue, typename Adapter, typename... Args>
double bench_spsc(size_t N, Args&&... args)
{
    Queue q(std::forward<Args>(args)...);

    std::atomic<bool> start{false};

    std::thread producer([&]{
        while(!start.load(std::memory_order_acquire)) {}

        for(size_t i = 0; i < N; ++i)
            Adapter::enqueue(q, i);
    });

    std::thread consumer([&]{
        int v;
        size_t count = 0;

        while(!start.load(std::memory_order_acquire)) {}

        while(count < N) {
            if(Adapter::dequeue(q, v))
                ++count;
        }
    });

    Timer timer;

    timer.start();
    start.store(true, std::memory_order_release);

    producer.join();
    consumer.join();

    return timer.stop();
}

template<typename Queue, typename Adapter, typename... Args>
double bench_mpmc(size_t N, int producers, int consumers, Args&&... args)
{
    Queue q(std::forward<Args>(args)...);

    std::atomic<bool> start{false};
    std::atomic<size_t> produced{0};
    std::atomic<size_t> consumed{0};

    std::vector<std::thread> threads;
    threads.reserve(producers + consumers);

    for(int i = 0; i < producers; ++i) {
        threads.emplace_back([&]{
            while(!start.load(std::memory_order_acquire)) {}

            size_t id;
            while((id = produced.fetch_add(1)) < N) {
                Adapter::enqueue(q, (int)id);
            }
        });
    }

    for(int i = 0; i < consumers; ++i) {
        threads.emplace_back([&]{
            int v;

            while(!start.load(std::memory_order_acquire)) {}

            while(consumed.load() < N) {
                if(Adapter::dequeue(q, v))
                    consumed.fetch_add(1);
            }
        });
    }

    Timer timer;

    timer.start();
    start.store(true, std::memory_order_release);

    for(auto& t : threads)
        t.join();

    return timer.stop();
}