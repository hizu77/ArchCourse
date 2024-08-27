#pragma once
#include "hit.h"
#include <random>
#include <cfloat>

//std::mersenne_twister_engine
//Each thread has its own seed
struct Point {
    Point() = default;
    Point(size_t seed)
        : generator(seed)
    {
        const float* range = get_axis_range();
        x = std::uniform_real_distribution<float>(range[0], range[1] + FLT_MIN);
        y = std::uniform_real_distribution<float>(range[2], range[3] + FLT_MIN);
        z = std::uniform_real_distribution<float>(range[4], range[5] + FLT_MIN);
    }

    bool isHit() {
        return hit_test(x(generator), y(generator), z(generator));
    }

    std::mt19937 generator;
    std::uniform_real_distribution<float> x;
    std::uniform_real_distribution<float> y;
    std::uniform_real_distribution<float> z;
};
