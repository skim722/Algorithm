#ifndef CSE6140_STOPWATCH_
#define CSE6140_STOPWATCH_

#include <chrono>

class Stopwatch {
    std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

  public:
  	void reset();
    double elapsed_ms();
};

#endif
