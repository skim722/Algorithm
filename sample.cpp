#include "RawMetisFile.cpp"
#include "EdgeDenotedGraph.h"
#include "Random.h"
#include "GeneticAlgoSearch.h"

#include <iostream>
#include <map>

using namespace std;

void approxVertexCover(char* filename, char* outputfile) {
    EdgeDenotedGraph graph(filename);

    vector<int> selected_nodes;

    while (graph.num_edges() > 0) {
        int edge = graph.select_an_edge();
        int source = graph.get_source(edge);
        int dest = graph.get_dest(edge);
	//cout << "[DEBUG] edge selected: (" << source << ", " << dest << ")" << endl;

        selected_nodes.push_back(source);
        selected_nodes.push_back(dest);
	//cout << "[DEBUG] selected nodes: {";
	//for (auto i = selected_nodes.begin(); i != selected_nodes.end(); ++i)
	//    cout << *i << ' ';
	//cout << "}" << endl;
	graph.remove_edge(edge);

        auto source_edges = graph.connected_edges(source);
        for (auto it = source_edges.begin(); it != source_edges.end(); ++it) {
	    //cout << "[DEBUG] source connected edge removed: " << *it << " (" << graph.get_source(*it) << ", " << graph.get_dest(*it) << ")" << endl;
	    graph.remove_edge(*it);
        }

	auto dest_edges = graph.connected_edges(dest);
        for (auto it = dest_edges.begin(); it != dest_edges.end(); ++it) {
	    //cout << "[DEBUG] dest connected edge removed: " << *it << " (" << graph.get_source(*it) << ", " << graph.get_dest(*it) << ")" << endl;
            graph.remove_edge(*it);
	}

	//graph.print();
	//cout << "[DEBUG] Remaining edges: " << graph.num_edges() << endl;
    }

    cout << "Are nodes { ";
    for (const auto &v : selected_nodes)
        cout << v << ", ";
    cout << "} a vertex cover?: " << (graph.is_vertex_cover(selected_nodes) ? "YES" : "NO") << endl;
    cout << "Discovered vertex cover size: " << selected_nodes.size() << endl;

    auto metis_file = graph.to_metis_file();
    //metis_file.print();
    metis_file.write_to_file("copy.graph");

    if (strlen(outputfile) > 0) {
        ofstream myfile;
        myfile.open (outputfile);
        myfile << selected_nodes.size() << endl;
        for (const auto &v : selected_nodes)
            myfile << v << ",";
        myfile << endl;
        myfile.close();
    }
}

int main(int argc, char** argv) {
    if (argc == 1) {
        cout << "Usage: ./runApprox <input_file_path>" << endl;
        return 0;
    }

    std::srand(std::time(0));

    EdgeDenotedGraph graph(argv[1]);
    //graph.print();
    cout << "Node count: " << graph.num_vertices() << endl;
    cout << "Edge count: " << graph.num_edges() << endl;

    cout << "[DEBUG] Start ApproxVertexCover" << endl;
    approxVertexCover(argv[1], argv[2]);
    cout << "[DEBUG] End ApproxVertexCover" << endl;
/*
    vector<vector<int>> nodesets = {
        {1, 2, 4, 3, 6},
        {0, 2, 3, 4, 5},
        {2, 3},
    };

    for (const auto &nodeset : nodesets) {
        cout << "Are nodes { ";
        for (const auto &v : nodeset) cout << v << ", ";
        cout << "} a vertex cover?: " << (graph.is_vertex_cover(nodeset) ? "YES" : "NO") << endl;
    }

    vector<int> bits = { 0, 1, 0, 0, 0, 0, 0 };
    cout << "BITS vertex cover?: " << (graph.is_vertex_cover_using_bitfield(bits) ? "YES" : "NO") << endl;
    bits = { 1, 0, 1, 1, 1, 1, 0 };
    cout << "BITS vertex cover?: " << (graph.is_vertex_cover_using_bitfield(bits) ? "YES" : "NO") << endl;

    cout << "MARGINAL COST: " << graph.incremental_edge_cost_of_bit_flip({ 0, 0, 1, 1, 0, 0, 0 }, 4) << endl;
    cout << "MARGINAL COST: " << graph.incremental_edge_cost_of_bit_flip({ 1, 0, 1, 1, 1, 1, 0 }, 0) << endl;

    auto pp = graph.vertex_cover_check({1, 2});
    cout << "Cost {1, 2}: " << std::get<1>(pp) << "\n";

    auto metis_file = graph.to_metis_file();
    metis_file.print();
    metis_file.write_to_file("copy.graph");

    // Test Random
    Random ran3(10);
    for (int i=0; i < 10; i++) {
        cout << ran3() << endl;
    }
*/
    return 0;
}
