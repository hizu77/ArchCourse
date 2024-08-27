#pragma once
#include "constants.h"
#include "SupportFunctions.h"
#include <iostream>
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>
#include <list>
//Memory structure
class Memory {
public:
    //Copy data in cache line from memory with current address
    void ReadLine(uint32_t address, int8_t* data) {
        auto begin = cells.begin() + address;
        copy(begin, begin + CACHE_LINE_SIZE, data);
    }

    //Copy data in memory from cache line with current address
    void WriteLine(uint32_t address, int8_t* data) {
        copy(data, data + CACHE_LINE_SIZE, cells.begin() + address);
    }

private:
    std::vector<int8_t> cells = std::vector<int8_t>(MEM_SIZE);      //Memory cells
};


//Cache line
class CacheLine {
public:
    //Safety type(8, 16, 32) read from cache line. Template = value type
    template<typename Type>
    Type read(uint32_t offset) {
        valid = true;
        return *((Type*)(&data[offset]));
    }
    
    //Safety type(8, 16, 32) write in cache line. Template = value type
    template<typename Type>
    void write(uint32_t offset, Type value) {
        valid = true;
        dirty = true;
        *((Type*)(&data[offset])) = value;
    }

    std::vector<int8_t> data = std::vector<int8_t>(CACHE_LINE_SIZE); //Data
    uint32_t tag = 0;                                                   //Tag
    bool valid = false;                                                 //Data validity
    bool dirty = false;                                                 //Data rewrite flag
    bool plru_bit = false;                                              //Policy flag
};

//Cache set, size = CACHE_WAY
template<int Policy>
class Set {
public:
    //Check validity of line
    bool isValid(uint32_t line) const {
        return lines[line].valid;
    }
    //Check rewriting of line
    bool isDirty(uint32_t line) const {
        return lines[line].dirty && lines[line].valid;
    }
    //Get tag of line
    uint32_t GetTag(uint32_t line) const {
        return lines[line].tag;
    }
    //Check line tag equal with need tag
    bool isThisTag(uint32_t line, uint32_t this_tag) const {
        return lines[line].tag == this_tag && lines[line].valid;
    }
    //Get data from cache line
    int8_t* GetData(uint32_t line) {
        return lines[line].data.data();
    }
    //Check hit and get line index
    bool isHit(uint32_t tag, uint32_t& idx) {
        for (size_t i = 0; i < CACHE_WAY; ++i) {
            if (isThisTag(i, tag)) {
                idx = i;
                return true;
            }
        }
        return false;
    }
    //Return index of line, which must be rewritten
    uint32_t GetRewriteLine() {
        uint32_t index = 0;
        if (Policy == LRU) {
            if (indices.size() < CACHE_WAY) index = static_cast<uint32_t>(indices.size());          //Next free line
            else index = indices.front();                                                           //Head
        } else if (Policy == pLRU) {
            for (size_t i = 0; i < CACHE_WAY; ++i) {
                if (!lines[i].plru_bit) {
                    index = i;                                                                      //First with false plru flag
                    break;
                }
            }
        }
        return index;
    }
    //Update block.
    //Lru : Retrieves the given line from the middle and places it at the end.
    //Plru : sets the pLRU bit of this line. If pLRU bits are set in all lines as a result,
    // then resets them for all lines except the specified line.
    void update(uint32_t line) {
        if (Policy == LRU) {
            indices.remove(line);
            indices.push_back(line);
        } else if (Policy == pLRU) {
            lines[line].plru_bit = true;
            if (std::none_of(lines.begin(), lines.end(), [](const CacheLine& line) {return !line.plru_bit;})) {
                std::for_each(lines.begin(), lines.end(), [](CacheLine& line) {line.plru_bit = false;});
                lines[line].plru_bit = true;
            }
        }
    }
    //Safety type(8, 16, 32) read data from cache line with same index
    template<typename Type>
    Type read(uint32_t line, uint32_t tag, uint32_t offset) {
        update(line);
        lines[line].tag = tag;
        return lines[line].read<Type>(offset);
    }
    //Safety type(8, 16, 32) write data in cache line with same index
    template<typename Type>
    void write(uint32_t line, uint32_t tag, uint32_t offset, Type value) {
        update(line);
        lines[line].tag = tag;
        lines[line].write<Type>(offset, value);
    }

    std::array<CacheLine, CACHE_WAY> lines;                             //Cache lines of cache set
    std::list<uint32_t> indices;                                        // For lru. Head = next line for substitution

};

//Cache
//Look-through
//Write-back
//LRU and pLRU
template<int Policy>
class Cache {
public:
    //Safety type(8, 16, 32) read data from cache with same address
    template<typename Type>
    Type read(uint32_t address) {
        ++total;
        //Get address info
        uint32_t tag = GetTag(address);
        uint32_t index = GetIndex(address);
        uint32_t offset = GetOffset(address);
        //Get cache set that may contain data
        Set<Policy>& set = sets[index];
        uint32_t line;
        //Find data in this block with same tag
        if (set.isHit(tag, line)) {
            ++hits;
        } else {
            //Get index of displaced line
            line = set.GetRewriteLine();
            //The line was rewritten -> write line in memory
            if (set.isDirty(line)) {
                uint32_t rewrite_address = MakeLineAddress(set.GetTag(line), index);
                memory.WriteLine(rewrite_address, set.GetData(line));
            }
            uint32_t new_address = MakeLineAddress(tag, index);
            memory.ReadLine(new_address, set.GetData(line));
        }
        return set.template read<Type>(line, tag, offset);
    }
    //Safety type(8, 16, 32) write data in cache with same address
    template<typename Type>
    void write(uint32_t address, Type value) {
        ++total;

        uint32_t tag = GetTag(address);
        uint32_t index = GetIndex(address);
        uint32_t offset = GetOffset(address);

        Set<Policy>& set = sets[index];
        uint32_t line;
        if (set.isHit(tag, line)) {
            ++hits;
        } else {
            line = set.GetRewriteLine();
            if (set.isDirty(line)) {
                uint32_t rewrite_address = MakeLineAddress(set.lines[line].tag, index);
                memory.WriteLine(rewrite_address, set.GetData(line));
            }
            uint32_t new_address = MakeLineAddress(tag, index);
            memory.ReadLine(new_address, set.GetData(line));
        }
        return set.template write<Type>(line, tag, offset, value);
    }


    Memory memory;                                                      //Memory
    std::array<Set<Policy>, CACHE_SETS> sets;                           //Cache sets
    uint32_t hits = 0;                                                  //Cache hit
    uint32_t total = 0;                                                 //Total requests
};

//Cpu
class CPU {
public:
    //Init
    CPU()
        : pc(BASE)
    {
        registers[0] = 0;
    }
    //Save results in register
    void SetDestination(uint32_t rd, int32_t data) {
        if (rd == 0) {
            //cant edit zero
            return;
        }
        registers[rd] = data;
    }
    //Emulation of execution of a program defined as a binary array of instructions
    template<int Policy>
    double Execute(const std::vector<uint32_t>& program) {
        Cache<Policy> cache;
        registers[ABI_REG["ra"]] = 0x30000;                 //set return address
        pc = BASE;                                          //pc register in default address of loading

        while (true) {
            if (pc < BASE || pc >= BASE + program.size() * 4) {
                break;
            }
            uint32_t instruction = program[(pc - BASE) / 4];
            uint32_t opcode = GetOPCode(instruction);
            if (opcode == IMM) {
                // ADDI SLTI SLTIU XORI ORI ANDI SLLI SRLI SRAI
                //I-type
                uint32_t funct3 = GetFunct3(instruction);
                uint32_t funct7 = GetFunct7(instruction);
                uint32_t rd = GetRD(instruction);
                int32_t imm = GetImmI(instruction);
                int32_t rs1 = registers[GetRS1(instruction)];
                int64_t result = 0;

                if (funct3 == F3_ADDI) {
                    result = rs1 + imm;
                } else if (funct3 == F3_SLTI) {
                    if (rs1 < imm) {
                        result = 1;
                    }
                } else if (funct3 == F3_SLTIU) {
                    if (static_cast<uint32_t>(rs1) < static_cast<uint32_t>(imm)) {
                        result = 1;
                    }
                } else if (funct3 == F3_ANDI) {
                    result = rs1 & imm;
                } else if (funct3 == F3_ORI) {
                    result = rs1 | imm;
                } else if (funct3 == F3_XORI) {
                    result = rs1 ^ imm;
                } else if (funct3 == F3_SLLI && funct7 == F7_SLLI) {
                    uint32_t shamt = imm & 0b11111;
                    result = static_cast<int64_t>(rs1) << shamt;
                } else if (funct3 == F3_SRLI && funct7 == F7_SRLI) {
                    uint32_t shamt = imm & 0b11111;
                    result = static_cast<uint32_t>(rs1) >> shamt;
                } else if (funct3 == F3_SRAI && funct7 == F7_SRAI) {
                    //save sign right shift
                    uint32_t shamt = imm & 0b11111;
                    result = rs1 >> shamt;
                }
                pc += 4;
                SetDestination(rd, static_cast<int32_t>(result));
            } else if (opcode == OP) {
                //REG-REG
                uint32_t funct3 = GetFunct3(instruction);
                uint32_t funct7 = GetFunct7(instruction);
                uint32_t rd = GetRD(instruction);
                int32_t rs1 = registers[GetRS1(instruction)];
                int32_t rs2 = registers[GetRS2(instruction)];
                int64_t result = 0;
                if (funct3 == F3_ADD && funct7 == F7_ADD) {
                    result = rs1 + rs2;
                } else if (funct3 == F3_SUB && funct7 == F7_SUB) {
                    result = rs1 - rs2;
                } else if (funct3 == F3_SLT && funct7 == F7_SLT) {
                    if (rs1 < rs2) {
                        result = 1;
                    }
                } else if (funct3 == F3_SLTU && funct7 == F7_SLTU) {
                    if (static_cast<uint32_t>(rs1) < static_cast<uint32_t>(rs2)) {
                        result = 1;
                    }
                } else if (funct3 == F3_AND && funct7 == F7_AND) {
                    result = rs1 & rs2;
                } else if(funct3 == F3_OR && funct7 == F7_OR) {
                    result = rs1 | rs2;
                } else if(funct3 == F3_XOR && funct7 == F7_XOR) {
                    result = rs1 ^ rs2;
                } else if (funct3 == F3_SLL && funct7 == F7_SLL) {
                    uint32_t shamt = rs2 & 0b11111;
                    result = static_cast<int64_t>(rs1) << shamt;
                } else if (funct3 == F3_SRL && funct7 == F7_SRL) {
                    uint32_t shamt = rs2 & 0b11111;
                    result = static_cast<uint32_t>(rs1) >> shamt;
                } else if (funct3 == F3_SRA && funct7 == F7_SRA) {
                    //save sign right shift
                    uint32_t shamt = rs2 & 0b11111;
                    result = rs1 >> shamt;
                } else if (funct7 == F7_MUL) {
                    if (funct3 == F3_MUL) {
                        result = static_cast<int64_t>(rs1) * static_cast<int64_t>(rs2);
                    } else if (funct3 == F3_MULH) {
                        result = (static_cast<int64_t>(rs1) * static_cast<int64_t>(rs2)) >> 32;
                    } else if (funct3 == F3_MULHU) {
                        result = (static_cast<uint64_t>(rs1) * static_cast<uint64_t>(rs2)) >> 32;
                    } else if (funct3 == F3_MULHSU) {
                        result = (static_cast<int64_t>(rs1) * static_cast<uint64_t>(rs2)) >> 32;
                    } else if (funct3 == F3_DIV) {
                        result = static_cast<int64_t>(rs1) / static_cast<int64_t>(rs2);
                    } else if (funct3 == F3_DIVU) {
                        result = static_cast<uint64_t>(rs1) / static_cast<uint64_t>(rs2);
                    } else if (funct3 == F3_REM) {
                        result = static_cast<int64_t>(rs1) % static_cast<int64_t>(rs2);
                    } else if (funct3 == F3_REMU) {
                        result = static_cast<uint64_t>(rs1) % static_cast<uint64_t>(rs2);
                    }
                }
                pc += 4;
                SetDestination(rd, static_cast<int32_t>(result));
            } else if (opcode == LUI) {
                uint32_t rd = GetRD(instruction);
                int32_t imm = GetImmU(instruction);
                int32_t result = imm << 12;
                pc += 4;
                SetDestination(rd, result);
            } else if (opcode == AUIPC) {
                uint32_t rd = GetRD(instruction);
                int32_t imm = GetImmU(instruction);
                int32_t result = static_cast<int32_t>(pc) + (imm << 12);
                pc += 4;
                SetDestination(rd, result);
            } else if (opcode == JAL) {
                uint32_t rd = GetRD(instruction);
                int32_t imm = GetImmJ(instruction);
                SetDestination(rd, pc + 4);
                pc += imm;
            } else if (opcode == JALR) {
                uint32_t rd = GetRD(instruction);
                int32_t imm = GetImmI(instruction);
                int32_t rs1 = registers[GetRS1(instruction)];
                SetDestination(rd, pc + 4);
                pc = (rs1 + imm) & 0b11111111111111111111111111111110;
            } else if (opcode == BRANCH) {
                // BEQ BNE BLT BGE BLTU BGEU
                int32_t rs1 = registers[GetRS1(instruction)];
                int32_t rs2 = registers[GetRS2(instruction)];
                int32_t imm = GetImmB(instruction);
                uint32_t funct3 = GetFunct3(instruction);
                if (funct3 == F3_BEQ) {
                    if (rs1 == rs2) {
                        pc += imm;
                    } else {
                        pc += 4;
                    }
                } else if (funct3 == F3_BNE) {
                    if (rs1 != rs2) {
                        pc += imm;
                    } else {
                        pc += 4;
                    }
                } else if (funct3 == F3_BLT) {
                    if (rs1 < rs2) {
                        pc += imm;
                    } else {
                        pc += 4;
                    }
                } else if (funct3 == F3_BLTU) {
                    if (static_cast<uint32_t>(rs1) < static_cast<uint32_t>(rs2)) {
                        pc += imm;
                    } else {
                        pc += 4;
                    }
                } else if (funct3 == F3_BGE) {
                    if (rs1 >= rs2) {
                        pc += imm;
                    } else {
                        pc += 4;
                    }
                } else if (funct3 == F3_BGEU) {
                    if (static_cast<uint32_t>(rs1) >= static_cast<uint32_t>(rs2)) {
                        pc += imm;
                    } else {
                        pc += 4;
                    }
                }
            } else if (opcode == LOAD) {
                //LB LH LW LBU LHU
                uint32_t rd = GetRD(instruction);
                uint32_t funct3 = GetFunct3(instruction);
                int32_t rs1 = registers[GetRS1(instruction)];
                int32_t imm = GetImmI(instruction);
                auto address = static_cast<uint32_t>(rs1 + imm);
                int32_t result = 0;
                if (funct3 == F3_LW) {
                    result = cache.template read<int32_t>(address);
                } else if (funct3 == F3_LH) {
                    result = cache.template read<int16_t>(address);
                } else if (funct3 == F3_LHU) {
                    result = cache.template read<int16_t>(address);
                    result &= 0b1111111111111111; //zero extends
                } else if (funct3 == F3_LB) {
                    result = cache.template read<int8_t>(address);
                } else if (funct3 == F3_LBU) {
                    result = cache.template read<int8_t>(address);
                    result &= 0b11111111; //zero extends
                }
                pc += 4;
                SetDestination(rd, result);
            } else if (opcode == STORE) {
                //SB SH SW
                uint32_t funct3 = GetFunct3(instruction);
                int32_t rs1 = registers[GetRS1(instruction)];
                int32_t rs2 = registers[GetRS2(instruction)];
                int32_t imm = GetImmS(instruction);
                auto address = static_cast<uint32_t>(rs1 + imm);
                if (funct3 == F3_SW) {
                    cache.template write<int32_t>(address, rs2);
                } else if (funct3 == F3_SH) {
                    cache.template write<int16_t>(address, rs2 & 0b1111111111111111);
                } else if (funct3 == F3_SB) {
                    cache.template write<int8_t>(address, rs2 & 0b11111111);
                }
                pc += 4;
            }
        }
        return static_cast<double>(cache.hits) / cache.total * 100;
    }



    std::array<int32_t, 32> registers;                               //all registers
    uint32_t pc;                                                     //pc register
    static constexpr int BASE = 0x20000;                                //Loading address
};
