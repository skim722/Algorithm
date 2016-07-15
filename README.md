# CSE6140 Final Project - Vertex Cover

## Software Requirements

A C++11 compiler is required to build the programs.  The recommended compilers are: GCC 5.1+ or Clang 3.4+  This project also requires CMake 2.8+ to build properly.  We have taken the necessary steps to make sure that the following build instructions will "just work" on either a Linux or Mac OS X machine.

## Build Instructions

To build the programs, simply run the following commands in the ROOT directory of the project:

    $ cmake .
    $ make

## Running the program

The following program will be built after running `cmake . && make`:

    local bm$ ./runGA
        Usage: ./runGA <input_file_path> <method> <time_cutoff_sec> <rand_seed>
        Methods allowed: [ GA, ISING, BB ]
    local bm$

In a similar way, to run approximation algorithm:

    local bm$ ./runApprox
        Usage: ./runApprox <input_file_path>
    local bm$

## Source Code Contents

All code is stored in `src/` folder.  Scripts for running the algorithms in batch and post-run analysis are stored in the root directory of the project

This section is a short description of the core modules:
* `RawMetisFile.cpp`: This module parses a metis file into RawMetisFile objects that contain a 2D vector of numbers.
* `EdgeDenotedGraph.cpp`: This module intakes RawMetisFile objects to construct EdgeDenotedGraphs, which contains edge-denoted representations of the graph (the incidence matrix).  EdgeDenotedGraph answers vertex cover queries, i.e. "is this set of vertices (by vertex ID) a vertex cover" in O(|V||E|) time.
* `Chromosome.cpp`: This module contains the Chromosome class, which represents a bitfield representation of a vertex cover.  Furthermore, it contains variables that cache the result of a vertex cover query computation, such as number of edges not covered, number of bits that are flipped ON, etc.  Since the underlying representation is the bitfield, Chromosome is used by both GA and IMMC algorithms.
* `GeneticAlgoSearch.cpp`: This module contains the routines for GA.  The routines should be well-commented
* `IsingMCAlgoSearch.cpp`: This module contains the routines for IMMC.  The routines should be well-commented
* `BranchAndBound.cpp`: This module contains the routines for Branch and Bound.


In addition, we have included some utility modules:
* `Utilities.cpp`: functions for generating output filenames given input filenames and parameters
* `Stopwatch.cpp`: a basic stopwatch to time runs
* `Random.cpp`: a basic random number generator that is initialized with a provided seed, and can generate either numbers in a range or between 0 and 1
# Algorithm
# Algorithm
