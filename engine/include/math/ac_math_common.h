#ifndef AC_MATH_COMMON_H
#define AC_MATH_COMMON_H

#include <stdint.h>

#define AC_PI 3.14159265358979323846f

float ac_math_deg_to_rad(float deg);
float ac_math_rad_to_deg(float rad);

float ac_math_clamp(float value, float min, float max);
float ac_math_lerp(float a, float b, float t);

float ac_math_min(float a, float b);
float ac_math_max(float a, float b);

float ac_math_abs(float a);

float ac_math_sin(float a);
float ac_math_cos(float a);
float ac_math_tan(float a);

float ac_math_asin(float a);
float ac_math_acos(float a);
float ac_math_atan(float a);

float ac_math_sqrtf(float a);

#endif  // AC_MATH_COMMON_H
