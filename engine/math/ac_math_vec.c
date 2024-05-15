#include "math/ac_math_common.h"
#include "math/ac_math_vec.h"

ac_vec2f_t ac_vec2f_add(ac_vec2f_t a, ac_vec2f_t b) {
    return (ac_vec2f_t){a.x + b.x, a.y + b.y};
}

ac_vec2f_t ac_vec2f_sub(ac_vec2f_t a, ac_vec2f_t b) {
    return (ac_vec2f_t){a.x - b.x, a.y - b.y};
}

ac_vec3f_t ac_vec3f_add(ac_vec3f_t a, ac_vec3f_t b) {
    return (ac_vec3f_t){a.x + b.x, a.y + b.y, a.z + b.z};
}

ac_vec3f_t ac_vec3f_sub(ac_vec3f_t a, ac_vec3f_t b) {
    return (ac_vec3f_t){a.x - b.x, a.y - b.y, a.z - b.z};
}

ac_vec3f_t ac_vec3f_mul(ac_vec3f_t a, float b) {
    return (ac_vec3f_t){a.x * b, a.y * b, a.z * b};
}

float ac_vec3f_dot(ac_vec3f_t a, ac_vec3f_t b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

ac_vec3f_t ac_vec3f_cross(ac_vec3f_t a, ac_vec3f_t b) {
    return (ac_vec3f_t){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                        a.x * b.y - a.y * b.x};
}

ac_vec3f_t ac_vec3f_normalize(ac_vec3f_t a) {
    float length = ac_math_sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    return (ac_vec3f_t){a.x / length, a.y / length, a.z / length};
}

ac_vec3f_t ac_vec3f_from_vec4f(ac_vec4f_t a) {
    return (ac_vec3f_t){a.x, a.y, a.z};
}

ac_vec3f_t ac_vec3f_mix(ac_vec3f_t a, ac_vec3f_t b, float t) {
    return (ac_vec3f_t){a.x * (1.0f - t) + b.x * t, a.y * (1.0f - t) + b.y * t,
                        a.z * (1.0f - t) + b.z * t};
}

ac_vec4f_t ac_vec4f_normalize(ac_vec4f_t a) {
    float length = ac_math_sqrt(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w);
    return (ac_vec4f_t){a.x / length, a.y / length, a.z / length, a.w / length};
}

ac_vec4f_t ac_vec4f_mix(ac_vec4f_t a, ac_vec4f_t b, float t) {
    return (ac_vec4f_t){a.x * (1.0f - t) + b.x * t, a.y * (1.0f - t) + b.y * t,
                        a.z * (1.0f - t) + b.z * t, a.w * (1.0f - t) + b.w * t};
}

float ac_vec4f_dot(ac_vec4f_t a, ac_vec4f_t b) {
    return (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
}

ac_vec4f_t ac_vec4f_from_vec3f(ac_vec3f_t a) {
    return (ac_vec4f_t){a.x, a.y, a.z, 0.0f};
}

ac_vec4f_t ac_vec4f_mul(ac_vec4f_t a, float b) {
    return (ac_vec4f_t){a.x * b, a.y * b, a.z * b, a.w * b};
}

ac_vec4f_t ac_quat_slerp(ac_vec4f_t a, ac_vec4f_t b, float t) {
    ac_vec4f_t result = {0};
    float dot = ac_vec4f_dot(a, b);
    if (dot < 0.0f) {
        b = ac_vec4f_mul(b, -1.0f);
        dot = -dot;
    }
    if (dot > 0.9995f) {
        result = ac_vec4f_mix(a, b, t);
        return result;
    }
    float theta_0 = ac_math_acos(dot);
    float theta = theta_0 * t;
    float sin_theta = ac_math_sin(theta);
    float sin_theta_0 = ac_math_sin(theta_0);
    float s0 = ac_math_cos(theta) - dot * sin_theta / sin_theta_0;
    float s1 = sin_theta / sin_theta_0;
    result.x = s0 * a.x + s1 * b.x;
    result.y = s0 * a.y + s1 * b.y;
    result.z = s0 * a.z + s1 * b.z;
    result.w = s0 * a.w + s1 * b.w;
    return result;
}
