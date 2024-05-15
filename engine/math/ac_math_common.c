#include <math.h>

#include "math/ac_math_common.h"

float ac_math_deg_to_rad(float deg) { return deg * AC_PI / 180.0f; }
float ac_math_rad_to_deg(float rad) { return rad * 180.0f / AC_PI; }

float ac_math_clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
float ac_math_lerp(float a, float b, float t) { return a + (b - a) * t; }

float ac_math_min(float a, float b) { return a < b ? a : b; }
float ac_math_max(float a, float b) { return a > b ? a : b; }

float ac_math_abs(float a) { return a < 0.0f ? -a : a; }

float ac_math_sin(float a) { return sinf(a); }
float ac_math_cos(float a) { return cosf(a); }
float ac_math_tan(float a) { return tanf(a); }

float ac_math_asin(float a) { return asinf(a); }
float ac_math_acos(float a) { return acosf(a); }
float ac_math_atan(float a) { return atanf(a); }

float ac_math_sqrtf(float a) { return sqrtf(a); }
