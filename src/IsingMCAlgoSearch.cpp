/*
IsingMCAlgoSearch.cpp: This module contains the routines for IMMC.  The routines should be well-commented
*/
#include "IsingMCAlgoSearch.h"
#include "Utilities.h"
#include <iostream>

using namespace std;

IsingMCAlgoSearch::IsingMCAlgoSearch(const std::string &filepath) {
    _filepath = filepath;
    _graph = EdgeDenotedGraph(filepath);
}


void IsingMCAlgoSearch::init(int num_systems, int rand_seed) {
    _num_systems    = num_systems;
    ran3            = Random(rand_seed);

    // Initialize systems as full vertex-covers
    best_solution_found_so_far = System::full_cover(_graph.num_vertices());
    Systems.clear(); Systems.resize(num_systems, best_solution_found_so_far);
}


void IsingMCAlgoSearch::cycle(int iteration) {
    for (auto &sys : Systems) {
        auto bit_index = int( ran3(_graph.num_vertices()) );
        auto marginal_edge_cost = _graph.incremental_edge_cost_of_bit_flip(sys.bitfield, bit_index);

        // The difference in energy is the sum of the marginal cost of flipping the bit, plus the difference of 1 vertex added/removed
        auto hamiltonian_diff = _constant_A * ((sys.bitfield[bit_index] == 0) ? 1.0 : -1.0 ) + _constant_B * double(marginal_edge_cost);

        /*
            Metropolis criterion - commence MC move with probability p = exp(-deltaE / kT)
            We short circuit the case where deltaE < 0, since exp(<positive number>) > 1
        */
        auto beta = 3.0; // the Boltzmann temperature factor
        if (hamiltonian_diff < 0 or ran3() < std::exp(-beta * hamiltonian_diff)) {
            sys.update_with_bit_flip(_graph, bit_index);
        }

        // If the new solution is the best, copy it to best_solution_found_so_far and log it in the trace file
        if (sys.is_vc and sys.filled_bits < best_solution_found_so_far.filled_bits) {
            best_solution_found_so_far = sys;
            tracefile << (stopwatch.elapsed_ms() / 1000.0) << "," << best_solution_found_so_far.filled_bits << endl;
        }
    }
}


int IsingMCAlgoSearch::run_for_ms(double milliseconds, bool verbose) {
    milliseconds = std::abs(milliseconds);
    cout << "[ ISING MC ALGORITHM ]: Running algorithm for target " << (milliseconds/1000.0) << "s..." << endl;

    // Open tracefile
    tracefile = std::ofstream( generate_trace_filepath(_filepath, "ISING", milliseconds/1000.0, ran3.seed()).c_str(), std::ofstream::out );
    tracefile.sync_with_stdio(false);

    //　Run ISING-MC
    stopwatch.reset(); auto iteration=0;
    while (++iteration, stopwatch.elapsed_ms() < milliseconds) {
        if (verbose) cout << "[ ISING MC ALGORITHM ] CYCLING (" << iteration << ")" << endl;
        cycle(iteration);
    }

    cout << "[ ISING MC ALGORITHM ]: Finished running " << iteration << " cycles in approximately " << (stopwatch.elapsed_ms()/1000.0) << "s" << endl;

    auto solution_filepath = generate_solution_filepath(_filepath, "ISING", milliseconds/1000.0, ran3.seed());
    cout << "[ ISING MC ALGORITHM ]: Writing best solution found to file '" << solution_filepath << "'" << endl;
    best_solution_found_so_far.write_solution_to_file(solution_filepath);

    return iteration;

}

EdgeDenotedGraph IsingMCAlgoSearch::graph() {
    return _graph;
}

std::vector<System> IsingMCAlgoSearch::systems() {
    return Systems;
}

void IsingMCAlgoSearch::print_systems() const {
    cout << "[ ISING SYSTEMS ]\n";
    int count=0;
    for (auto &sys : Systems) {
        cout << "[ID " << count++ << "]:  ";
        sys.print();
    } cout << endl;
}
