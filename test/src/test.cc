

#include <lockfree.hh>
#include "concurrentqueue.h"

#include "adapter.hh"
#include "runner.hh"
#include "scenarios.hh"

const size_t N = 50000000;
const int repeat = 5;

void test_mpmc(int p, int c){

    char scenario[10] = { 0 };
    snprintf(scenario,sizeof(scenario) - 1, "%dP%dC", p, c);
    if (p == 1 && c == 1){
        auto time = run_repeated([&]{
            return bench_mpmc<lockfree::lfree_queue_spsc<int>, LFreeQueueAdapter<lockfree::lfree_queue_spsc<int>>>(N,p,c);
        }, repeat);
        print_result("lfree_queue_spsc", scenario, N, time);
    }else {
        print_result("lfree_queue_spsc", scenario, N, 0);
    }
    if (p == 1){
        auto time = run_repeated([&]{
            return bench_mpmc<lockfree::lfree_queue_spmc<int>, LFreeQueueAdapter<lockfree::lfree_queue_spmc<int>>>(N, p, c, c);
        }, repeat);
        print_result("lfree_queue_spmc", scenario, N, time);
    }else {
        print_result("lfree_queue_spmc", scenario, N, 0);
    }
    if(c == 1){
        auto time = run_repeated([&]{
            return bench_mpmc<lockfree::lfree_queue_mpsc<int>, LFreeQueueAdapter<lockfree::lfree_queue_mpsc<int>>>(N, p, c, p);
        }, repeat);
        print_result("lfree_queue_mpsc", scenario, N, time);
    }else {
        print_result("lfree_queue_mpsc", scenario, N, 0);
    }
    {
        auto time = run_repeated([&]{
            return bench_mpmc<lockfree::lfree_queue_mpmc<int>, LFreeQueueAdapter<lockfree::lfree_queue_mpmc<int>>>(N, p, c, p, c);
        }, repeat);
        print_result("lfree_queue_mpmc", scenario, N, time);
    }
    {
        auto time = run_repeated([&]{
            return bench_mpmc<lockfree::lfree_queue<int>, LFreeQueueAdapter<lockfree::lfree_queue<int>>>(N, p, c);
        }, repeat);
        print_result("lfree_queue", scenario, N, time);
    }
    {
        auto time = run_repeated([&]{
            return bench_mpmc<moodycamel::ConcurrentQueue<int>, ConcurrentQueueAdapter<moodycamel::ConcurrentQueue<int>>>(N, p, c);
        }, repeat);
        print_result("ConcurrentQueue", scenario, N, time);
    }
}


int main() {
    std::cout << "queue,scenario,throughput\n";

    test_mpmc(1,1);
    test_mpmc(1,4);
    test_mpmc(1,8);
    test_mpmc(4,1);
    test_mpmc(8,1);
    test_mpmc(2,2);
    test_mpmc(4,4);
    test_mpmc(8,8);
    return 0;
}