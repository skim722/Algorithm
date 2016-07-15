#ifndef GENETIC_ALGO_SEARCH_H_
#define GENETIC_ALGO_SEARCH_H_

#include "EdgeDenotedGraph.h"
#include "Chromosome.h"
#include "Random.h"
#include "Stopwatch.h"
#include <vector>
#include <fstream>

class GeneticAlgoSearch {
private:
    Random                      ran3;
    Stopwatch                   stopwatch;
    std::ofstream               tracefile;
    std::string                 _filepath;
    EdgeDenotedGraph            _graph;
    std::vector<Chromosome>     Population;
    Chromosome                  best_solution_found_so_far;

    double  _crossover_probability  = 0;
    double  _mutation_probability   = 0;
    int     _elite_size             = 0;
    int     _population_size        = 0;

    void compute_fitness_scores_and_prune(int iteration);
    Chromosome* pick_parent(double total_fitness);
    void generate_children(std::vector<Chromosome> &second_generation, double total_fitness);
    void apply_mutations();
    void cycle(int iteration);
    void find_best_valid_solution();

public:
    GeneticAlgoSearch() = delete;
    GeneticAlgoSearch(const std::string &filepath);
    void init(int population_size, int rand_seed);
    void init(int population_size, double crossover_probability, double mutation_probability, int elite_size, int rand_seed);
    int run_for_ms(double milliseconds, bool verbose=false);
    EdgeDenotedGraph graph();
    std::vector<Chromosome> population();
    void print_population() const;
};

#endif
