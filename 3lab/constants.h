#pragma once
#include <iostream>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <map>
// we have this
static constexpr const uint32_t ADDR_LEN = 18;
static constexpr const uint32_t CACHE_INDEX_LEN = 3;
static constexpr const uint32_t CACHE_LINE_SIZE = 64;
static constexpr const uint32_t CACHE_LINE_COUNT = 32;

//we need to calculate this
static constexpr const uint32_t MEM_SIZE = (1 << ADDR_LEN); // 262144 bytes
static constexpr const uint32_t CACHE_SIZE = CACHE_LINE_SIZE * CACHE_LINE_COUNT; // 2048 bytes
static constexpr const uint32_t CACHE_SETS = (1 << CACHE_INDEX_LEN); // 8
static constexpr const uint32_t CACHE_WAY = CACHE_LINE_COUNT / CACHE_SETS; // 4
static constexpr const uint32_t CACHE_OFFSET_LEN = 6; // log2(CACHE_LINE_SIZE) = 6
static constexpr const uint32_t CACHE_TAG_LEN = ADDR_LEN - CACHE_INDEX_LEN - CACHE_OFFSET_LEN; // 9 bits

//policy constants
constexpr int LRU = 1;
constexpr int pLRU = 2;

//RV32I and RV32M instructions
static const std::unordered_set<std::string> INSTRUCTIONS = {
        "addi", "slti", "sltiu", "andi", "ori", "xori",
        "slli", "srli", "srai",
        "lui", "auipc",
        "add", "slt", "sltu",
        "and", "or", "xor",
        "sll", "srl",
        "sub", "sra",
        "jal", "jalr",
        "beq", "bne",
        "blt", "bltu",
        "bge", "bgeu",
        "fence",
        "rem", "remu",
        "lw", "lh", "lhu", "lb", "lbu",
        "sw", "sh", "sb",
        "mul", "mulh", "mulhu", "mulhsu",
        "div", "divu",
        "ecall", "ebreak",
};

//ABI registers
static std::unordered_map<std::string, uint8_t> ABI_REG = {
    {"zero", 0},                                                                                        //hard wired-zero
    {"ra", 1},                                                                                          //return address
    {"sp", 2},                                                                                          //stack pointer
    {"gp", 3},                                                                                          //global pointer
    {"tp", 4},                                                                                          //thread pointer
    {"t0", 5}, {"t1", 6}, {"t2", 7},                                                         // temporaries
    {"s0", 8},                                                                                          //saved register/frame
    {"s1", 9},                                                                                          //saved register
    {"a0", 10}, {"a1", 11},                                                                        //function args/return
    {"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15}, {"a6", 16}, {"a7", 17},  //func args
    {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23},  //saved regs
    {"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27},                                   //saved regs
    {"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31}                                      //temporaries
};

//OPCODES
static constexpr const uint32_t LUI = 0b0110111;
static constexpr const uint32_t AUIPC = 0b0010111;
static constexpr const uint32_t JAL = 0b1101111;
static constexpr const uint32_t JALR = 0b1100111;
static constexpr const uint32_t BRANCH = 0b1100011;
static constexpr const uint32_t LOAD = 0b0000011;
static constexpr const uint32_t STORE = 0b0100011;
static constexpr const uint32_t IMM = 0b0010011;
static constexpr const uint32_t OP = 0b0110011;
static constexpr const uint32_t MISC_MEM = 0b0001111;
static constexpr const uint32_t SYSTEM = 0b1110011;

//Instructions RV32I and RV32M with OPCODES
static std::unordered_map<std::string, uint32_t> OPCODES = {
        {"mul", OP}, {"mulh", OP}, {"mulhu", OP}, {"mulhsu", OP},
        {"rem", OP}, {"remu", OP},
        {"divu", OP}, {"div", OP},
        {"add", OP},
        {"slt", OP}, {"sltu", OP},
        {"and", OP}, {"or", OP}, {"xor", OP},
        {"sll", OP}, {"srl", OP}, {"sub", OP}, {"sra", OP},
        {"addi", IMM},
        {"slti", IMM}, {"sltiu", IMM},
        {"andi", IMM}, {"ori", IMM}, {"xori", IMM},
        {"slli", IMM}, {"srli", IMM}, {"srai", IMM},
        {"lui", LUI},
        {"auipc", AUIPC},
        {"jal", JAL},
        {"jalr", JALR},
        {"beq", BRANCH},
        {"bne", BRANCH},
        {"blt", BRANCH}, {"bltu", BRANCH},
        {"bge", BRANCH}, {"bgeu", BRANCH},
        {"lw", LOAD},
        {"lh", LOAD}, {"lhu", LOAD},
        {"lb", LOAD}, {"lbu", LOAD},
        {"sw", STORE}, {"sh", STORE}, {"sb", STORE},
        {"fence", MISC_MEM},
        {"ecall", SYSTEM}, {"ebreak", SYSTEM}
};

//FUNCT3
static constexpr const uint32_t F3_JALR = 0b000;
static constexpr const uint32_t F3_BEQ = 0b000;
static constexpr const uint32_t F3_BNE = 0b001;
static constexpr const uint32_t F3_BLT = 0b100;
static constexpr const uint32_t F3_BGE = 0b101;
static constexpr const uint32_t F3_BLTU = 0b110;
static constexpr const uint32_t F3_BGEU = 0b111;
static constexpr const uint32_t F3_LB = 0b000;
static constexpr const uint32_t F3_LH = 0b001;
static constexpr const uint32_t F3_LW = 0b010;
static constexpr const uint32_t F3_LBU = 0b100;
static constexpr const uint32_t F3_LHU = 0b101;
static constexpr const uint32_t F3_SB = 0b000;
static constexpr const uint32_t F3_SH = 0b001;
static constexpr const uint32_t F3_SW = 0b010;
static constexpr const uint32_t F3_ADDI = 0b000;
static constexpr const uint32_t F3_SLTI = 0b010;
static constexpr const uint32_t F3_SLTIU = 0b011;
static constexpr const uint32_t F3_XORI = 0b100;
static constexpr const uint32_t F3_ORI = 0b110;
static constexpr const uint32_t F3_ANDI = 0b111;
static constexpr const uint32_t F3_SLLI = 0b001;
static constexpr const uint32_t F3_SRLI = 0b101;
static constexpr const uint32_t F3_SRAI = 0b101;
static constexpr const uint32_t F3_ADD = 0b000;
static constexpr const uint32_t F3_SUB = 0b000;
static constexpr const uint32_t F3_SLL = 0b001;
static constexpr const uint32_t F3_SLT = 0b010;
static constexpr const uint32_t F3_SLTU = 0b011;
static constexpr const uint32_t F3_XOR = 0b100;
static constexpr const uint32_t F3_SRL = 0b101;
static constexpr const uint32_t F3_SRA = 0b101;
static constexpr const uint32_t F3_OR = 0b110;
static constexpr const uint32_t F3_AND = 0b111;
static constexpr const uint32_t F3_FENCE = 0b000;
static constexpr const uint32_t F3_FENCE_ISO = 0b000;
static constexpr const uint32_t F3_PAUSE = 0b000;
static constexpr const uint32_t F3_ECALL = 0b000;
static constexpr const uint32_t F3_EBREAK = 0b000;
static constexpr const uint32_t F3_MUL = 0b000;
static constexpr const uint32_t F3_MULH = 0b001;
static constexpr const uint32_t F3_MULHU = 0b011;
static constexpr const uint32_t F3_MULHSU = 0b010;
static constexpr const uint32_t F3_DIV = 0b100;
static constexpr const uint32_t F3_DIVU = 0b101;
static constexpr const uint32_t F3_REM = 0b110;
static constexpr const uint32_t F3_REMU = 0b111;

//FUNCT3 with INSTRUCTIONS
static std::unordered_map<std::string, uint32_t> FUNCT3 {
        {"addi", F3_ADDI},
        {"slti", F3_SLTI}, {"sltiu", F3_SLTIU},
        {"andi", F3_ANDI},  {"and", F3_AND},
        {"ori", F3_ORI}, {"xori", F3_XORI},
        {"slli", F3_SLLI}, {"srli", F3_SRLI}, {"srai", F3_SRAI},
        {"add", F3_ADD},
        {"slt", F3_SLT}, {"sltu", F3_SLTU},
        {"or", F3_OR}, {"xor", F3_XOR},
        {"sll", F3_SLL}, {"srl", F3_SRL}, {"sub", F3_SUB}, {"sra", F3_SRA},
        {"jalr", F3_JALR},
        {"beq", F3_BEQ}, {"bne", F3_BNE}, {"blt", F3_BLT}, {"bltu", F3_BLTU},
        {"bge", F3_BGE}, {"bgeu", F3_BGEU},
        {"lw", F3_LW}, {"lh", F3_LH}, {"lhu", F3_LHU}, {"lb", F3_LB}, {"lbu", F3_LBU},
        {"sw", F3_SW}, {"sh", F3_SH}, {"sb", F3_SB},
        {"fence", F3_FENCE}, {"ecall", F3_ECALL},
        {"ebreak", F3_EBREAK},
        {"mul", F3_MUL}, {"mulh", F3_MULH}, {"mulhu", F3_MULHU}, {"mulhsu", F3_MULHSU},
        {"div", F3_DIV}, {"divu", F3_DIVU},
        {"rem", F3_REM}, {"remu", F3_REMU},
        {"pause", F3_PAUSE}
};

//FUNCT7
static constexpr const uint32_t F7_SLLI = 0b0000000;
static constexpr const uint32_t F7_SRLI = 0b0000000;
static constexpr const uint32_t F7_SRAI = 0b0100000;
static constexpr const uint32_t F7_ADD = 0b0000000;
static constexpr const uint32_t F7_SUB = 0b0100000;
static constexpr const uint32_t F7_SLL = 0b0000000;
static constexpr const uint32_t F7_SLT = 0b0000000;
static constexpr const uint32_t F7_SLTU = 0b0000000;
static constexpr const uint32_t F7_XOR = 0b0000000;
static constexpr const uint32_t F7_SRL = 0b0000000;
static constexpr const uint32_t F7_SRA = 0b0100000;
static constexpr const uint32_t F7_OR = 0b0000000;
static constexpr const uint32_t F7_AND = 0b0000000;
static constexpr const uint32_t F7_MUL = 0b0000001;

//FUNCT7 with INSTRUCTIONS
static std::unordered_map<std::string, uint32_t> FUNCT7 = {
        {"slli", F7_SLLI}, {"srli", F7_SRLI}, {"srai", F7_SRAI},
        {"add",  F7_ADD},
        {"slt",  F7_SLT}, {"sltu", F7_SLTU},
        {"and",  F7_AND}, {"or", F7_OR}, {"xor", F7_XOR},
        {"sll",  F7_SLL}, {"srl", F7_SRL},
        {"sub",  F7_SUB}, {"sra", F7_SRA},
        {"mul",  F7_MUL}, {"mulh", F7_MUL}, {"mulhu", F7_MUL}, {"mulhsu", F7_MUL},
        {"div",  F7_MUL}, {"divu", F7_MUL},
        {"rem",  F7_MUL}, {"remu", F7_MUL}
};

//FUNCT12
static constexpr const uint32_t F12_ECALL = 0b000000000000;
static constexpr const uint32_t F12_EBREAK = 0b000000000001;

//FUNCT12 with INSTRUCTIONS
static std::unordered_map<std::string, uint32_t> FUNCT12 = {
        {"ecall", F12_ECALL},
        {"ebreak", F12_EBREAK}
};