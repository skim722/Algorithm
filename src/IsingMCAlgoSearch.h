#ifndef ISING_MC_ALGO_SEARCH_H_
#define ISING_MC_ALGO_SEARCH_H_

#include "EdgeDenotedGraph.h"
#include "Chromosome.h"
#include "Random.h"
#include "Stopwatch.h"
#include <vector>
#include <fstream>

typedef Chromosome System;

class IsingMCAlgoSearch {
private:
    Random                      ran3;
    Stopwatch                   stopwatch;
    std::ofstream               tracefile;
    std::string                 _filepath;
    EdgeDenotedGraph            _graph;
    std::vector<System>         Systems;
    std::vector<double>         Temperatures;
    System                      best_solution_found_so_far;

    int     _num_systems        = 0;
    double  _constant_A         = 1;
    double  _constant_B         = 100;

    void cycle(int iteration);

public:
    IsingMCAlgoSearch() = delete;
    IsingMCAlgoSearch(const std::string &filepath);
    void init(int num_systems, int rand_seed);
    int run_for_ms(double milliseconds, bool verbose=false);
    EdgeDenotedGraph graph();
    std::vector<Chromosome> systems();
    void print_systems() const;
};

#endif
