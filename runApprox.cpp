/*
 * runApprox.cpp
 * This file contains our implementation of the approximation algorithm, randomly selecting edges until we cover every edge. (as we discussed in the class.)
 * To run the approximation algorithm: ./runApprox <input_file_path>
 * The main function of this file executes approxVertexCover function in this file with the given graph.
 */

#include "RawMetisFile.cpp"
#include "EdgeDenotedGraph.h"
#include "Random.h"
#include "GeneticAlgoSearch.h"

#include <iostream>
#include <map>

using namespace std;

void approxVertexCover(char* filename) {
    EdgeDenotedGraph graph(filename);

    vector<int> selected_nodes;

    while (graph.num_edges() > 0) {
        int edge = graph.select_an_edge();
        int source = graph.get_source(edge);
        int dest = graph.get_dest(edge);

        selected_nodes.push_back(source);
        selected_nodes.push_back(dest);
	graph.remove_edge(edge);

        auto source_edges = graph.connected_edges(source);
        for (auto it = source_edges.begin(); it != source_edges.end(); ++it) {
	    graph.remove_edge(*it);
        }

	auto dest_edges = graph.connected_edges(dest);
        for (auto it = dest_edges.begin(); it != dest_edges.end(); ++it) {
            graph.remove_edge(*it);
	}
    }

    cout << "Are nodes { ";
    for (const auto &v : selected_nodes)
        cout << v << ", ";
    cout << "} a vertex cover?: " << (graph.is_vertex_cover(selected_nodes) ? "YES" : "NO") << endl;
    cout << "Discovered vertex cover size: " << selected_nodes.size() << endl;

    auto metis_file = graph.to_metis_file();
    metis_file.write_to_file("copy.graph");
}

int main(int argc, char** argv) {
    std::srand(std::time(0));

    EdgeDenotedGraph graph(argv[1]);
    cout << "Node count: " << graph.num_vertices() << endl;
    cout << "Edge count: " << graph.num_edges() << endl;

    approxVertexCover(argv[1]);

    return 0;
}
