#pragma once
#include <chrono>

class Timer {
public:
    void start() {
        begin_ = clock::now();
    }

    double stop() {
        auto end = clock::now();
        return std::chrono::duration<double>(end - begin_).count();
    }

private:
    using clock = std::chrono::steady_clock;
    clock::time_point begin_;
};