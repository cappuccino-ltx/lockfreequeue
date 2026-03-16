

#include <cassert>
#include <lockfree.hh>
#include <iostream>
#include <thread>
#include <memory>

void test_internal1(){
    lockfree::lfree_queue_spsc<int> q;
    std::thread producer([&q](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(i)){
                std::this_thread::yield();
                i--;
            }
        }
    });

    std::thread consumer([&q](){
        int data = 0;
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            assert(data == i);
        }
    });

    producer.join();
    consumer.join();
    assert(q.size_approx() == 0);
}
void test_internal2(){
    lockfree::lfree_queue_spsc<int*> q;
    std::thread producer([&q](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(reinterpret_cast<int*>(i))){
                std::this_thread::yield();
                i--;
            }
        }
    });

    std::thread consumer([&q](){
        int* data = 0;
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            assert((long long)data == i);
        }
    });

    producer.join();
    consumer.join();
    assert(q.size_approx() == 0);
}

struct temp1{
    int a;
};

void test_customize1(){
    lockfree::lfree_queue_spsc<temp1> q;
    std::thread producer([&q](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(temp1{i})){
                std::this_thread::yield();
                i--;
            }
        }
    });

    std::thread consumer([&q](){
        temp1 data;
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            assert(data.a == i);
        }
    });

    producer.join();
    consumer.join();
    assert(q.size_approx() == 0);
}


void test_customize2(){
    lockfree::lfree_queue_spsc<std::string> q;
    std::thread producer([&q](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(std::to_string(i))){
                std::this_thread::yield();
                i--;
            }
        }
    });

    std::thread consumer([&q](){
        std::string data;
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            assert(data == std::to_string(i));
        }
    });

    producer.join();
    consumer.join();
    assert(q.size_approx() == 0);
}
void test_customize3(){
    lockfree::lfree_queue_spsc<std::vector<int>> q;
    std::thread producer([&q](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(std::vector<int>{i})){
                std::this_thread::yield();
                i--;
            }
        }
    });

    std::thread consumer([&q](){
        std::vector<int> data;
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            assert(data[0] == i);
        }
    });

    producer.join();
    consumer.join();
    assert(q.size_approx() == 0);
}

struct NoDefault {
    int x;
    NoDefault(int v) : x(v) {}
};

void test_customize4(){
    lockfree::lfree_queue_spsc<NoDefault> q;
    std::thread producer([&q](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(NoDefault{i})){
                std::this_thread::yield();
                i--;
            }
        }
    });

    std::thread consumer([&q](){
        NoDefault data{ 0 };
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            assert(data.x == i);
        }
    });

    producer.join();
    consumer.join();
    assert(q.size_approx() == 0);
}
struct BigObject {
    char buf[4096];
};

void test_customize5(){
    lockfree::lfree_queue_spsc<BigObject> q{lockfree::queue_size::K01};
    std::thread producer([&q](){
        for(int i = 1; i <= 10000; i++) {
            BigObject data;
            *((int*)data.buf) = i;
            if(!q.try_put(data)){
                std::this_thread::yield();
                i--;
            }
        }
    });

    std::thread consumer([&q](){
        BigObject data{ 0 };
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            assert(*((int*)data.buf) == i);
        }
    });

    producer.join();
    consumer.join();
    assert(q.size_approx() == 0);
}

void test_shared(){
    lockfree::lfree_queue_spsc<std::shared_ptr<int>> q;
    std::thread producer([&q](){
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_put(std::make_shared<int>(i))){
                std::this_thread::yield();
                i--;
            }
        }
    });

    std::thread consumer([&q](){
        std::shared_ptr<int> data;
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            assert(*data == i);
        }
    });

    producer.join();
    consumer.join();
    assert(q.size_approx() == 0);
}

void test_unique(){
    lockfree::lfree_queue_spsc<std::unique_ptr<int>> q;
    std::thread producer([&q](){
        for(int i = 1; i <= 10000; i++) {
            std::unique_ptr<int> data(new int{i});
            if(!q.try_put(std::move(data))){
                std::this_thread::yield();
                i--;
            }
        }
    });

    std::thread consumer([&q](){
        std::unique_ptr<int> data;
        for(int i = 1; i <= 10000; i++) {
            if(!q.try_get(data)){
                std::this_thread::yield();
                i--;
                continue;
            }
            assert(*data == i);
        }
    });

    producer.join();
    consumer.join();
    assert(q.size_approx() == 0);
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