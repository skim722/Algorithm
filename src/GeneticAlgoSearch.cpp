/*
GeneticAlgoSearch.cpp: This module contains the routines for GA.  The routines should be well-commented
*/
#include "GeneticAlgoSearch.h"
#include "Utilities.h"
#include <algorithm>
#include <iostream>
#include <limits>

using namespace std;

GeneticAlgoSearch::GeneticAlgoSearch(const string &filepath) {
    _filepath = filepath;
    _graph = EdgeDenotedGraph(filepath);
}

void GeneticAlgoSearch::init(int population_size, int rand_seed) {
    init(population_size, 0.9, 1.5/_graph.num_vertices(), int(0.1 * 100), rand_seed);
}

void GeneticAlgoSearch::init(int population_size, double crossover_probability, double mutation_probability, int elite_size, int rand_seed) {
    _crossover_probability  = crossover_probability;
    _mutation_probability   = mutation_probability;
    _elite_size             = elite_size;
    _population_size        = population_size;
    ran3                    = Random(rand_seed);

    // Initialize population with full vertex-covers
    best_solution_found_so_far = Chromosome::full_cover(_graph.num_vertices());
    Population.clear(); Population.resize(population_size, best_solution_found_so_far);

    cout << "[ GENETIC ALGORITHM ]: Initializing with the following parameters:\n"
         << "filepath               = " << _filepath << "\n"
         << "rand_seed              = " << rand_seed << "\n"
         << "population_size        = " << population_size << "\n"
         << "crossover_probability  = " << crossover_probability << "\n"
         << "mutation_probability   = " << mutation_probability << "\n"
         << "elite_size             = " << elite_size << "\n" << endl;
}

void GeneticAlgoSearch::compute_fitness_scores_and_prune(int iteration) {
    // Compute and update chromosomes with scores
    for (auto &chromosome : Population) chromosome.calculate_score(_graph);

    if (iteration % 10000 == 0) {
        // Sort by chromosome bitfield equality, prune away duplicated, and replace *half* the pruned-away chromosomes with full-cover chromosomes
        std::sort(Population.begin(), Population.end());
        Population.erase( std::unique(Population.begin(), Population.end()), Population.end() );
        Population.resize(Population.size() + (_population_size - Population.size())/2, Chromosome::full_cover(_graph.num_vertices()));
    }

    // Sort by chromosome *score*, in descending order
    std::sort(Population.begin(), Population.end(), [](const Chromosome& lhs, const Chromosome& rhs) { return lhs.score > rhs.score; });
}

Chromosome* GeneticAlgoSearch::pick_parent(double total_fitness) {
    // Choose only from the non-elite chromosomes
    Chromosome *chromosome = nullptr;
    while (not chromosome or ran3() >= chromosome->score / total_fitness) {
        chromosome = &Population[ int( ran3(0, Population.size()) ) ];
    } return chromosome;
}

void GeneticAlgoSearch::generate_children(std::vector<Chromosome> &second_generation, double total_fitness) {
    // Generate either 2 children with crossovers (with crossover probability) or cloning one parent
    if (ran3() < _crossover_probability) {
        // Choose 2 unique non-elite parents
        auto parent1 = pick_parent(total_fitness); auto parent2 = parent1;
        while (parent2 == parent1) parent2 = pick_parent(total_fitness);

        // Add them in as children
        second_generation.emplace_back( *parent1 );
        second_generation.emplace_back( *parent2 );

        // Perform crossover
        second_generation.back().perform_crossover( second_generation[second_generation.size()-2], int(ran3(0, parent1->bitfield.size())) );
    } else {
        // Pick random parent and clone
        second_generation.emplace_back( *pick_parent(total_fitness) );
    }
}

void GeneticAlgoSearch::apply_mutations() {
    // Apply mutation to each bit in each chromosome with tiny mutation probability
    for (auto &chromosome : Population) {
        for (auto &bit : chromosome.bitfield) {
            if (ran3() < _mutation_probability) {
                bit = (bit == 0) ? 1 : 0;
            }
        }
    }
}

void GeneticAlgoSearch::find_best_valid_solution() {
    Chromosome *best = nullptr;
    for (auto &chromosome : Population) {
        if (chromosome.is_vc and
            ((best != nullptr and chromosome.filled_bits < best->filled_bits) or chromosome.filled_bits < best_solution_found_so_far.filled_bits)) {
            best = &chromosome;
        }
    }

    if (best) {
        best_solution_found_so_far = *best;
        tracefile << (stopwatch.elapsed_ms() / 1000.0) << "," << best_solution_found_so_far.filled_bits << endl;
    }
}

void GeneticAlgoSearch::cycle(int iteration) {
    // Compute/update fitness scores and sort the chromosomes in descending order of scores
    compute_fitness_scores_and_prune(iteration);

    // Find and record the best found solution
    find_best_valid_solution();

    // Compute total fitness
    double total_fitness = 0;
    for (auto i=0U; i < Population.size(); ++i) total_fitness += Population[i].score;

    // Elitist selection (save the top N solutions for next round)
    auto new_generation = decltype(Population)( Population.begin(), Population.begin() + _elite_size );

    // Generate children
    while (new_generation.size() < _population_size) {
        generate_children(new_generation, total_fitness);
    } new_generation.resize(Population.size());

    // Set as the new population
    Population = new_generation;

    // Apply mutations on the new generation
    apply_mutations();
}

int GeneticAlgoSearch::run_for_ms(double milliseconds, bool verbose) {
    milliseconds = std::abs(milliseconds);
    cout << "[ GENETIC ALGORITHM ]: Running algorithm for target " << (milliseconds/1000.0) << "s..." << endl;

    // Open tracefile
    tracefile = std::ofstream( generate_trace_filepath(_filepath, "GA", milliseconds/1000.0, ran3.seed()).c_str(), std::ofstream::out );
    tracefile.sync_with_stdio(false);

    //　Run GA
    stopwatch.reset(); auto iteration=0;
    while (++iteration, stopwatch.elapsed_ms() < milliseconds) {
        if (verbose) cout << "[ GENETIC ALGORITHM ] CYCLING (" << iteration << ")" << endl;
        cycle(iteration);
    }

    // Final scores update
    compute_fitness_scores_and_prune(iteration);
    find_best_valid_solution();
    cout << "[ GENETIC ALGORITHM ]: Finished running " << iteration << " cycles in approximately " << (stopwatch.elapsed_ms()/1000.0) << "s" << endl;

    auto solution_filepath = generate_solution_filepath(_filepath, "GA", milliseconds/1000.0, ran3.seed());
    cout << "[ GENETIC ALGORITHM ]: Writing best solution found to file '" << solution_filepath << "'" << endl;
    best_solution_found_so_far.write_solution_to_file(solution_filepath);

    return iteration;
}

std::vector<Chromosome> GeneticAlgoSearch::population() {
    return Population;
}

EdgeDenotedGraph GeneticAlgoSearch::graph() {
    return _graph;
}

void GeneticAlgoSearch::print_population() const {
    cout << "[ GENETIC ALGORITHM POPULATION ]\n";
    int count=0;
    for (auto &chromosome : Population) {
        cout << "[ID " << count++ << "]:  ";
        chromosome.print();
    } cout << endl;
}
