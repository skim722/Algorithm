#include "Utilities.h"
#include "GeneticAlgoSearch.h"
#include "IsingMCAlgoSearch.h"
#include "BranchAndBound.h"
#include "Stopwatch.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

void run_genetic_algorithm(const string &filepath, double cutoff_time_sec, int rand_seed, bool verbose=false) {
    auto population_size        = 100;
    GeneticAlgoSearch algorithm(filepath);
    algorithm.init(population_size, rand_seed);
    algorithm.run_for_ms(cutoff_time_sec*1000.0, verbose);
}

void run_ising_mc_algorithm(const string &filepath, double cutoff_time_sec, int rand_seed, bool verbose=false) {
    IsingMCAlgoSearch algorithm(filepath);
    algorithm.init(1, rand_seed);
    algorithm.run_for_ms(cutoff_time_sec*1000.0, verbose);
}

void run_bb_algorithm(const string &filepath, double cutoff_time_sec, bool verbose=false) {
    BranchAndBound algorithm(filepath);
    algorithm.init(verbose, cutoff_time_sec);
    algorithm.run();
}

int main(int argc, char** argv) {
    if (argc == 4 && strcmp(argv[2], "BB") == 0) { }
    else if (argc != 5) {
        cerr << "    Usage: " << argv[0] << " <input_file_path> <method> <time_cutoff_sec> <rand_seed>\n"
             << "    Methods allowed: [ GA, ISING, BB ]" << endl; std::exit(-1);
    }

    double cutoff_time_sec  = std::atof(argv[3]);
    
    int rand_seed;
    if(argc > 4) {
        rand_seed           = std::atoi(argv[4]);
    }

    if (strcmp(argv[2], "GA") == 0) {
        cout << "\n    Method 'GA' chosen\n" << endl;
        run_genetic_algorithm(argv[1], cutoff_time_sec, rand_seed, false);

    } else if (strcmp("ISING", argv[2]) == 0) {
        cout << "\n    Method 'ISING' chosen\n" << endl;
        run_ising_mc_algorithm(argv[1], cutoff_time_sec, rand_seed, false);
    } else if (strcmp("BB", argv[2]) == 0) {
        cout << "\n    Method 'BB' chosen\n" << endl;
        run_bb_algorithm(argv[1], cutoff_time_sec, false);
    } else {
        cout << "\n    Method '" << argv[2] << "' not implemented\n" << endl;
    }

    return 0;
}
