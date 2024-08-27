#include "hit.h"
#include <cmath>

//We have Piriform Surface: (x^4 - 2x^3) + 4(y^2 + z^2) = 0
// Returns true if specified coordinates are inside the object
bool hit_test(float x, float y, float z) {
    float pow_x_3 = x * x * x;
    float pow_x_4 = pow_x_3 * x;
    bool test = ((pow_x_4 - 2 * pow_x_3) + 4 * (y * y + z * z) <= 0);
    return test;
}


// Returns array of 6 values:
// x_min, x_max, y_min, y_max, z_min, z_max
const float* get_axis_range() {
    static float range[6] = {0.0f, 2.0f, -0.65f, 0.65f, -0.65f, 0.65f};
    return range;
}