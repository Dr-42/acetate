#ifndef AC_MATH_COMMON_H
#define AC_MATH_COMMON_H

/**
 * @file ac_math_common.h
 * @brief Common math functions.
 *
 * This file contains common math functions.
 */

#include <stdint.h>

/**
 * @brief Pi constant.
 */
#define AC_PI 3.14159265358979323846f

/**
 * Convert degrees to radians.
 * @param deg Degrees.
 * @return Radians.
 */
float ac_math_deg_to_rad(float deg);

/**
 * Convert radians to degrees.
 * @param rad Radians.
 * @return Degrees.
 */
float ac_math_rad_to_deg(float rad);

/**
 * Clamp a value between a minimum and maximum.
 * @param value Value to clamp.
 * @param min Minimum value.
 * @param max Maximum value.
 * @return Clamped value.
 */
float ac_math_clamp(float value, float min, float max);

/**
 * Linear interpolation between two values.
 * @param a First value.
 * @param b Second value.
 * @param t Interpolation factor.
 * @return Interpolated value.
 */
float ac_math_lerp(float a, float b, float t);

/**
 * @brief Calculate the minimum of two values.
 * @param a First value.
 * @param b Second value.
 * @return Minimum value.
 */
float ac_math_min(float a, float b);

/**
 * @brief Calculate the maximum of two values.
 * @param a First value.
 * @param b Second value.
 * @return Maximum value.
 */
float ac_math_max(float a, float b);

/**
 * @brief Calculate the absolute value of a number.
 * @param a Number.
 * @return Absolute value.
 */
float ac_math_abs(float a);

/**
 * @brief Calculate the sine of a number.
 * @param a Number.
 * @return Sine value.
 */
float ac_math_sin(float a);

/**
 * @brief Calculate the cosine of a number.
 * @param a Number.
 * @return Cosine value.
 */
float ac_math_cos(float a);

/**
 * @brief Calculate the tangent of a number.
 * @param a Number.
 * @return Tangent value.
 */
float ac_math_tan(float a);

/**
 * @brief Calculate the arcsine of a number.
 * @param a Number.
 * @return Arcsine value.
 */
float ac_math_asin(float a);

/**
 * @brief Calculate the arccosine of a number.
 * @param a Number.
 * @return Arccosine value.
 */
float ac_math_acos(float a);

/**
 * @brief Calculate the arctangent of a number.
 * @param a Number.
 * @return Arctangent value.
 */
float ac_math_atan(float a);

/**
 * @brief Calculate the square root of a number.
 * @param a Number.
 * @return Square root value.
 */
float ac_math_sqrt(float a);

#endif  // AC_MATH_COMMON_H
