#include "FloatingPoint.h"

uint32_t ParseOneFloating(std::string& value) {
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

    int_value = std::stoll(value, nullptr, 16);

    return int_value;
}

InputFloatingData ParseFloatingPoint(int argc, char** argv) {
    if (argc > 6 || argc == 5) {
        std::cerr << "Incorrect options";
        exit(EXIT_FAILURE);

    }
    InputFloatingData data;

    if (argv[1][0] != 'h' && argv[1][0] != 'f') {
        std::cerr << "Incorrect precision : " << argv[1][0];
        exit(EXIT_FAILURE);
    }

    data.precision = argv[1][0];


    if (argv[2][0] != '0') {
        std::cerr << "Incorrect round code :" << argv[2][0];
        exit(EXIT_FAILURE);
    }

    data.round_code = argv[2][0];

    std::string first_value = argv[3];
    data.first_value = ParseOneFloating(first_value);

    if (argc > 4) {
        if (argv[4][0] == '+' || argv[4][0] == '-' || argv[4][0] == '*' || argv[4][0] == '/') {
            data.operation = argv[4][0];
        } else {
            std::cerr << "Incorrect operation with value : " << argv[4][0];
            exit(EXIT_FAILURE);
        }

        std::string second_value = argv[5];
        data.second_value = ParseOneFloating(second_value);
    }

    return data;
}

FloatingOneValue::FloatingOneValue(uint64_t cur_value, char cur_precision)
    : precision(cur_precision)
    , value(cur_value)
{
    if (cur_precision == 'h') {
        mantissa_size = 10;
        exponent_size = 5;
    } else {
        mantissa_size = 23;
        exponent_size = 8;
    }

    sign = (cur_value >> (mantissa_size + exponent_size)); // get sign
    exponent = static_cast<int32_t>((cur_value >> mantissa_size) & ((1 << exponent_size) - 1)); // get exponent
    mantissa = (cur_value & ((1 << mantissa_size) - 1)); // get mantissa

    if (exponent == 0 && mantissa == 0) {
        special_meanings = "0";
    } else if (exponent == 0 && mantissa != 0) {
        subnormal = true;
        exponent = 1;
    } else if ((exponent == ((1 << exponent_size) - 1)) && mantissa == 0) {
        special_meanings = "inf";
    } else if ((exponent == ((1 << exponent_size) - 1)) && mantissa != 0) {
        special_meanings = "nan";
    }
}

FloatingOneValue FloatingOneValue::operator*(const FloatingOneValue &other) const {
    if (special_meanings == "nan" || other.special_meanings == "nan") {
        return FloatingOneValue(precision, 0, 1, (1 << exponent_size) - 1);
    }
    if ((special_meanings == "0" && other.special_meanings == "inf") || (special_meanings == "inf" && other.special_meanings == "0")) {
        return FloatingOneValue(precision, 0, 1, (1 << exponent_size) - 1);

    }
    if ((special_meanings == "0" && other.special_meanings != "inf") || special_meanings != "inf" && other.special_meanings == "0") {
        return FloatingOneValue(precision, 0, 0, 0);

    }

    if ((special_meanings != "0" && other.special_meanings == "inf") || special_meanings == "inf" && other.special_meanings != "0") {
        return FloatingOneValue(precision, sign ^ other.sign, 0, (1 << exponent_size) - 1);

    }

    int32_t sum_exp = exponent + other.exponent;
    sum_exp -= (precision == 'h' ? 15 : 127);

    uint64_t new_mantissa = (GetFullMantissa() * other.GetFullMantissa()) >> mantissa_size;
    uint8_t new_sign = sign ^ other.sign;

    FloatingOneValue result = FloatingOneValue(precision, new_sign, new_mantissa, sum_exp);
    result.CorrectValue();

    return result;

}

uint64_t FloatingOneValue::GetFullMantissa() const {
    if (subnormal || special_meanings == "0") {
        return mantissa;
    }

    return (1 << mantissa_size) | mantissa;
}

FloatingOneValue::FloatingOneValue(char cur_precision, uint8_t cur_sign, uint64_t cur_mantissa, int32_t cur_exponent)
    : precision(cur_precision)
    , sign(cur_sign)
    , mantissa(cur_mantissa)
    , exponent(cur_exponent)

{
    if (cur_precision == 'h') {
        mantissa_size = 10;
        exponent_size = 5;
    } else {
        mantissa_size = 23;
        exponent_size = 8;
    }

    if (exponent == 0 && mantissa == 0) {
        special_meanings = "0";
    } else if (exponent == 0 && mantissa != 0) {
        subnormal = true;
        exponent = 1;
    } else if ((exponent == ((1 << exponent_size) - 1)) && mantissa == 0) {
        special_meanings = "inf";
    } else if ((exponent == ((1 << exponent_size) - 1)) && mantissa != 0) {
        special_meanings = "nan";
    }
}

void FloatingOneValue::CorrectValue() {
    int32_t max_exponent = (1 << exponent_size) - 2; //without specials
    int32_t min_exponent = 1;
    uint64_t max_mantissa = (1 << mantissa_size) - 1; //111...11


    while (exponent < min_exponent || (mantissa >> (mantissa_size + 1)) > 0) {
        mantissa >>= 1;
        ++exponent;
    }
    if (exponent > max_exponent) {
        mantissa = max_mantissa;
        exponent = max_exponent;
    }


    while (exponent > min_exponent && ((mantissa >> mantissa_size) & 1) == 0) {
        mantissa <<= 1;
        --exponent;
    }


    if (exponent == min_exponent) {
        if (mantissa != 0) {
            subnormal = true;
        } else {
            exponent = (1 << ( exponent_size - 1)) - 1;
            special_meanings = "0";

        }
    }

    mantissa &= (1 << mantissa_size) - 1;
}

FloatingOneValue FloatingOneValue::operator/(const FloatingOneValue &other) const {
    if (special_meanings == "nan" || other.special_meanings == "nan") {
        return FloatingOneValue(precision, 0, 1, (1 << exponent_size) - 1);
    }
    if (special_meanings == "0" && other.special_meanings == "0") {
        return FloatingOneValue(precision, 0, 1, (1 << exponent_size) - 1);
    }
    if (special_meanings == "inf" && other.special_meanings == "inf") {
        return FloatingOneValue(precision, sign ^ other.sign, 1, (1 << exponent_size) - 1);
    }
    if (special_meanings == "inf") {
        return FloatingOneValue(precision, sign ^ other.sign, 0, (1 << exponent_size) - 1);
    }
    if (other.special_meanings == "0") {
        return FloatingOneValue(precision, sign ^ other.sign, 0, (1 << exponent_size) - 1);
    }
    if (special_meanings == "0") {
        return FloatingOneValue(precision, 0, 0, 0);

    }


    int32_t sum_exp = exponent - other.exponent;
    sum_exp += (precision == 'h' ? 15 : 127);

    uint64_t new_mantissa = (GetFullMantissa() << mantissa_size) / other.GetFullMantissa();
    uint8_t new_sign = sign ^ other.sign;

    FloatingOneValue result = FloatingOneValue(precision, new_sign, new_mantissa, sum_exp);
    result.CorrectValue();

    return result;
}

FloatingOneValue FloatingOneValue::operator+(const FloatingOneValue &other) const {
    if (special_meanings == "nan" || other.special_meanings == "nan") {
        return FloatingOneValue(precision, 0, 1, (1 << exponent_size) - 1);
    }
    if (special_meanings == "inf" && other.special_meanings == "inf") {
        if ((sign ^ other.sign) == 1) {
            return FloatingOneValue(precision, 0, 1, (1 << exponent_size) - 1);
        }
        return FloatingOneValue(precision, sign, 0, (1 << exponent_size) - 1);
    }
    if (special_meanings == "inf") {
        return FloatingOneValue(precision, sign, 0, (1 << exponent_size) - 1);

    }
    if (other.special_meanings == "inf") {
        return FloatingOneValue(precision, other.sign, 0, (1 << exponent_size) - 1);
    }
    if ((sign ^ other.sign) == 1) {
        if (sign) {
            return (other - *this);
        }
        return (*this - other);
    }
    uint64_t first_mantissa = GetFullMantissa();
    uint64_t second_mantissa = other.GetFullMantissa();

    int32_t max_exp = std::max(exponent, other.exponent);

    first_mantissa >>= (max_exp - exponent);
    second_mantissa >>= (max_exp - other.exponent);
    
    uint64_t result_mantissa = first_mantissa + second_mantissa;

    auto result = FloatingOneValue(precision, sign, result_mantissa, max_exp);
    result.CorrectValue();
    return result;


}

FloatingOneValue FloatingOneValue::operator-(const FloatingOneValue &other) const {
    if (special_meanings == "nan" || other.special_meanings == "nan") {
        return FloatingOneValue(precision, 0, 1, (1 << exponent_size) - 1);
    }
    if (special_meanings == "inf" && other.special_meanings == "inf") {
        if ((sign ^ other.sign) == 1) {
            return FloatingOneValue(precision, 0, 1, (1 << exponent_size) - 1);
        }
        return FloatingOneValue(precision, sign, 0, (1 << exponent_size) - 1);
    }
    if (special_meanings == "inf") {
        return FloatingOneValue(precision, sign, 0, (1 << exponent_size) - 1);

    }
    if (other.special_meanings == "inf") {
        return FloatingOneValue(precision, other.sign, 0, (1 << exponent_size) - 1);
    }

    if ((sign ^ other.sign) == 1) {
        if (other.sign) {
            return *this + FloatingOneValue(precision, 0, other.mantissa, other.exponent);
        }
        return *this + FloatingOneValue(precision, 1, other.mantissa, other.exponent);
    }

    uint64_t first_mantissa = GetFullMantissa();
    uint64_t second_mantissa = other.GetFullMantissa();

    int32_t max_exp = std::max(exponent, other.exponent);

    first_mantissa >>= (max_exp - exponent);
    second_mantissa >>= (max_exp - other.exponent);
    uint64_t new_mantissa = std::max(first_mantissa, second_mantissa) - std::min(first_mantissa, second_mantissa);

    uint8_t new_sign = (first_mantissa > second_mantissa ? sign : other.sign ^ 1);
    FloatingOneValue result = FloatingOneValue(precision, new_sign, new_mantissa, max_exp);
    result.CorrectValue();
    return result;

}

std::ostream& operator<<(std::ostream &ostream, FloatingOneValue& value) {
    uint32_t cur_mantissa = value.mantissa;
    int32_t cur_exponent = value.exponent;

    if (value.special_meanings == "nan") {
        ostream << "nan";
        return ostream;
    }

    if (value.special_meanings == "inf") {
        ostream << (value.sign > 0 ? "-inf" : "inf");
        return ostream;
    }

    ostream << (value.sign > 0 ? "-0x" : "0x");
    ostream << (value.special_meanings == "0" ? "0." : "1.");

    if (value.subnormal) {
        while (((cur_mantissa >> value.mantissa_size) & 1) == 0 && cur_mantissa != 0) {
            //until the value becomes zero or the most significant bit of the significant part becomes 1
            cur_mantissa = (cur_mantissa << 1);
            --cur_exponent;
        }
    }

    cur_mantissa &= ((1 << value.mantissa_size) - 1); // the value of the significant part remains within the acceptable size.
    cur_mantissa <<= (value.precision == 'h' ? 2 : 1);

    int32_t bits_count = (value.precision == 'h' ? 12 : 24);

    for (int32_t i = bits_count - 4; i >= 0; i -= 4) {
        ostream << std::hex << ((cur_mantissa >> i) & 0xF);
    }

    ostream << "p";

    if (value.exponent == 0) {
        ostream << "+0";
        return ostream;
    }

    int real_exponent = (value.precision == 'h' ? cur_exponent - 15 : cur_exponent - 127); // val.exp


    if (real_exponent >= 0) {
        ostream << "+" << std::dec << real_exponent;
    } else {
        ostream << std::dec << real_exponent;
    }

    return ostream;

}
void RunFloatingPoint(int argc, char** argv) {
    InputFloatingData data = ParseFloatingPoint(argc, argv);
    FloatingOneValue first_value(data.first_value, data.precision);

    if (data.operation != kDefaultOperation) {
        FloatingOneValue second_value(data.second_value, data.precision);
        FloatingOneValue result;
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
