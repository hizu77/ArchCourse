#pragma once
#include <iostream>
#include <fstream>
#include <omp.h>
#include <string>
#include <optional>
#include <cstdint>

struct Result {
    uint8_t threads_count;
    double time;
    float volume;
};

void PrintResult(const std::string& out, const Result& result) {
    FILE* file = fopen(out.data(), "w");
    fprintf(file, "%g\n", result.volume);
    fprintf(stdout, "Time (%i thread(s)): %g ms\n", result.threads_count, result.time);
}

struct Options {
    size_t iterations;
    std::string output;
    std::optional<uint8_t> threads;
};

Options Parse(int argc, char** argv) {
    Options options;

    uint8_t i = 1;
    while (i < argc) {
        std::string cur_command = argv[i];
        if (cur_command == "--input") {
            ++i;
            std::ifstream stream(argv[i]);
            stream >> options.iterations;
        } else if (cur_command == "--output") {
            ++i;
            options.output = argv[i];
        } else if (cur_command == "--omp-threads") {
            ++i;
            std::string thread_option = argv[i];
            if (thread_option == "default") {
                options.threads = omp_get_max_threads();
            } else {
                options.threads = std::stoi(thread_option);
            }
        }
        ++i;
    }

    return options;
}