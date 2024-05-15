#pragma once

#include <stdint.h>

/// @brief 2D Vector of uint64_t
typedef struct {
    uint64_t x, y;
} ac_vec2ui_t;

/// @brief 2D Vector of int64_t
typedef struct {
    int64_t x, y;
} ac_vec2i_t;

/// @brief 2D Vector of float
typedef struct {
    float x, y;
} ac_vec2f_t;

typedef struct {
    float x, y, z;
} ac_vec3f_t;

typedef struct {
    float x, y, z, w;
} ac_vec4f_t;

float heim_deg_to_rad(float deg);

/// @brief Add two HeimVec2f vectors
/// @param a vec 1
/// @param b vec 2
/// @return
ac_vec2f_t ac_vec2f_add(ac_vec2f_t a, ac_vec2f_t b);
ac_vec2f_t ac_vec2f_sub(ac_vec2f_t a, ac_vec2f_t b);

ac_vec3f_t ac_vec3f_add(ac_vec3f_t a, ac_vec3f_t b);
ac_vec3f_t ac_vec3f_sub(ac_vec3f_t a, ac_vec3f_t b);
ac_vec3f_t ac_vec3f_mul(ac_vec3f_t a, float b);

float ac_vec3f_dot(ac_vec3f_t a, ac_vec3f_t b);
ac_vec3f_t ac_vec3f_cross(ac_vec3f_t a, ac_vec3f_t b);
ac_vec3f_t ac_vec3f_normalize(ac_vec3f_t a);

ac_vec3f_t ac_vec3f_from_vec4f(ac_vec4f_t a);
ac_vec3f_t ac_vec3f_mix(ac_vec3f_t a, ac_vec3f_t b, float t);

ac_vec4f_t ac_vec4f_from_vec3f(ac_vec3f_t a);
float ac_vec4f_dot(ac_vec4f_t a, ac_vec4f_t b);
ac_vec4f_t ac_vec4f_mix(ac_vec4f_t a, ac_vec4f_t b, float t);
ac_vec4f_t ac_vec4f_normalize(ac_vec4f_t a);
ac_vec4f_t ac_vec4f_mul(ac_vec4f_t a, float b);
ac_vec4f_t ac_quat_slerp(ac_vec4f_t a, ac_vec4f_t b, float t);
