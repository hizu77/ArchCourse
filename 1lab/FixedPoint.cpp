#include "FixedPoint.h"

uint32_t ParseOneFixed(std::string& value, uint32_t main_bits, uint32_t fractional_bits) {
    if (value.size() < 3 || value[0] != '0' || value[1] != 'x') {
        exit(EXIT_FAILURE);
    }

    uint32_t int_value;

    for (size_t i = 2; i < value.size(); ++i) {
        if (!(value[i] >= 'a' && value[i] <= 'f') && !(value[i] >= '0' && value[i] <= '9')
            && !(value[i] >= 'A' && value[i] <= 'F')) {
            std::cerr << "Incorrect letter in value : " << value[i];
            exit(EXIT_FAILURE);
        }
    }

    int_value = ((std::stoll(value, nullptr, 16)) & ((1ull << (fractional_bits + main_bits)) - 1));
    return int_value;

}

std::pair<uint8_t ,uint8_t> ParseBits(std::string& all_bits) {
    uint8_t main_bits = 0;
    uint8_t fractional_bits = 0;

    bool sign = false;
    for (size_t i = 0; i < all_bits.size(); ++i) {
        if (all_bits[i] == '.') {
            sign = true;
        } else if (!sign) {
            main_bits = main_bits * 10 + (all_bits[i] - '0');
        } else {
            fractional_bits = fractional_bits * 10 + (all_bits[i] - '0');
        }
    }

    return {main_bits, fractional_bits};
}


InputFixedData ParseFixedPoint(int argc, char** argv) {
    if (argc > 6 || argc == 5) {
        std::cerr << "Incorrect options";
        exit(EXIT_FAILURE);
    }
    InputFixedData data;

    std::string all_bits = argv[1];
    std::string first_value = argv[3];

    data.main_bits = ParseBits(all_bits).first;
    data.fractional_bits = ParseBits(all_bits).second;

    if (argv[2][0] != '0') {
        std::cerr << "Incorrect round code :" << argv[2][0];
        exit(EXIT_FAILURE);
    }

    data.round_code = argv[2][0];
    data.first_value = ParseOneFixed(first_value, data.main_bits, data.fractional_bits);

    if (argc > 4) {
        if (argv[4][0] == '+' || argv[4][0] == '-' || argv[4][0] == '*' || argv[4][0] == '/') {
            data.operation = argv[4][0];
        } else {
            std::cerr << "Incorrect operation with value : " << argv[4][0];
            exit(EXIT_FAILURE);
        }

        std::string second_value = argv[5];
        data.second_value = ParseOneFixed(second_value, data.main_bits, data.fractional_bits);
    }

    return data;
}

FixedOneValue::FixedOneValue(uint64_t cur_val, uint8_t main, uint8_t fractional)
        : value(cur_val)
        , main_bits(main)
        , fractional_bits(fractional)
{}

FixedOneValue FixedOneValue::operator+(const FixedOneValue& other) const {
    uint32_t new_value = (value + other.value) & ((1ull << (fractional_bits + main_bits)) - 1); // cut off additional bits
    return {new_value, main_bits, fractional_bits};
}

FixedOneValue FixedOneValue::operator-(const FixedOneValue& other) const {
    uint32_t new_value = (value - other.value) & ((1ull << (fractional_bits + main_bits)) - 1);
    return {new_value, main_bits, fractional_bits};
}

FixedOneValue FixedOneValue::operator*(const FixedOneValue& other) const {
    uint8_t cur_sign = GetSign();
    uint8_t other_sign = other.GetSign();
    uint8_t new_sign = cur_sign ^ other_sign;


    uint32_t cur_value = (cur_sign ? InAdditional(value) : value);
    uint32_t other_value = (other_sign ? InAdditional(other.value) : other.value);

    uint32_t new_value = ((static_cast<uint64_t>(cur_value) * other_value) >> fractional_bits)  & ((1ull << (fractional_bits + main_bits)) - 1);

    uint32_t result_value = (new_sign ? InAdditional(new_value) : new_value);

    return {result_value, main_bits, fractional_bits};
}

uint8_t FixedOneValue::GetSign() const {
    return (value >> (fractional_bits + main_bits - 1)) & 1;
}

uint32_t FixedOneValue::InAdditional(uint32_t cur_val) const {
    return (~cur_val + 1) & ((1ull << (fractional_bits + main_bits)) - 1);
}

FixedOneValue FixedOneValue::operator/(const FixedOneValue& other) const {
    if (other.value == 0) {
        std::cerr << "division by zero";
        exit(0);
    }

    uint8_t cur_sign = GetSign();
    uint8_t other_sign = other.GetSign();
    uint8_t new_sign = cur_sign ^ other_sign;

    uint32_t cur_value = (cur_sign ? InAdditional(value) : value);
    uint32_t other_value = (other_sign ? InAdditional(other.value) : other.value);

    uint32_t new_value = ((static_cast<uint64_t>(cur_value) << fractional_bits) / other_value) & ((1ull << (fractional_bits + main_bits)) - 1);

    uint32_t result_value = (new_sign ? InAdditional(new_value) : new_value);

    return {result_value, main_bits, fractional_bits};
}


std::ostream& operator<<(std::ostream &ostream, FixedOneValue& value) {
    uint8_t sign = value.GetSign();

    uint32_t edited_value = (sign ? value.InAdditional(value.value) : value.value);

    if (sign) {
        ostream << "-";
    }

    value.main_part = (edited_value >> value.fractional_bits);
    value.fractional_part = ((edited_value & ((1ull << value.fractional_bits) - 1)));
    value.fractional_part = (value.fractional_part * kRoundingFactor);
    value.fractional_part >>= value.fractional_bits;


    ostream << value.main_part << ".";

    std::string fractional = std::to_string(value.fractional_part);
    if (fractional.size() == 1) fractional = "00" + fractional;
    if (fractional.size() == 2) fractional = "0" + fractional;

    ostream << fractional;
    return ostream;
}

void RunFixedPoint(int argc, char** argv) {
    InputFixedData data = ParseFixedPoint(argc, argv);
    FixedOneValue first_value(data.first_value, data.main_bits, data.fractional_bits);

    if (data.operation != kDefaultOperation) {
        FixedOneValue second_value(data.second_value, data.main_bits, data.fractional_bits);
        FixedOneValue result;
        if (data.operation == '+') {
            result = first_value + second_value;
        }
        if (data.operation == '-') {
            result = first_value - second_value;
        }
        if (data.operation == '*') {
            result = first_value * second_value;
        }
        if (data.operation == '/') {
            result = first_value / second_value;

        }
        std::cout << result;
    } else {
        std::cout << first_value;
    }
}