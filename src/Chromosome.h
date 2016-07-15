#ifndef GENETIC_ALGO_CHROMOSOME_H_
#define GENETIC_ALGO_CHROMOSOME_H_

#include "EdgeDenotedGraph.h"
#include "Random.h"
#include <vector>

struct Chromosome {
	bool is_vc 		= false;
    double score 	= 0;
    int filled_bits = 0;
    std::vector<int> bitfield;

    Chromosome() = default;
    Chromosome(const std::vector<int> &_bitfield);
    void calculate_score(const EdgeDenotedGraph &graph);
    void update_with_bit_flip(const EdgeDenotedGraph &_graph, int bit_index);
    void perform_crossover(Chromosome &other, int position);
    void print() const;
    void write_solution_to_file(const std::string &filepath) const;
    bool operator==(const Chromosome &other) const;
    bool operator<(const Chromosome &other) const;

    static Chromosome full_cover(int size);
};

#endif
