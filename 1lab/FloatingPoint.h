#pragma once
#include "FixedPoint.h"
#include <string>

const std::string kDefault = "default";

struct FloatingOneValue;

struct InputFloatingData {
    char precision{};

    char round_code = '0';
    char operation = kDefaultOperation;

    uint64_t first_value{};
    uint64_t second_value{};
};

uint32_t ParseOneFloating(std::string& value);

InputFloatingData ParseFloatingPoint(int argc, char** argv);

struct FloatingOneValue {
    FloatingOneValue() = default;
    FloatingOneValue(uint64_t cur_value, char cur_precision);
    FloatingOneValue(char cur_precision, uint8_t cur_sign, uint64_t cur_mantissa, int32_t cur_exponent);

    uint64_t GetFullMantissa() const;
    void CorrectValue();

    uint64_t value{};
    char precision;

    uint32_t exponent_size;
    uint32_t mantissa_size;

    uint8_t sign;
    int32_t exponent;
    uint64_t mantissa; // control overflowing

    std::string special_meanings = kDefault;
    bool subnormal = false;

    FloatingOneValue operator+(const FloatingOneValue& other) const;

    FloatingOneValue operator-(const FloatingOneValue& other) const;

    FloatingOneValue operator*(const FloatingOneValue& other) const;

    FloatingOneValue operator/(const FloatingOneValue& other) const;

};

void RunFloatingPoint(int argc, char** argv);

std::ostream& operator<<(std::ostream &ostream, FloatingOneValue& value);
