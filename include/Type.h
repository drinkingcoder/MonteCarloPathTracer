#ifndef TYPE_H
#define TYPE_H

#include <chrono>
#include <limits>
#include <string>
#include "Eigen/Eigen"
#include "Eigen/Core"
#include "Eigen/Geometry"
#include "Eigen/Dense"

// Eigen Matrices
typedef double real_type;
typedef Eigen::Matrix<real_type, 3, 1> Vector3;
typedef Eigen::Matrix<real_type, 2, 1> Vector2;

typedef Eigen::Matrix<real_type, 4, 4> Matrix4;
typedef Eigen::Matrix<real_type, 3, 3> Matrix3;

inline Vector3 transform(const Matrix4 & M, const Vector3 & v) {
    return M.block(0, 0, 3, 3) * v + M.block(0, 3, 3, 1);
}

inline Vector3 rotate(const Matrix4 & M, const Vector3 & v) {
    return M.block(0, 0, 3, 3) * v;
}

inline Vector3 get_position(const Matrix4 & M) {
    return -M.block(0, 0, 3, 3).transpose()*M.block(0, 3, 3, 1);
}

//#define TIMER_COUNT
//#undef _OPEN_MP_
//
// rendering parameters
#define T_MIN 0.1
#define T_MAX 5.0
#define EPSILON 1e-4f

struct Configuration {
    std::string obj;
    std::string output_name;
    int width;
    int height;
    int samples;
    int batch_size;
    Vector3 camera_position;
    Vector3 look_at_point;
    Vector3 ambient_light;
    bool light;
};

#endif
