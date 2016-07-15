/*
Random.cpp: a basic random number generator that is initialized with a provided seed, and can generate either numbers in a range or between 0 and 1
*/
#include "Random.h"
#include <random>

Random::Random(int seed) {
	_seed 		= seed;
    generator 	= std::mt19937(seed);
    dist 		= std::uniform_real_distribution<double>(0, 1);
}

int Random::seed() {
	return _seed;
}

double Random::operator()() {
    return dist(generator);
}

double Random::operator()(double high) {
    return dist(generator)*high;
}

double Random::operator()(double low, double high) {
    return dist(generator)*(high-low) + low;
}
