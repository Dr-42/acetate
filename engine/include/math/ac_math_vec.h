#pragma once

/**
 * @file ac_math_vec.h
 * @brief Vector math functions
 */

#include <stdint.h>

/** A 2D vector of uint64_t
 * @brief 2D Vector of uint64_t
 */
typedef struct {
    /** X component */
    uint64_t x;
    /** Y component */
    uint64_t y;
} ac_vec2ui_t;

/** A 2D vector of int64_t
 * @brief 2D Vector of int64_t
 */
typedef struct {
    /** X component */
    int64_t x;
    /** Y component */
    int64_t y;
} ac_vec2i_t;

/** A 2D vector of float
 * @brief 2D Vector of float
 */
typedef struct {
    /** X component */
    float x;
    /** Y component */
    float y;
} ac_vec2f_t;

/** A 3D vector of float
 * @brief 3D Vector of float
 */
typedef struct {
    /** X component */
    float x;
    /** Y component */
    float y;
    /** Z component */
    float z;
} ac_vec3f_t;

/** A 4D vector of float
 * @brief 4D Vector of float
 */
typedef struct {
    /** X component */
    float x;
    /** Y component */
    float y;
    /** Z component */
    float z;
    /** W component */
    float w;
} ac_vec4f_t;

/** Add two 2D vectors of float
 * @brief Add two 2D vectors of float
 * @param a First vector
 * @param b Second vector
 * @return Sum of the two vectors
 */
ac_vec2f_t ac_vec2f_add(ac_vec2f_t a, ac_vec2f_t b);

/** Subtract two 2D vectors of float
 * @brief Subtract two 2D vectors of float
 * @param a First vector
 * @param b Second vector
 * @return Difference of the two vectors
 */
ac_vec2f_t ac_vec2f_sub(ac_vec2f_t a, ac_vec2f_t b);

/** Add two 3D vectors of float
 * @brief Add two 3D vectors of float
 * @param a First vector
 * @param b Second vector
 * @return Sum of the two vectors
 */
ac_vec3f_t ac_vec3f_add(ac_vec3f_t a, ac_vec3f_t b);

/** Subtract two 3D vectors of float
 * @brief Subtract two 3D vectors of float
 * @param a First vector
 * @param b Second vector
 * @return Difference of the two vectors
 */
ac_vec3f_t ac_vec3f_sub(ac_vec3f_t a, ac_vec3f_t b);

/** Multiply a 3D vector by a scalar
 * @brief Multiply a 3D vector by a scalar
 * @param a Vector
 * @param b Scalar
 * @return Product of the vector and scalar
 */
ac_vec3f_t ac_vec3f_mul(ac_vec3f_t a, float b);

/** Get the dot product of two 3D vectors
 * @brief Get the dot product of two 3D vectors
 * @param a First vector
 * @param b Second vector
 * @return Dot product of the two vectors
 */
float ac_vec3f_dot(ac_vec3f_t a, ac_vec3f_t b);

/** Get the cross product of two 3D vectors
 * @brief Get the cross product of two 3D vectors
 * @param a First vector
 * @param b Second vector
 * @return Cross product of the two vectors
 */
ac_vec3f_t ac_vec3f_cross(ac_vec3f_t a, ac_vec3f_t b);

/** Get the normal vector of a 3D vector
 * @brief Get the normal vector of a 3D vector
 * @param a Vector
 * @return Normal vector of the vector
 */
ac_vec3f_t ac_vec3f_normalize(ac_vec3f_t a);

/** Get a 3D vector from a 4D vector
 * @brief Get a 3D vector from a 4D vector
 * @param a Vector
 * @return 3D vector
 */
ac_vec3f_t ac_vec3f_from_vec4f(ac_vec4f_t a);

/** Mix two 3D vectors
 * @brief Mix two 3D vectors
 * @param a First vector
 * @param b Second vector
 * @param t Mix factor
 * @return Mixed vector
 */
ac_vec3f_t ac_vec3f_mix(ac_vec3f_t a, ac_vec3f_t b, float t);

/** Get a 4D vector from a 3D vector
 * @brief Get a 4D vector from a 3D vector
 * @param a Vector
 * @return 4D vector
 */
ac_vec4f_t ac_vec4f_from_vec3f(ac_vec3f_t a);

/** Get the dot product of two 4D vectors
 * @brief Get the dot product of two 4D vectors
 * @param a First vector
 * @param b Second vector
 * @return Dot product of the two vectors
 */
float ac_vec4f_dot(ac_vec4f_t a, ac_vec4f_t b);

/** Mix two 4D vectors
 * @brief Mix two 4D vectors
 * @param a First vector
 * @param b Second vector
 * @param t Mix factor
 * @return Mixed vector
 */
ac_vec4f_t ac_vec4f_mix(ac_vec4f_t a, ac_vec4f_t b, float t);

/** Get the normal vector of a 4D vector
 * @brief Get the normal vector of a 4D vector
 * @param a Vector
 * @return Normal vector of the vector
 */
ac_vec4f_t ac_vec4f_normalize(ac_vec4f_t a);

/** Multiply a 4D vector by a scalar
 * @brief Multiply a 4D vector by a scalar
 * @param a Vector
 * @param b Scalar
 * @return Product of the vector and scalar
 */
ac_vec4f_t ac_vec4f_mul(ac_vec4f_t a, float b);

/** Spherical linear interpolation between two 4D vectors (quaternions)
 * @brief Spherical linear interpolation between two Quaternions
 * @param a First vector
 * @param b Second vector
 * @param t Interpolation factor
 * @return Interpolated vector
 */
ac_vec4f_t ac_quat_slerp(ac_vec4f_t a, ac_vec4f_t b, float t);
