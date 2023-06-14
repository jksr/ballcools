#include "timer.h"

Timer::Timer() : start_time(std::chrono::high_resolution_clock::now()) {
}

void Timer::reset(){
    start_time = std::chrono::high_resolution_clock::now();
}

double Timer::elapsed() const{
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start_time).count();
}
