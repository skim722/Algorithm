/*
Chromosome.cpp: This module contains the Chromosome class, which represents a bitfield representation of a vertex cover.  Furthermore, it contains variables that cache the result of a vertex cover query computation, such as number of edges not covered, number of bits that are flipped ON, etc.  Since the underlying representation is the bitfield, Chromosome is used by both GA and IMMC algorithms.
*/
#include "Chromosome.h"
#include <iostream>
#include <fstream>

using namespace std;


Chromosome Chromosome::full_cover(int size) {
    Chromosome chromosome;
    size = std::abs(size);
    chromosome.bitfield = vector<int>(size, 1);
    chromosome.filled_bits = size;
    chromosome.is_vc = true;
    chromosome.score = 1;
    return chromosome;
}

Chromosome::Chromosome(const vector<int> &_bitfield) {
    bitfield = _bitfield;
}

void Chromosome::calculate_score(const EdgeDenotedGraph &_graph) {
    auto tup                    = _graph.vertex_cover_check_using_bitfield(bitfield);
    auto _is_vc                 = std::get<0>(tup);
    auto num_vertices_used      = std::get<1>(tup);
    auto num_uncovered_edges    = std::get<2>(tup);

    /* The fitness function is as such:

        f(x) = \sum_{i=1}^{n} \bigg(
            x_i + n*(1 - x_i)*\sum_{j=1}^{n} (1 - x_j) e_ij
        \bigg)

        The first term in the sum penalizes on larger sizes of potential vertex cover (1 if vertex is used; 0 if not)
        The second term penalizes non-covered edges of potential vertex cover (add n for every edge not covered).

        The objective is to minimize f(x), or maximize 1/f(x)
    */
    filled_bits = num_vertices_used;
    is_vc = _is_vc;
    score = double(num_vertices_used) + double(num_uncovered_edges)*num_uncovered_edges*bitfield.size();
    score = bitfield.size() / score;
}

void Chromosome::update_with_bit_flip(const EdgeDenotedGraph &_graph, int bit_index) {
    bitfield[bit_index]         = (bitfield[bit_index] == 0) ? 1 : 0;
    auto tup                    = _graph.vertex_cover_check_using_bitfield(bitfield);
    is_vc                       = std::get<0>(tup);
    filled_bits                 = std::get<1>(tup);
}

void Chromosome::perform_crossover(Chromosome &other, int position) {
    for (int i=position; i < bitfield.size(); ++i) {
        std::swap(bitfield[i], other.bitfield[i]);
    }
}

void Chromosome::print() const {
    for (auto &bit : bitfield) cout << bit << " ";
    cout << endl;
}

void Chromosome::write_solution_to_file(const std::string &filepath) const {
    std::ofstream ofs(filepath.c_str(), std::ofstream::out);
    ofs.sync_with_stdio(false);

    ofs << filled_bits << "\n";
    for (auto i=0U; i < bitfield.size(); ++i) {
        if (bitfield[i] != 0) ofs << (i+1) << ",";
    } ofs << "\n";
}

bool Chromosome::operator==(const Chromosome &other) const {
    // Short-circuit the comparison operator by returning false if the scores or number of bits filled don't match.
    if (score != other.score) return false;
    if (filled_bits != other.filled_bits) return false;

    for (int i=0U; i < bitfield.size(); ++i) {
        if (bitfield[i] != other.bitfield[i]) return false;
    } return true;
}

bool Chromosome::operator<(const Chromosome &other) const {
    for (int i=0U; i < bitfield.size(); ++i) {
        if (bitfield[i] != other.bitfield[i]) {
            if (bitfield[i] < other.bitfield[i]) return true;
            else return false;
        }
    } return false;
}
