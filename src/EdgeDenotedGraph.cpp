/*
EdgeDenotedGraph.cpp: This module intakes RawMetisFile objects to construct EdgeDenotedGraphs, which contains edge-denoted representations of the graph (the incidence matrix).  EdgeDenotedGraph answers vertex cover queries, i.e. "is this set of vertices (by vertex ID) a vertex cover" in O(|V||E|) time.
*/

#include "EdgeDenotedGraph.h"
#include "Random.h"
#include <iostream>
#include <algorithm>

using namespace std;

EdgeDenotedGraph::EdgeDenotedGraph(const std::string &filepath) {
    metis_file = RawMetisFile(filepath);
    _num_edges = metis_file.num_edges();
    _num_vertices = metis_file.num_vertices();

    max_vertex_degree = std::numeric_limits<double>::infinity();

    int edge_idx = 0;
    for (auto vertex_idx=0U; vertex_idx < metis_file.Connections.size(); ++vertex_idx) {
        vector<int> edges_connected_to_vertex_i;

        for (const auto &neighbor_idx : metis_file.Connections[vertex_idx]) {
            if (vertex_idx < neighbor_idx) {
                edges_connected_to_vertex_i.emplace_back(edge_idx);
                connections_cache[make_pair(vertex_idx, neighbor_idx)] = edge_idx;
                edge_map.insert(std::pair<int, std::pair<int, int>>(edge_idx, std::pair<int, int>(vertex_idx, neighbor_idx)));
                edge_idx++;
            } else {
                edges_connected_to_vertex_i.emplace_back( connections_cache[make_pair(neighbor_idx, vertex_idx)] );
            }
        } IncidenceMatrix.emplace_back( std::move(edges_connected_to_vertex_i) );
    }

    // get the max vertex degree
    for (int vertex_idx = 0; vertex_idx < _num_vertices; vertex_idx++) {
        int edge_count = connected_edges(vertex_idx).size();
        if(edge_count > max_vertex_degree) {
            max_vertex_degree = edge_count;
        }
    }

    //for (int i = 0; i < 10; i++) {
    //    cout << edge_map.at(i).first << ", " << edge_map.at(i).second << endl;
    //}
}

void EdgeDenotedGraph::print() const {
    for (auto vertex_idx=0U; vertex_idx < IncidenceMatrix.size(); ++vertex_idx) {
        cout << "[Vertex " << vertex_idx << "]: ";
        for (const auto &edge_idx : IncidenceMatrix[vertex_idx]) {
            cout << edge_idx << " ";
        } cout << "\n";
    } cout << endl;
}

int EdgeDenotedGraph::get_source(int edge_idx) {
    return edge_map.at(edge_idx).first;
}

int EdgeDenotedGraph::get_dest(int edge_idx) {
    return edge_map.at(edge_idx).second;
}

std::pair<int,int> EdgeDenotedGraph::get_edge_nodes(int edge_idx) {
    return edge_map.at(edge_idx);
}

vector<int> EdgeDenotedGraph::compute_edge_cover_using_vertex_idxs(const std::vector<int> &vertex_indices) const {
    vector<int> bitfield(num_edges(), 0);
    for (const auto i : vertex_indices) {
        if (i >= IncidenceMatrix.size() or i < 0) continue;
        for (const auto &edge_idx : IncidenceMatrix[i]) {
            bitfield[edge_idx] = 1;
        }
    }
    return bitfield;
}

bool EdgeDenotedGraph::is_vertex_cover(const std::vector<int> &vertex_indices) const {
    auto bitfield = compute_edge_cover_using_vertex_idxs(vertex_indices);
    for (const auto &b : bitfield) {
        if (b == 0) return false;
    } return true;
}

tuple<bool, int> EdgeDenotedGraph::vertex_cover_check(const std::vector<int> &vertex_indices) const {
    auto bitfield = compute_edge_cover_using_vertex_idxs(vertex_indices);

    auto is_vc = true; auto num_uncovered_edges = 0;
    for (const auto &b : bitfield) {
        if (b == 0) {
            is_vc = false;
            num_uncovered_edges++;
        }
    } return make_tuple(is_vc, num_uncovered_edges);
}

tuple<int, vector<int>> EdgeDenotedGraph::compute_edge_cover_using_bitfield(const std::vector<int> &bitfield) const {
    auto num_vertices = 0;
    vector<int> edges_bitfield(num_edges(), 0);

    for (auto idx=0U; idx < bitfield.size(); ++idx) {
        if (bitfield[idx] == 0) continue;
        num_vertices++;
        for (const auto &edge_idx : IncidenceMatrix[idx]) {
            edges_bitfield[edge_idx] = 1;
        }
    }

    return make_tuple(num_vertices, edges_bitfield);
}

bool EdgeDenotedGraph::is_vertex_cover_using_bitfield(const std::vector<int> &bitfield) const {
    auto vertex_count_edges_bitfield_pair = compute_edge_cover_using_bitfield(bitfield);
    for (const auto &b : std::get<1>(vertex_count_edges_bitfield_pair)) {
        if (b == 0) return false;
    } return true;
}

std::tuple<bool, int, int> EdgeDenotedGraph::vertex_cover_check_using_bitfield(const std::vector<int> &bitfield) const {
    auto vertex_count_edges_bitfield_pair = compute_edge_cover_using_bitfield(bitfield);
    auto is_vc = true; auto num_uncovered_edges = 0;
    for (const auto &b : std::get<1>(vertex_count_edges_bitfield_pair)) {
        if (b == 0) {
            is_vc = false;
            num_uncovered_edges++;
        }
    } return make_tuple(is_vc, std::get<0>(vertex_count_edges_bitfield_pair), num_uncovered_edges);
}

int EdgeDenotedGraph::incremental_edge_cost_of_bit_flip(const std::vector<int> &bitfield, int bit_index) const {
    auto edges_affected = 0;
    for (const auto neighbor_idx : metis_file.Connections[bit_index]) {
        // This counts the edges that 1) have bit_index on one node end, but 2) have a non-marked node on the other end
        // Such edges indicate the marginal cost if the bit was flipped
        if (bitfield[neighbor_idx] == 0) edges_affected++;
    }

    // if the bit is currently 0, then flipping the bit to 1 will add *negative* cost, since it will be covering previously-uncovered edges
    return (bitfield[bit_index] == 0) ? -edges_affected : edges_affected;
}

int EdgeDenotedGraph::num_vertices() const {
    return _num_vertices;
}

int EdgeDenotedGraph::num_edges() const {
    return _num_edges;
}

RawMetisFile EdgeDenotedGraph::to_metis_file() const {
    auto mfile = metis_file;
    return mfile;
}

bool EdgeDenotedGraph::has_edge(int node1, int node2) const {
    const std::vector<int>& row = IncidenceMatrix.at(node1);
    for (const auto &edge : row) {
    if ((edge_map.at(edge).first == node1 && edge_map.at(edge).second == node2) ||
        (edge_map.at(edge).first == node2 && edge_map.at(edge).second == node1)) {
        return true;
    }
    }
    return false;
}

EdgeDenotedGraph EdgeDenotedGraph::copy() {
    EdgeDenotedGraph new_graph;

    return new_graph;
}

int EdgeDenotedGraph::select_an_edge() {
    int edge_cnt = std::rand() % num_edges();

    auto it = edge_map.begin();
    std::advance(it, edge_cnt);
    int edge_idx = it->first;
    //cout << "[DEBUG] edge_idx = " << edge_idx << endl;

    int source_idx = edge_map.at(edge_idx).first;
    int dest_idx = edge_map.at(edge_idx).second;
    //cout << "source_idx = " << source_idx << endl;
    //cout << "dest_idx = " << dest_idx << endl;

    return edge_idx;
}

int EdgeDenotedGraph::select_a_node()
{
    int edge_idx = select_an_edge();
    return get_edge_nodes(edge_idx).first;
}

const std::vector<int>& EdgeDenotedGraph::connected_edges(int node) {
    return IncidenceMatrix.at(node);
}

void EdgeDenotedGraph::remove_edge(int edge_idx) {
    int source = edge_map.at(edge_idx).first;
    vector<int> source_row = IncidenceMatrix.at(source);
    for (auto i = 0U; i < source_row.size(); i++) {
        if (source_row[i] == edge_idx) {
            source_row.erase(source_row.begin() + i);
            break;
        }
    }
    IncidenceMatrix.at(source) = source_row;

    int dest = edge_map.at(edge_idx).second;
    vector<int> dest_row = IncidenceMatrix.at(dest);
    for (auto i = 0U; i < dest_row.size(); i++) {
        if (dest_row[i] == edge_idx) {
            dest_row.erase(dest_row.begin() + i);
            break;
        }
    }
    IncidenceMatrix.at(dest) = dest_row;

    edge_map.erase(edge_idx);
    _num_edges--;
}

void EdgeDenotedGraph::add_edge(int edge_idx, std::pair<int,int> edge_nodes)
{
    // add back to edge_map
    edge_map.insert(std::pair<int, std::pair<int,int>>(edge_idx, std::pair<int,int>(edge_nodes.first, edge_nodes.second)));

    // insert the edge into the source row (smaller node)
    vector<int> first_row = IncidenceMatrix.at(edge_nodes.first);
    vector<int> second_row = IncidenceMatrix.at(edge_nodes.second);

    first_row.emplace_back(edge_idx);
    second_row.emplace_back(edge_idx);

    IncidenceMatrix.at(edge_nodes.first) = first_row;
    IncidenceMatrix.at(edge_nodes.second) = second_row;

    // increment the number of edges counter
    _num_edges++;
}

