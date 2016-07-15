#ifndef CSE6140_RANDOM_
#define CSE6140_RANDOM_

#include <random>

class Random {
	int _seed = 0;
    std::mt19937 generator;
    std::uniform_real_distribution<double> dist;

  public:
    Random(int seed);
    Random() = default;
    int seed();
    double operator()();
    double operator()(double high);
    double operator()(double low, double high);
};

#endif
