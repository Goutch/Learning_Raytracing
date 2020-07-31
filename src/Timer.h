
#pragma once
#include <chrono>
#include <unordered_map>

#define SECONDS_TO_NANOSECOND 1000000000

#define SECOND_TO_MILISECOND 1000
class Timer {
    std::chrono::high_resolution_clock ::time_point start;

public:
    Timer();
    void reset();
    float ms() const;

    float ns() const;
};


