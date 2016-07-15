#include "RawMetisFile.cpp"
#include "EdgeDenotedGraph.h"
#include <iostream>
#include <map>
#include <set>
#include <tuple>

using namespace std;

int main(int argc, char** argv) {
    EdgeDenotedGraph graph(argv[1]);

    vector<int> selected_nodes;

    while (graph.num_edges() > 0) {
        auto edge = graph.select_an_edge();
        int source = std::get<0>(edge);
        int dest = std::get<1>(edge);

        selected_nodes.insert(source);
        selected_nodes.insert(dest);
        
        set<int> source_edges = graph.connected_edges(source);
        for (auto it = source_edges.begin(); it != source_edges.end(); ++it) {
            graph.remove_edge(source, *it);
        }
    }

    cout << "Are nodes { ";
    for (const auto &v : nodeset)
        cout << v << ", ";
    cout << "} a vertex cover?: " << (graph.is_vertex_cover(selected_nodes) ? "YES" : "NO") << endl;

    auto metis_file = graph.to_metis_file();
    metis_file.print();
    metis_file.write_to_file("copy.graph");

    return 0;
}
