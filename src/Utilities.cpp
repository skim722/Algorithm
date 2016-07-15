/*
Utilities.cpp: functions for generating output filenames given input filenames and parameters
*/
#include "Utilities.h"
#include <sstream>

using namespace std;

string generate_output_filepath(const std::string &input_filepath, const std::string &method, double cutoff_time_sec, bool trace, int rand_seed) {
    auto pos = input_filepath.find_last_of("/\\");
    stringstream ss; ss << input_filepath.substr( pos+1, input_filepath.find(".graph")-pos-1 )
                        << "_" << method << "_" << cutoff_time_sec;
    if (rand_seed != 0) ss << "_" << rand_seed;

    if (trace) ss << ".trace";
    else ss << ".sol";

    return ss.str();
}

string generate_solution_filepath(const string &input_filepath, const string &method, double cutoff_time_sec, int rand_seed) {
	return generate_output_filepath(input_filepath, method, cutoff_time_sec, false, rand_seed);
}

string generate_trace_filepath(const string &input_filepath, const string &method, double cutoff_time_sec, int rand_seed) {
	return generate_output_filepath(input_filepath, method, cutoff_time_sec, true, rand_seed);
}
