#ifndef RAW_METIS_FILE_H_
#define RAW_METIS_FILE_H_

#include <vector>
#include <string>

struct RawMetisFile {
    std::vector<std::vector<int>>   Connections;

    RawMetisFile() = default;
    RawMetisFile(const std::string &filepath);
    int num_vertices();
    int num_edges();
    void print();
    void write_to_file(const std::string &filepath);
};

#endif
