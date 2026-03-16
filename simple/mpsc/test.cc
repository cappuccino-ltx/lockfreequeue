

#include <cassert>
#include <lockfree.hh>
#include <iostream>
#include <thread>
#include <memory>

void test_internal1(){
    lockfree::lfree_queue_mpsc<int> q(2);
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
        for(int i = 1; i <= 20000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += data;
        }
    };

    std::thread consumer1(consumer_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
}
void test_internal2(){
    lockfree::lfree_queue_mpsc<int*> q(2);
    std::atomic<size_t> count_p { 0 };
    std::atomic<size_t> count_c { 0 };
    auto producer_back = [&q, &count_p](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(reinterpret_cast<int*>(i))){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_p += i;
        }
    };
    std::thread producer1(producer_back);
    std::thread producer2(producer_back);

    auto comuser_back = [&q, &count_c](){
        int* data = 0;
        for(int i = 1; i <= 20000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += (size_t)data;
        }
    };

    std::thread consumer1(comuser_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
}

struct temp1{
    int a;
};

void test_customize1(){
    lockfree::lfree_queue_mpsc<temp1> q(2);
    std::atomic<size_t> count_p { 0 };
    std::atomic<size_t> count_c { 0 };
    auto producer_back = [&q, &count_p](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(temp1{i})){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_p += i;
        }
    };
    std::thread producer1(producer_back);
    std::thread producer2(producer_back);

    auto comuser_back = [&q, &count_c](){
        temp1 data;
        for(int i = 1; i <= 20000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += data.a;
        }
    };

    std::thread consumer1(comuser_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
}


void test_customize2(){
    lockfree::lfree_queue_mpsc<std::string> q(2);
    std::atomic<size_t> count_p { 0 };
    std::atomic<size_t> count_c { 0 };
    auto producer_back = [&q, &count_p](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(std::to_string(i))){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_p += i;
        }
    };
    std::thread producer1(producer_back);
    std::thread producer2(producer_back);

    auto comuser_back = [&q, &count_c](){
        std::string data;
        for(int i = 1; i <= 20000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += std::stoi(data);
        }
    };

    std::thread consumer1(comuser_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
}
void test_customize3(){
    lockfree::lfree_queue_mpsc<std::vector<int>> q(2);
    std::atomic<size_t> count_p { 0 };
    std::atomic<size_t> count_c { 0 };
    auto producer_back = [&q, &count_p](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(std::vector<int>{i})){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_p += i;
        }
    };
    std::thread producer1(producer_back);
    std::thread producer2(producer_back);

    auto comuser_back = [&q, &count_c](){
        std::vector<int> data;
        for(int i = 1; i <= 20000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += data[0];
        }
    };

    std::thread consumer1(comuser_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
}

struct NoDefault {
    int x;
    NoDefault(int v) : x(v) {}
};

void test_customize4(){
    lockfree::lfree_queue_mpsc<NoDefault> q(2);
    std::atomic<size_t> count_p { 0 };
    std::atomic<size_t> count_c { 0 };
    auto producer_back = [&q, &count_p](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(NoDefault{i})){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_p += i;
        }
    };
    std::thread producer1(producer_back);
    std::thread producer2(producer_back);

    auto comuser_back = [&q, &count_c](){
        NoDefault data{ 0 };
        for(int i = 1; i <= 20000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += data.x;
        }
    };

    std::thread consumer1(comuser_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
}
struct BigObject {
    char buf[4096];
};

void test_customize5(){
    lockfree::lfree_queue_mpsc<BigObject> q(2,lockfree::K01);
    std::atomic<size_t> count_p { 0 };
    std::atomic<size_t> count_c { 0 };
    auto producer_back = [&q, &count_p](){
        for(int i = 1; i <= 10000; i++) {
            BigObject data;
            *((int*)data.buf) = i;
            if(!q.try_put(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_p += i;
        }
    };
    std::thread producer1(producer_back);
    std::thread producer2(producer_back);

    auto comuser_back = [&q, &count_c](){
        BigObject data;
        for(int i = 1; i <= 20000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += *((int*)data.buf);
        }
    };

    std::thread consumer1(comuser_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
}

void test_shared(){
    lockfree::lfree_queue_mpsc<std::shared_ptr<int>> q(2);
    std::atomic<size_t> count_p { 0 };
    std::atomic<size_t> count_c { 0 };
    auto producer_back = [&q, &count_p](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(std::make_shared<int>(i))){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_p += i;
        }
    };
    std::thread producer1(producer_back);
    std::thread producer2(producer_back);

    auto comuser_back = [&q, &count_c](){
        std::shared_ptr<int> data;
        for(int i = 1; i <= 20000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += *data;
        }
    };

    std::thread consumer1(comuser_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
}

void test_unique(){
    lockfree::lfree_queue_mpsc<std::unique_ptr<int>> q(2);
    std::atomic<size_t> count_p { 0 };
    std::atomic<size_t> count_c { 0 };
    auto producer_back = [&q, &count_p](){
        for(int i = 1; i <= 10000; i++) {
            std::unique_ptr<int> data(new int{i});
            if(!q.try_put(std::move(data))){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_p += i;
        }
    };
    std::thread producer1(producer_back);
    std::thread producer2(producer_back);

    auto comuser_back = [&q, &count_c](){
        std::unique_ptr<int> data;
        for(int i = 1; i <= 20000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            count_c += *data;
            data.reset();
        }
    };

    std::thread consumer1(comuser_back);

    producer1.join();
    producer2.join();
    consumer1.join();
    assert(q.size_approx() == 0);
    assert(count_p == count_c.load());
}

int main() {

    std::cout << "test begin" << std::endl;

    
    test_internal1();
    test_internal2();
    test_customize1();
    test_customize2();
    test_customize3();
    test_customize4();
    test_customize5();
    test_shared();
    test_unique();
    std::cout << "test end" << std::endl;

    return 0;
}