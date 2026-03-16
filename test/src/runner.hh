#pragma once
#include <iostream>
#include <vector>
#include <numeric>

template<typename F>
double run_repeated(F&& fn, int repeat)
{
    std::vector<double> results;
    results.reserve(repeat);

    for(int i = 0; i < repeat; ++i) {
        results.push_back(fn());
    }

    double sum = std::accumulate(results.begin(), results.end(), 0.0);
    return sum / results.size();
}

inline void print_result(
    const std::string& name,
    const std::string& scenario,
    double ops,
    double seconds)
{
    double throughput;
    if (seconds == 0){
        throughput = 0;
    }else {
        throughput = ops / seconds / 1e6;
    }

    std::cout
        << name << ","
        << scenario << ","
        << throughput << " Mops/sec"
        << std::endl;
}