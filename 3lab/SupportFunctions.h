#pragma once
#include "constants.h"
#include <vector>
#include <fstream>
#include <sstream>

//Get tag from address
uint32_t GetTag(uint32_t address) {
    uint32_t tag = address >> (CACHE_INDEX_LEN + CACHE_OFFSET_LEN);
    return tag;
}

//Get index from address
uint32_t GetIndex(uint32_t address) {
    uint32_t index = (address >> CACHE_OFFSET_LEN) & ((1 << CACHE_INDEX_LEN) - 1);
    return index;
}

//Get offset from address
uint32_t GetOffset(uint32_t address) {
    uint32_t offset = address & ((1 << CACHE_OFFSET_LEN) - 1);
    return offset;
}

//Make address from tag and index for reading or writing cache line
uint32_t MakeLineAddress(uint32_t tag, uint32_t index) {
    uint32_t address = 0;
    address |= index << CACHE_OFFSET_LEN;
    address |= tag << (CACHE_OFFSET_LEN + CACHE_INDEX_LEN);
    return address;
}

//All INSTRUCTIONS FORMATS : R, I, U, S, B, J

//Get R-type instruction :
//7   5   5   3  5  7
//f7 rs2 rs1 f3 rd op
uint32_t RType(uint32_t opcode, uint32_t rd, uint32_t funct3, uint32_t rs1, uint32_t rs2, uint32_t funct7) {
    uint32_t instruction;
    instruction = opcode & 0b1111111;
    instruction |= (rd & 0b11111) << 7;
    instruction |= (funct3 & 0b111) << 12;
    instruction |= (rs1 & 0b11111) << 15;
    instruction |= (rs2 & 0b11111) << 20;
    instruction |= (funct7 & 0b1111111) << 25;
    return instruction;
}

//Get I-type instruction :
//12         5   3  5  7
//imm[11:0] rs1 f3 rd op
uint32_t IType(uint32_t opcode, uint32_t rd, uint32_t f3, uint32_t rs1, int32_t imm) {
    uint32_t instruction;
    instruction = opcode & 0b1111111;
    instruction |= (rd & 0b11111) << 7;
    instruction |= (f3 & 0b111) << 12;
    instruction |= (rs1 & 0b11111) << 15;
    instruction |= (imm & 0b111111111111) << 20;
    return instruction;
}

//Get U-type instructions :
//20          5  7
//imm[31:12] rd op
uint32_t UType(uint32_t opcode, uint32_t rd, int32_t imm) {
    uint32_t instruction;
    instruction = opcode;
    instruction |= rd << 7;
    instruction |= imm << 12;
    return instruction;
}

//Get S-type instruction :
//7          5   5   3    5     7
//imm[11:5] rs2 rs1 f3 imm[4:0] op
uint32_t SType(uint32_t opcode, uint32_t f3, uint32_t rs1, uint32_t rs2, int32_t imm) {
    uint32_t instruction;
    instruction = opcode & 0b1111111;
    instruction |= (imm & 0b11111) << 7;
    instruction |= f3 << 12;
    instruction |= rs1 << 15;
    instruction |= rs2 << 20;
    instruction |= (((imm & 0b111111100000) >> 5) & 0b1111111) << 25;
    return instruction;
}

//Get B-type instruction :
//1          6       5   5  3     4       1      7
//imm[12] imm[10:5] rs2 rs1 f3 imm[4:1] imm[11] op
uint32_t BType(uint32_t opcode, uint32_t f3, uint32_t rs1, uint32_t rs2, int32_t imm) {
    uint32_t instruction;
    uint32_t imm12 = (imm & 0b1000000000000) >> 12;
    uint32_t imm10_5 = (imm & 0b11111100000) >> 5;
    uint32_t imm4_1 = (imm & 0b11110);
    uint32_t imm11 = (imm & 0b100000000000) >> 11;

    uint32_t imm12_10_5 = imm10_5 | (imm12 << 6);
    uint32_t imm4_1_11 = imm11 | imm4_1;

    instruction = opcode;
    instruction |= imm4_1_11 << 7;
    instruction |= f3 << 12;
    instruction |= rs1 << 15;
    instruction |= rs2 << 20;
    instruction |= imm12_10_5 << 25;
    return instruction;
}

//Get J-type instruction
//1           10       1         8      5 7
//imm[20] imm[10:1] imm[11] imm[19:12] rd op
uint32_t JType(uint32_t opcode, uint32_t rd, int32_t imm) {
    uint32_t instruction;
    uint32_t imm20 = (imm & 0b100000000000000000000) >> 20;
    uint32_t imm10_1 = (imm & 0b11111111111) >> 1;
    uint32_t imm11 = (imm & 0b100000000000) >> 11;
    uint32_t imm19_12 = (imm & 0b11111111000000000000) >> 12;
    uint32_t new_imm = imm19_12 | (imm11 << 8) | (imm10_1 << 9) | (imm20 << 19);

    instruction = opcode;
    instruction |= rd << 7;
    instruction |= new_imm << 12;
    return instruction;
}

//Value to int. base 16 or 10
int32_t ToInt(const std::string& s) {
    int32_t imm = std::stoi(s, nullptr, 0);
    return imm;
}
//Get OPcode from instruction
uint32_t GetOPCode(uint32_t instruction) {
    uint32_t opcode = instruction & 0b1111111;
    return opcode;
}
//Get Funct3 from instruction
uint32_t GetFunct3(uint32_t instruction) {
    uint32_t funct3 = (instruction & (0b111 << 12)) >> 12;
    return funct3;
}
//Get Funct7 from instruction
uint32_t GetFunct7(uint32_t instruction) {
    uint32_t funct7 = (instruction & (0b1111111 << 25)) >> 25;
    return funct7;
}
//Get Funct12 from instruction
uint32_t GetFunct12(uint32_t instruction) {
    uint32_t funct12 = (instruction & (0b111111111111 << 20)) >> 20;
    return funct12;
}
//Get rd from instruction
uint32_t GetRD(uint32_t instruction) {
    uint32_t rd = (instruction & (0b11111 << 7)) >> 7;
    return rd;
}
//Get rs1 from instruction
uint32_t GetRS1(uint32_t instruction) {
    uint32_t rs1 = (instruction & (0b11111 << 15)) >> 15;
    return rs1;
}
//Get rs2 from instruction
uint32_t GetRS2(uint32_t instruction) {
    uint32_t rs2 = (instruction & (0b11111 << 20)) >> 20;
    return rs2;
}
//Sign extending. If imm is negative - extending with 1, else with 0
int32_t SignExtending(uint32_t instruction, int32_t imm, uint32_t mask) {
    if (instruction & 0b10000000000000000000000000000000) {
        imm |= mask;
    }
    return imm;
}
//Get I-type imm(12 bits with sign)
int32_t GetImmI(uint32_t instruction) {
    int32_t imm = GetFunct12(instruction);
    return SignExtending(instruction, imm, 0b11111111111111111111000000000000);
}
//Get S-type imm(12 bits with sign)
int32_t GetImmS(uint32_t instruction) {
    int32_t imm = GetFunct7(instruction) << 5;
    imm |= GetRD(instruction);
    return SignExtending(instruction, imm, 0b11111111111111111111000000000000);
}
//Get B-type imm(13 bits with sign, but bit 0 = 0)
int32_t GetImmB(uint32_t instruction) {
    int32_t imm = (instruction & 0b10000000) << 4; // imm[11]
    imm |= (instruction & 0b111100000000) >> 7; // im[4:1]
    imm |= (instruction & 0b1111110000000000000000000000000) >> 20; //imm[10:5]
    imm |= (instruction & 0b10000000000000000000000000000000) >> 19; //imm[12] sign
    return SignExtending(instruction, imm, 0b11111111111111111110000000000000);
}
//Get U-type imm(20 bits with sign)
int32_t GetImmU(uint32_t instruction) {
    int32_t imm = (instruction & (0b11111111111111111111 << 12)) >> 12;
    return SignExtending(instruction, imm, 0b11111111111100000000000000000000);
}
//Get J-type imm(21 bits with sign, but bit 0 = 0)
int32_t GetImmJ(uint32_t instruction) {
    int32_t imm = instruction & 0b11111111000000000000; //imm[19:12]
    imm |= (instruction & 0b100000000000000000000) >> 9; //imm[11]
    imm |= (instruction & 0b1111111111000000000000000000000) >> 20; //imm[10:1]
    imm |= (instruction & 0b10000000000000000000000000000000) >> 11; //imm[20] sign
    return SignExtending(instruction, imm, 0b11111111111000000000000000000000);
}

//Read instruction from asm-file
bool ReadInstruction(std::ifstream& in, std::vector<std::string>& parts) {
    parts.clear();
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream str(line);
        std::string first;
        str >> first;
        //skip comments and labels
        if (first.back() == ':' || first.front() == '/' || first.empty() || INSTRUCTIONS.find(first) == INSTRUCTIONS.end()) {
            continue;
        }
        parts.push_back(first);
        do {
            std::string arg;
            str >> arg;
            if (arg.empty() || arg.front() == '/') {
                break;
            }
            if (arg.back() == ',') {
                arg.pop_back();
            }
            parts.push_back(arg);
        } while (str);

        return true;
    }
    return false;
}
//Save bin array with instructions in file
void SaveBin(const std::string& filename, const std::vector<uint32_t>& program) {
    std::ofstream file(filename, std::ios::binary);
    for (const auto& i : program) {
        file.write(reinterpret_cast<const char*>(&i), 4);
    }
}
//Convert asm to binary
std::vector<uint32_t> ConvertToBinary(const std::string& filename) {
    std::vector<uint32_t> program;
    std::ifstream in(filename);
    std::vector<std::string> parts;
    while (ReadInstruction(in, parts)) {
        std::string op = parts.front();
        uint32_t instruction = 0;
        //I-type
        if (op == "addi" || op == "slti" || op == "sltiu" || op == "xori" || op == "ori" || op == "andi") {
            instruction = IType(OPCODES[op], ABI_REG[parts[1]], FUNCT3[op], ABI_REG[parts[2]], ToInt(parts[3]));
        } else if (op == "slli" || op == "srli" || op == "srai") {
            uint32_t imm = (ToInt(parts[3]) & 0x1f) | (FUNCT7[op] << 5);
            instruction = IType(OPCODES[op], ABI_REG[parts[1]], FUNCT3[op], ABI_REG[parts[2]], imm);
        } else if (op == "lw" || op == "lh" || op == "lhu" || op == "lb" || op == "lbu") {
            instruction = IType(OPCODES[op], ABI_REG[parts[1]], FUNCT3[op], ABI_REG[parts[3]], ToInt(parts[2]));
        } else if (op == "jalr") {
            instruction = IType(OPCODES[op], ABI_REG[parts[1]], FUNCT3[op], ABI_REG[parts[2]], ToInt(parts[3]));
        }
        //R-type
        else if (op == "add" || op == "slt" || op == "sltu" || op == "and" || op == "or" || op == "xor" || op == "sll" || op == "srl" || op == "sub" || op == "sra") {
            instruction = RType(OPCODES[op], ABI_REG[parts[1]], FUNCT3[op], ABI_REG[parts[2]], ABI_REG[parts[3]], FUNCT7[op]);
        } else if (op == "mul" || op == "mulh" || op == "mulhu" || op == "mulhsu" || op == "div" || op == "divu" || op == "rem" || op == "remu") {
            instruction = RType(OPCODES[op], ABI_REG[parts[1]], FUNCT3[op], ABI_REG[parts[2]], ABI_REG[parts[3]], FUNCT7[op]);
        }
        //U-type
        else if (op == "lui" || op == "auipc") {
            instruction = UType(OPCODES[op], ABI_REG[parts[1]], ToInt(parts[2]));
        }
        //J-type
        else if (op == "jal") {
            instruction = JType(OPCODES[op], ABI_REG[parts[1]], ToInt(parts[2]));
        }
        //B-type
        else if (op == "beq" || op == "bne" || op == "blt" || op == "bltu" || op == "bge" || op == "bgeu") {
            instruction = BType(OPCODES[op], FUNCT3[op], ABI_REG[parts[1]], ABI_REG[parts[2]], ToInt(parts[3]));
        }
        //S-type
        else if (op == "sw" || op == "sh" || op == "sb") {
            instruction = SType(OPCODES[op], FUNCT3[op], ABI_REG[parts[3]], ABI_REG[parts[1]], ToInt(parts[2]));
        } else {
            instruction = IType(OPCODES["addi"], ABI_REG["zero"], FUNCT3["addi"], ABI_REG["zero"], ToInt("0"));
        }
        program.push_back(instruction);
    }
    return program;
}














