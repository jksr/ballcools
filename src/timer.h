#include <chrono>

class Timer
{
public:
    Timer();

    void reset();

    double elapsed() const;

private:
    std::chrono::high_resolution_clock::time_point start_time;
};
