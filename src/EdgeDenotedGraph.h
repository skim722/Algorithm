#ifndef EDGE_DENOTED_GRAPH_H_
#define EDGE_DENOTED_GRAPH_H_

#include "RawMetisFile.h"
#include <vector>
#include <map>
#include <set>
#include <list>
#include <tuple>
#include <cstdlib>
#include <limits>

class EdgeDenotedGraph {
private:
    std::vector<std::vector<int>> IncidenceMatrix;
    RawMetisFile metis_file;
    std::map<std::pair<int, int>, int> connections_cache;
    std::map<int, std::pair<int, int>> edge_map;

    int _num_edges      = 0;
    int _num_vertices   = 0;

    std::vector<int> compute_edge_cover_using_vertex_idxs(const std::vector<int> &vertex_indices) const;
    std::tuple<int, std::vector<int>> compute_edge_cover_using_bitfield(const std::vector<int> &bitfield) const;

    void print_incidence_matrix();
public:
    EdgeDenotedGraph() = default;
    EdgeDenotedGraph(const std::string &filepath);
    int get_source(int edge_idx);
    int get_dest(int edge_idx);
    std::pair<int, int> get_edge_nodes(int edge_idx);
    void print() const;
    bool is_vertex_cover(const std::vector<int> &vertex_indices) const;
    std::tuple<bool, int> vertex_cover_check(const std::vector<int> &vertex_indices) const;
    bool is_vertex_cover_using_bitfield(const std::vector<int> &bitfield) const;
    std::tuple<bool, int, int> vertex_cover_check_using_bitfield(const std::vector<int> &bitfield) const;
    int incremental_edge_cost_of_bit_flip(const std::vector<int> &bitfield, int bit_index) const;
    int num_vertices() const;
    int num_edges() const;
    RawMetisFile to_metis_file() const;

    double max_vertex_degree;
    
    bool has_edge(int node1, int node2) const;
    EdgeDenotedGraph copy();
    int select_an_edge();
    int select_a_node();
    const std::vector<int>& connected_edges(int node);
    void remove_edge(int edge_idx);
    void add_edge(int edge_idx, std::pair<int,int> edge_nodes);
};

#endif
