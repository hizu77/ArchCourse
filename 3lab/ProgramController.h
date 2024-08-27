#pragma once
#include "emul.h"
#include <iostream>
#include <string>
#include <optional>

class ProgramController {
public:
    void GetParameters(int argc, char** argv) {
        for (size_t i = 1; i < argc; ++i) {
            std::string cur_option = argv[i];
            if (cur_option == "--replacement") {
                ++i;
                replacement = std::stoi(argv[i], nullptr, 10);
            } else if (cur_option == "--asm") {
                ++i;
                asm_file_ = argv[i];
            } else if (cur_option == "--bin") {
                ++i;
                bin_file_ = argv[i];
            }
        }
    }
    void RunProgram() {
        std::vector<uint32_t> program_binary = ConvertToBinary(asm_file_);
        if (bin_file_) {
            SaveBin(bin_file_.value(), program_binary);
        }
        CPU cpu;
        if (replacement == 0 || replacement == 1) {
            double lru_hit = cpu.Execute<LRU>(program_binary);
            printf("LRU\thit rate: %3.4f%%\n", lru_hit);
        }
        if (replacement == 0 || replacement == 2) {
            double plru_hit = cpu.Execute<pLRU>(program_binary);
            printf("pLRU\thit rate: %3.4f%%\n", plru_hit);
        }
    }



private:
    std::optional<std::string> bin_file_ = std::nullopt;
    std::string asm_file_;
    int replacement = -1;

};