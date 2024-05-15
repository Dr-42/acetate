#include <math.h>
#include <stdio.h>

#include "math/ac_math_common.h"
#include "math/ac_math_mat.h"
#include "math/ac_math_vec.h"

void ac_mat4_print(ac_mat4_t mat) {
    for (size_t i = 0; i < 4; i++) {
        printf("[");
        for (size_t j = 0; j < 4; j++) {
            printf("%f ", mat.m[i][j]);
        }
        printf("]\n");
    }
}

ac_mat4_t ac_mat4_identity() {
    ac_mat4_t mat = {0};
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;
    return mat;
}

ac_mat4_t ac_mat4_translate(ac_mat4_t m, ac_vec3f_t vec) {
    ac_mat4_t res = ac_mat4_identity();
    res.m[3][0] =
        m.m[0][0] * vec.x + m.m[1][0] * vec.y + m.m[2][0] * vec.z + m.m[3][0];
    res.m[3][1] =
        m.m[0][1] * vec.x + m.m[1][1] * vec.y + m.m[2][1] * vec.z + m.m[3][1];
    res.m[3][2] =
        m.m[0][2] * vec.x + m.m[1][2] * vec.y + m.m[2][2] * vec.z + m.m[3][2];
    res.m[3][3] =
        m.m[0][3] * vec.x + m.m[1][3] * vec.y + m.m[2][3] * vec.z + m.m[3][3];
    return res;
}

ac_mat4_t ac_mat4_rotate(ac_mat4_t m, float degrees, ac_vec3f_t axis) {
    ac_mat4_t rot = {0};
    float radians = degrees * AC_PI / 180.0f;
    float c = ac_math_cos(radians);
    float s = ac_math_sin(radians);
    float omc = 1.0f - c;

    ac_vec3f_t norm = ac_vec3f_normalize(axis);
    float x = norm.x;
    float y = norm.y;
    float z = norm.z;

    rot.m[0][0] = x * x * omc + c;
    rot.m[0][1] = y * x * omc + z * s;
    rot.m[0][2] = x * z * omc - y * s;
    rot.m[0][3] = 0.0f;

    rot.m[1][0] = x * y * omc - z * s;
    rot.m[1][1] = y * y * omc + c;
    rot.m[1][2] = y * z * omc + x * s;
    rot.m[1][3] = 0.0f;

    rot.m[2][0] = x * z * omc + y * s;
    rot.m[2][1] = y * z * omc - x * s;
    rot.m[2][2] = z * z * omc + c;
    rot.m[2][3] = 0.0f;

    rot.m[3][0] = 0.0f;
    rot.m[3][1] = 0.0f;
    rot.m[3][2] = 0.0f;
    rot.m[3][3] = 1.0f;

    ac_mat4_t res = ac_mat4_multiply(m, rot);
    return res;
}

ac_mat4_t ac_mat4_scale(ac_mat4_t m, ac_vec3f_t vec) {
    ac_mat4_t scale = ac_mat4_identity();
    scale.m[0][0] = vec.x;
    scale.m[1][1] = vec.y;
    scale.m[2][2] = vec.z;
    ac_mat4_t res = ac_mat4_multiply(m, scale);
    return res;
}

ac_mat4_t ac_mat4_multiply(ac_mat4_t a, ac_mat4_t b) {
    ac_mat4_t mat = {0};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mat.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                mat.m[i][j] += b.m[i][k] * a.m[k][j];
            }
        }
    }
    return mat;
}

ac_vec3f_t ac_mat4_transform_vec3(ac_mat4_t mat, ac_vec3f_t vec) {
    ac_vec3f_t result = {0};
    result.x = mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z +
               mat.m[3][0];
    result.y = mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z +
               mat.m[3][1];
    result.z = mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z +
               mat.m[3][2];
    return result;
}

ac_mat4_t ac_mat4_ortho(float left, float right, float bottom, float top,
                        float near, float far) {
    ac_mat4_t mat = {0};
    float rl = 1.0f / (right - left);
    float tb = 1.0f / (top - bottom);
    float fn = -1.0f / (far - near);

    mat.m[0][0] = 2.0f * rl;
    mat.m[1][1] = 2.0f * tb;
    mat.m[2][2] = 2.0f * fn;
    mat.m[3][0] = -(right + left) * rl;
    mat.m[3][1] = -(top + bottom) * tb;
    mat.m[3][2] = (far + near) * fn;
    mat.m[3][3] = 1.0f;
    return mat;
}

ac_mat4_t ac_mat4_perspective(float fov, float aspect, float near, float far) {
    ac_mat4_t mat = {0};
    float f = 1.0f / tanf(fov * 0.5f * AC_PI / 180.0f);
    float fn = 1.0f / (near - far);

    mat.m[0][0] = f / aspect;
    mat.m[1][1] = f;
    mat.m[2][2] = (near + far) * fn;
    mat.m[2][3] = -1.0f;
    mat.m[3][2] = 2.0f * near * far * fn;

    return mat;
}

ac_mat4_t ac_mat4_lookat(ac_vec3f_t eye, ac_vec3f_t target, ac_vec3f_t up) {
    ac_mat4_t mat = {0};
    ac_vec3f_t f = ac_vec3f_normalize(ac_vec3f_sub(target, eye));
    ac_vec3f_t s = ac_vec3f_normalize(ac_vec3f_cross(f, up));
    ac_vec3f_t u = ac_vec3f_cross(s, f);

    mat.m[0][0] = s.x;
    mat.m[1][0] = s.y;
    mat.m[2][0] = s.z;
    mat.m[0][1] = u.x;
    mat.m[1][1] = u.y;
    mat.m[2][1] = u.z;
    mat.m[0][2] = -f.x;
    mat.m[1][2] = -f.y;
    mat.m[2][2] = -f.z;
    mat.m[3][0] = -ac_vec3f_dot(s, eye);
    mat.m[3][1] = -ac_vec3f_dot(u, eye);
    mat.m[3][2] = ac_vec3f_dot(f, eye);
    mat.m[3][3] = 1.0f;

    return mat;
}

ac_mat3_t ac_mat3_identity() {
    ac_mat3_t mat = {0};
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    return mat;
}
ac_mat3_t ac_mat3_transpose(ac_mat3_t mat) {
    ac_mat3_t result = mat;
    mat.m[0][1] = result.m[1][0];
    mat.m[0][2] = result.m[2][0];
    mat.m[1][0] = result.m[0][1];
    mat.m[1][2] = result.m[2][1];
    mat.m[2][0] = result.m[0][2];
    mat.m[2][1] = result.m[1][2];
    return result;
}

float heim_mat3_determinant(ac_mat3_t mat) {
    float det = 0.0f;
    for (size_t i = 0; i < 3; i++) {
        det += (mat.m[0][i] * (mat.m[1][(i + 1) % 3] * mat.m[2][(i + 2) % 3] -
                               mat.m[1][(i + 2) % 3] * mat.m[2][(i + 1) % 3]));
    }
    return det;
}

ac_mat3_t ac_mat3_inverse(ac_mat3_t mat) {
    ac_mat3_t result = {0};
    float det = heim_mat3_determinant(mat);
    if (det == 0.0f) {
        return result;
    }
    float invdet = 1.0f / det;

    result.m[0][0] =
        (mat.m[1][1] * mat.m[2][2] - mat.m[1][2] * mat.m[2][1]) * invdet;
    result.m[0][1] =
        (mat.m[0][2] * mat.m[2][1] - mat.m[0][1] * mat.m[2][2]) * invdet;
    result.m[0][2] =
        (mat.m[0][1] * mat.m[1][2] - mat.m[0][2] * mat.m[1][1]) * invdet;
    result.m[1][0] =
        (mat.m[1][2] * mat.m[2][0] - mat.m[1][0] * mat.m[2][2]) * invdet;
    result.m[1][1] =
        (mat.m[0][0] * mat.m[2][2] - mat.m[0][2] * mat.m[2][0]) * invdet;
    result.m[1][2] =
        (mat.m[0][2] * mat.m[1][0] - mat.m[0][0] * mat.m[1][2]) * invdet;
    result.m[2][0] =
        (mat.m[1][0] * mat.m[2][1] - mat.m[1][1] * mat.m[2][0]) * invdet;
    result.m[2][1] =
        (mat.m[0][1] * mat.m[2][0] - mat.m[0][0] * mat.m[2][1]) * invdet;
    result.m[2][2] =
        (mat.m[0][0] * mat.m[1][1] - mat.m[0][1] * mat.m[1][0]) * invdet;

    return result;
}

ac_mat3_t ac_mat3_from_mat4(ac_mat4_t mat) {
    ac_mat3_t result = {0};
    result.m[0][0] = mat.m[0][0];
    result.m[0][1] = mat.m[0][1];
    result.m[0][2] = mat.m[0][2];
    result.m[1][0] = mat.m[1][0];
    result.m[1][1] = mat.m[1][1];
    result.m[1][2] = mat.m[1][2];
    result.m[2][0] = mat.m[2][0];
    result.m[2][1] = mat.m[2][1];
    result.m[2][2] = mat.m[2][2];
    return result;
}

ac_mat4_t ac_mat4_from_quat(ac_vec4f_t q) {
    float w, x, y, z, xx, yy, zz, xy, yz, xz, wx, wy, wz, norm, s;

    norm = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
    s = norm > 0.0f ? 2.0f / norm : 0.0f;

    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;

    xx = s * x * x;
    xy = s * x * y;
    wx = s * w * x;
    yy = s * y * y;
    yz = s * y * z;
    wy = s * w * y;
    zz = s * z * z;
    xz = s * x * z;
    wz = s * w * z;

    ac_mat4_t dest = {0};

    dest.m[0][0] = 1.0f - yy - zz;
    dest.m[1][1] = 1.0f - xx - zz;
    dest.m[2][2] = 1.0f - xx - yy;

    dest.m[0][1] = xy + wz;
    dest.m[1][2] = yz + wx;
    dest.m[2][0] = xz + wy;

    dest.m[1][0] = xy - wz;
    dest.m[2][1] = yz - wx;
    dest.m[0][2] = xz - wy;

    dest.m[0][3] = 0.0f;
    dest.m[1][3] = 0.0f;
    dest.m[2][3] = 0.0f;
    dest.m[3][0] = 0.0f;
    dest.m[3][1] = 0.0f;
    dest.m[3][2] = 0.0f;
    dest.m[3][3] = 1.0f;
    return dest;
}
