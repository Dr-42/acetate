#ifndef AC_MATH_MAT_H
#define AC_MATH_MAT_H

/**
 * @file ac_math_mat.h
 * @brief Matrix math functions.
 *
 * This file contains matrix math functions.
 */

#include "math/ac_math_vec.h"

/** A 4x4 matrix.*/
typedef struct ac_mat4_t {
    /** Matrix data. */
    float m[4][4];
} __attribute__((aligned(16))) ac_mat4_t;

/** A 3x3 matrix. */
typedef struct ac_mat3_t {
    /** Matrix data. */
    float m[3][3];
} __attribute__((aligned(16))) ac_mat3_t;

/**
 * Print a matrix.
 * @param mat Matrix to print.
 */
void ac_mat4_print(ac_mat4_t mat);

/**
 * Get an 4x4 identity matrix.
 */
ac_mat4_t ac_mat4_identity();

/**
 * Translates a matrix.
 * @param m Matrix to translate.
 * @param vec Translation vector.
 */
ac_mat4_t ac_mat4_translate(ac_mat4_t m, ac_vec3f_t vec);

/**
 * Rotates a matrix.
 * @param m Matrix to rotate.
 * @param degrees Degrees to rotate.
 * @param axis Axis to rotate around.
 */
ac_mat4_t ac_mat4_rotate(ac_mat4_t m, float degrees, ac_vec3f_t axis);

/**
 * Scales a matrix.
 * @param m Matrix to scale.
 * @param vec Scale vector.
 */
ac_mat4_t ac_mat4_scale(ac_mat4_t m, ac_vec3f_t vec);

/**
 * Multiplies two matrices.
 * @param a First matrix.
 * @param b Second matrix.
 * @return Resulting matrix.
 */
ac_mat4_t ac_mat4_multiply(ac_mat4_t a, ac_mat4_t b);

/**
 * Transforms a vector by a matrix.
 * @param mat Matrix to transform by.
 * @param vec Vector to transform.
 * @return Transformed vector.
 */
ac_vec3f_t ac_mat4_transform_vec3(ac_mat4_t mat, ac_vec3f_t vec);

/**
 * Get an orthographic projection matrix.
 * @param left Left plane.
 * @param right Right plane.
 * @param bottom Bottom plane.
 * @param top Top plane.
 * @param near Near plane.
 * @param far Far plane.
 * @return Orthographic projection matrix.
 */
ac_mat4_t ac_mat4_ortho(float left, float right, float bottom, float top,
                        float near, float far);

/**
 * Get a perspective projection matrix.
 * @param fov Field of view.
 * @param aspect Aspect ratio.
 * @param near Near plane.
 * @param far Far plane.
 * @return Perspective projection matrix.
 */
ac_mat4_t ac_mat4_perspective(float fov, float aspect, float near, float far);

/**
 * Get a lookat matrix.
 * @param eye Eye position.
 * @param target Target position.
 * @param up Up vector.
 * @return Lookat matrix.
 */
ac_mat4_t ac_mat4_lookat(ac_vec3f_t eye, ac_vec3f_t target, ac_vec3f_t up);

/**
 * Get a 3x3 identity matrix.
 * @return Identity matrix.
 */
ac_mat3_t ac_mat3_identity();

/**
 * Transposes a matrix.
 * @param mat Matrix to transpose.
 * @return Transposed matrix.
 */
ac_mat3_t ac_mat3_transpose(ac_mat3_t mat);

/**
 * Inverts a matrix.
 * @param mat Matrix to invert.
 * @return Inverted matrix.
 */
ac_mat3_t ac_mat3_inverse(ac_mat3_t mat);

/** Creates a 3x3 matrix from a 4x4 matrix.
 * @param mat Matrix to convert.
 * @return Converted matrix.
 */
ac_mat3_t ac_mat3_from_mat4(ac_mat4_t mat);

/**
 * Creates a 4x4 matrix from a quaternion.
 * @param quat Quaternion to convert.
 * @return Converted matrix.
 */
ac_mat4_t ac_mat4_from_quat(ac_vec4f_t quat);

#endif  // AC_MATH_MAT_H
