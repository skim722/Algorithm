/*
RawMetisFile.cpp: This module parses a metis file into RawMetisFile objects that contain a 2D vector of numbers.
*/
#include "RawMetisFile.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

using namespace std;

RawMetisFile::RawMetisFile(const string &filepath) {
    int num_vertices=0, num_edges=0, line_count=0;
    std::string line;
    ifstream ifs(filepath);
    ifs.sync_with_stdio(false);

    // Parse first line of file for number of vertices and edges
    std::getline(ifs, line);
    std::istringstream iss_first_line(line);
    if (not (iss_first_line >> num_vertices >> num_edges)) {
        cout << "[RawMetisFile] First line of file does not contain two integers!\n";
        std::exit(-1);
    }

    // Parse the rest of the file
    while (std::getline(ifs, line) and line_count++ < num_vertices) {
        vector<int> row;
        int tmp, token_count=0;

        // NOTE that the internal representation of the graph is 0-based, while the METIS output is 1-based
        std::istringstream iss(line);
        while ((iss >> tmp) and ++token_count < num_edges) { row.emplace_back(tmp-1); }

        Connections.emplace_back( std::move(row) );
    }
}

int RawMetisFile::num_vertices() {
    return Connections.size();
}

int RawMetisFile::num_edges() {
    int count = 0;
    for (const auto &v : Connections) { count += v.size(); }
    return count / 2;
}

void RawMetisFile::print() {
    for (const auto &v : Connections) {
        for (const auto &i : v) {
            cout << i << " ";
        } cout << "\n";
    }
    cout << "Number of vertices: " << num_vertices() << endl;
    cout << "Number of edges:    " << num_edges() << endl;
}

void RawMetisFile::write_to_file(const std::string &filepath) {
    std::ofstream ofs(filepath.c_str(), std::ofstream::out);
    ofs.sync_with_stdio(false);
    ofs << num_vertices() << " " << num_edges() << " 0\n";
    for (const auto &v : Connections) {
        for (const auto &i : v) {
            // NOTE that the internal representation of the graph is 0-based, while the METIS output is 1-based
            ofs << i+1 << " ";
        } ofs << "\n";
    } ofs.close();
}