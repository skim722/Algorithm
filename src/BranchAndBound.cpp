/*
BranchAndBound.cpp: This module contains the routines for Branch and Bound.
*/
#include "BranchAndBound.h"

using namespace std;

// Perform instantiation logic
BranchAndBound::BranchAndBound(const string &fileName)
{
	_fileloc = fileName;
    split_filename();
	_graph = EdgeDenotedGraph(_fileloc);
    _graph_approx = EdgeDenotedGraph(_fileloc);
}

// Perform initialization logic
void BranchAndBound::init(bool debug, int max_seconds)
{
	_debug = debug;
    _max_seconds = max_seconds;
	cout << "[ Branch and Bound ]: Initializing with the following parameters:" << endl;
    cout << "filepath    = " << _fileloc << endl;
    cout << "time limit  = " << max_seconds << "(s)" << endl;
}

// Begin running the algorithm
void BranchAndBound::run()
{
    run_approximation();
    _start_time = time(0);
	return start();
}

// Get bound approximation
void BranchAndBound::run_approximation()
{
    int node_idx = 0;
    int neighbor_idx;
    int iterations = 0;

    while(_graph_approx.num_edges() > 0)
    {
        std::vector<int> edges = _graph_approx.connected_edges(node_idx);
        if(edges.size() > 0)
        {
            std::pair<int,int> edge_nodes = _graph_approx.get_edge_nodes(edges.at(0));
            neighbor_idx = edge_nodes.first == node_idx ? edge_nodes.second : edge_nodes.first;

            for(auto edge_it = edges.begin(); edge_it != edges.end(); edge_it++)
            {
                _graph_approx.remove_edge(*edge_it);
            }

            std::vector<int>edges_neighbor = _graph_approx.connected_edges(neighbor_idx);
            for(auto edge_it = edges_neighbor.begin(); edge_it != edges_neighbor.end(); edge_it++)
            {
                _graph_approx.remove_edge(*edge_it);
            }
            iterations++;
        }
        node_idx++;
    }

    _approximation = iterations * 2;
    if(_debug)
    {
        cout << "_approximation: " << _approximation << ", nodes: " << _graph_approx.num_vertices() <<   endl;
    }
}

void BranchAndBound::start()
{
    _node_count = _graph.num_vertices();
    _current_best = std::numeric_limits<double>::infinity();

    // Get the first value to process
    srand(time(0));
    _current_node_idx = rand() % _node_count;
    _starting_node_idx = _current_node_idx;

    _node_states.push(std::tuple<int, bool, bool>(_current_node_idx, true, false));

    if(_debug)
    {
        cout << "+ [node: " << _current_node_idx << ", cover: " << true << "]" << endl;
    }

    _node_states.push(std::tuple<int, bool, bool>(_current_node_idx, false, false));

    if(_debug)
    {
        cout << "+ [node: " << _current_node_idx << ", cover: " << false << "]" << endl;
    }

    // Process each node state on the queue
    while(!_node_states.empty() && check_time())
    {
        // check out the current top node
        _current_node_state = _node_states.top();
        _current_node_idx = std::get<0>(_current_node_state);

        if(_debug)
        {
            cout << "@ [node: " << std::get<0>(_current_node_state) << ", cover: "
                << std::get<1>(_current_node_state) << "]" << endl;
        }

        // we are backtracking to this node; no further computation necessary
        if(std::get<2>(_current_node_state))
        {
            pop_current_node_state();
            continue;
        }

        // set the current node state as processed
        std::get<2>(_current_node_state) = true;
        _node_states.top() = _current_node_state;

        // -------- Bound -------------

        // Handle the covered node state
        if(std::get<1>(_current_node_state))
        {
            process_covered_state();
        }

        // Handle the uncovered node state
        if(!std::get<1>(_current_node_state))
        {
            process_uncovered_state();
        }

        // ---------- [Solutions/Dead Ends] ----------

        int unprocessed;
        if(_current_node_idx >= _starting_node_idx)
        {
            unprocessed = _node_count - 1 - (_current_node_idx - _starting_node_idx);
        }
        else
        {
            unprocessed = _starting_node_idx - _current_node_idx - 1;
        }

        // check whether all necessary nodes are covered
        if(_graph.num_edges() == 0)
        {
            if(_current_best > (_node_count - _nodes_uncovered.size() - unprocessed))
            {
                _current_best = _node_count - _nodes_uncovered.size() - unprocessed;
                log_new_best(_current_best);
                cout << "! new best: " << _current_best << endl;
            }
            pop_current_node_state();
            continue;
        }

        // already worse than current best solution
        if(_node_count - _nodes_uncovered.size() - unprocessed > _current_best)
        {
            pop_current_node_state();
            continue;
        }

        // worse than (dynamically calculated) upper bound
        if(_node_count - _nodes_uncovered.size() - unprocessed > _approximation)
        {
            pop_current_node_state();
            continue;
        }

        // ---------- Branch ----------

        // Get next node to process
        increment_current_node_idx();

        // [todo] : check if the NEW node has uncovered neighbors
        // check if node has an uncovered neighbor
        bool has_uncovered_neighbor = false;
        std::set<int>::iterator it_uncovered;

        std::vector<int> edges = _graph.connected_edges(_current_node_idx);
        for(std::vector<int>::iterator edge_it = edges.begin(); edge_it != edges.end(); edge_it++)
        {
            // get the node's neighbor
            std::pair<int,int> edge_nodes = _graph.get_edge_nodes(*edge_it);
            int neighbor_idx = edge_nodes.first == _current_node_idx ? edge_nodes.second : edge_nodes.first;

            // check whether it is uncovered
            std::set<int>::iterator it_uncovered = _nodes_uncovered.find(neighbor_idx);

            // if an uncovered neighbor is found
            if(!(it_uncovered == _nodes_uncovered.end()))
            {
                has_uncovered_neighbor = true;
                break;
            }
        }

        // push covered node state
        _node_states.push(std::tuple<int, bool, bool>(_current_node_idx, true, false));

        // push uncovered node
        if(!has_uncovered_neighbor)
        {
            _node_states.push(std::tuple<int, bool, bool>(_current_node_idx, false, false));

            if(_debug)
            {
                cout << "+ [node: " << _current_node_idx << ", cover: " << false << "]" << endl;
            }
        }

        if(_debug)
        {
            cout << "+ [node: " << _current_node_idx << ", cover: " << true << "]" << endl;
        }
    }
}

void BranchAndBound::log_new_best(int value)
{
    std::ostringstream oss;
    oss << "output/BB/" << _graph_name << "_BB_" << _max_seconds << ".trace";

    std::fstream output_file;
    output_file.open(oss.str(), fstream::out | fstream::app);
    output_file << difftime(time(0), _start_time) << ", " << value << "\n";
    output_file.close();
    log_new_solution(value);
}

void BranchAndBound::log_new_solution(int value)
{
    std::ostringstream oss;
    oss << "output/BB/" << _graph_name << "_BB_" << _max_seconds << ".sol";

    std::fstream output_file;
    output_file.open(oss.str(), fstream::out);
    output_file << value << "\n";

    int index_idx = _starting_node_idx;
    while(index_idx != (_current_node_idx+1))
    {
        std::set<int>::iterator it_uncovered = _nodes_uncovered.find(index_idx);

        // if an uncovered neighbor is found
        if((it_uncovered == _nodes_uncovered.end()))
        {
            output_file << index_idx << ", ";
        }
        index_idx = (index_idx+1) % _node_count;
    }

    output_file << "\n";
    output_file.close();
}

void BranchAndBound::split_filename()
{
    std::size_t found = _fileloc.find_last_of("/");
    _graph_name = _fileloc.substr(found+1);
}

bool BranchAndBound::check_time()
{
    if(difftime(time(0), _start_time) > _max_seconds)
    {
        return false;
    }
    return true;
}

void BranchAndBound::increment_current_node_idx()
{
    _current_node_idx = ++_current_node_idx % _node_count;
}

void BranchAndBound::decrement_current_node_idx()
{
    _current_node_idx = --_current_node_idx % _node_count;
}

void BranchAndBound::pop_current_node_state()
{
    _node_states.pop();

    if(_debug)
    {
        cout << "- [node: " << std::get<0>(_current_node_state) << ", cover: "
            << std::get<1>(_current_node_state) << "]" << endl;
    }

    if(!std::get<1>(_current_node_state))
    {
        _nodes_uncovered.erase(std::get<0>(_current_node_state));
    }

    // add any edges we removed during processing
    std::tuple<int, int, int> edge_info;
    while(!_edges_removed.empty() && std::get<0>(_edges_removed.top()) == std::get<0>(_current_node_state))
    {
        edge_info = _edges_removed.top();

        if(_debug)
        {
            cout << "\tadded edge [" << std::get<2>(edge_info) << "]: (" << std::get<0>(edge_info)
                << ", " << std::get<1>(edge_info) << ")" << endl;
        }
        _graph.add_edge(std::get<2>(edge_info), std::pair<int,int>(std::get<0>(edge_info), std::get<1>(edge_info)));
        _edges_removed.pop();
    }
}

void BranchAndBound::process_covered_state()
{
    // remove all edges associated with the node
    std::vector<int> edges = _graph.connected_edges(std::get<0>(_current_node_state));
    for(std::vector<int>::iterator edge_it = edges.begin(); edge_it != edges.end(); edge_it++)
    {
        std::pair<int,int> edge_nodes = _graph.get_edge_nodes(*edge_it);

        // get the node's neighbor
        int neighbor_idx = edge_nodes.first == std::get<0>(_current_node_state) ? edge_nodes.second : edge_nodes.first;

        // add the nodes and edge to the edge_removed cache
        std::tuple<int,int,int> edge_removed(std::get<0>(_current_node_state), neighbor_idx, *edge_it);
        _edges_removed.push(edge_removed);
        _graph.remove_edge(*edge_it);

        if(_debug)
        {
            cout << "\tremoved edge [" << std::get<2>(edge_removed) << "]: (" << std::get<0>(edge_removed)
                << ", " << std::get<1>(edge_removed) << ")" << endl;
        }
    }
}

void BranchAndBound::process_uncovered_state()
{
    _nodes_uncovered.insert(std::get<0>(_current_node_state));
}

