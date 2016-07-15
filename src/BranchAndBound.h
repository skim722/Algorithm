#ifndef BRANCH_AND_BOUND_H
#define BRANCH_AND_BOUND_H

#include "EdgeDenotedGraph.h"
#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <stack>
#include <stdexcept>
#include <set>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstddef>

class BranchAndBound 
{
private:
    bool _debug;
	std::string _fileloc;
    std::string _graph_name;
	EdgeDenotedGraph _graph;
    EdgeDenotedGraph _graph_approx;
	std::map<int, std::vector<int>> _cached_edges;
	
    std::set<int> _nodes_uncovered;
    double _current_best;
    int _node_count;
    time_t _start_time;
    int _max_seconds;
    double _approximation; 
    int _starting_node_idx;
    
    // int - node id
    // bool - whether to cover (true) or uncover (false)
    // bool - whether the state has been processed
    std::tuple<int,bool,bool> _current_node_state;
    
    // Node frontier states
    std::stack<std::tuple<int,bool,bool>> _node_states;
    
    // Cache of removed edges
    // int - node id visited when the edge was removed
    // int - node id neighboring the first 
    // int - edge id
    std::stack<std::tuple<int,int,int>> _edges_removed;
    
    // id of current node being processed
    int _current_node_idx;
    
    void run_approximation();
    
    void pop_current_node_state();
    void process_covered_state();
    void process_uncovered_state();
    
    void increment_current_node_idx();
    void decrement_current_node_idx();
    
    bool check_time();
	void start();
	
    void log_new_best(int value);
    void log_new_solution(int value);
    
    void split_filename();
    
	// test methods only
	void vectorTest();
	void cacheEdges(int vertex, std::vector<int> edges);
	void edgeTest();
    
public:
	BranchAndBound() = delete;
	BranchAndBound(const std::string &fileName);
	void init(bool debug, int max_seconds);
	void run();
};

#endif
