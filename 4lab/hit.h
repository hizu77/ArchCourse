#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Returns true if specified coordinates are inside the object
bool hit_test(float x, float y, float z);

// Returns array of 6 values:
// x_min, x_max, y_min, y_max, z_min, z_max
const float* get_axis_range();

#ifdef __cplusplus
}
#endif
