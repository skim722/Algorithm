#ifndef CSE6140_UTILITIES_
#define CSE6140_UTILITIES_

#include <string>

std::string generate_output_filepath(const std::string &input_filepath, const std::string &method, double cutoff_time_sec, bool trace, int rand_seed=0);
std::string generate_solution_filepath(const std::string &input_filepath, const std::string &method, double cutoff_time_sec, int rand_seed=0);
std::string generate_trace_filepath(const std::string &input_filepath, const std::string &method, double cutoff_time_sec, int rand_seed=0);

#endif
