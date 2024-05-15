#ifndef AC_MATH_MAT_H
#define AC_MATH_MAT_H

#include "math/ac_math_vec.h"

typedef struct ac_mat4_t {
    float m[4][4] __attribute__((aligned(16)));
} __attribute__((aligned(16))) ac_mat4_t;

typedef struct ac_mat3_t {
    float m[3][3] __attribute__((aligned(16)));
} __attribute__((aligned(16))) ac_mat3_t;

void ac_mat4_print(ac_mat4_t mat);

ac_mat4_t ac_mat4_identity();

ac_mat4_t ac_mat4_translate(ac_mat4_t m, ac_vec3f_t vec);

ac_mat4_t ac_mat4_rotate(ac_mat4_t m, float degrees, ac_vec3f_t axis);

ac_mat4_t ac_mat4_scale(ac_mat4_t m, ac_vec3f_t vec);

ac_mat4_t ac_mat4_multiply(ac_mat4_t a, ac_mat4_t b);

ac_vec3f_t ac_mat4_transform_vec3(ac_mat4_t mat, ac_vec3f_t vec);

ac_mat4_t ac_mat4_ortho(float left, float right, float bottom, float top,
                        float near, float far);
ac_mat4_t ac_mat4_perspective(float fov, float aspect, float near, float far);
ac_mat4_t ac_mat4_lookat(ac_vec3f_t eye, ac_vec3f_t target, ac_vec3f_t up);

ac_mat3_t ac_mat3_identity();
ac_mat3_t ac_mat3_transpose(ac_mat3_t mat);
ac_mat3_t ac_mat3_inverse(ac_mat3_t mat);
ac_mat3_t ac_mat3_from_mat4(ac_mat4_t mat);

ac_mat4_t ac_mat4_from_quat(ac_vec4f_t quat);

#endif  // AC_MATH_MAT_H
