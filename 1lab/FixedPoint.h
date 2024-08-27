#pragma once
#include <iostream>
#include <cstdint>
#include <string>
#include <cmath>

const uint32_t kRoundingFactor = 1000;
static const char kDefaultOperation = '?';

uint32_t ParseOneFixed(std::string& value, uint32_t main_bits, uint32_t fractional_bits);

std::pair<uint8_t ,uint8_t> ParseBits(std::string& all_bits);

struct InputFixedData {
    uint8_t main_bits = 0;
    uint8_t fractional_bits = 0;

    char round_code = 0;
    char operation = kDefaultOperation;

    uint64_t first_value{};
    uint64_t second_value{};
};

InputFixedData ParseFixedPoint(int argc, char** argv);

struct FixedOneValue{
    FixedOneValue() = default;
    FixedOneValue(uint64_t cur_val, uint8_t main, uint8_t fractional);

    uint8_t GetSign() const;
    uint32_t InAdditional(uint32_t cur_val) const;


    uint32_t value{};
    uint8_t main_bits{};
    uint8_t fractional_bits{};

    uint32_t main_part = 0;
    uint32_t fractional_part = 0;

    FixedOneValue operator+(const FixedOneValue& other) const;

    FixedOneValue operator-(const FixedOneValue& other) const;

    FixedOneValue operator*(const FixedOneValue& other) const;

    FixedOneValue operator/(const FixedOneValue& other) const;


};

std::ostream& operator<<(std::ostream &ostream, FixedOneValue& value);

void RunFixedPoint(int argc, char** argv);
