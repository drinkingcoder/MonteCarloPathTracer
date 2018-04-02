#ifndef RAY_H
#define RAY_H

#include <limits>
#include "Type.h"

enum RayType {
    INVALID,
    SPECULAR_REFLECT,
    DIFFUSE_REFLECT,
    TRANS_REFRACT
};

struct Ray {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Vector3 origin, direction, direction_inv;
    double tmin, tmax;
    RayType type;
    Ray(const Vector3& o_, const Vector3& d_, RayType t_ = INVALID) :
        origin(o_),
        direction(d_),
        direction_inv(d_.cwiseInverse()),
        tmin(EPSILON),
        tmax(std::numeric_limits<double>::max()),
        type(t_)
    { }

    inline double get_lower_bound() {
        return tmin;
    }

    inline double get_upper_bound() {
        return tmax;
    }

    inline bool inner_bound(double t) {
        return t <= tmax && t >= tmin;
    }

    inline Vector3 get_position(double t) {
        return origin + direction * t;
    }

    inline Vector3 reflect(const Vector3& normal)
    {
        return direction - 2.0f * normal.dot(direction) * normal;
    }

    bool refract(const Vector3& normal, double nit, Vector3& refract_direction)
    {
        double ndoti = normal.dot(direction),
              k = 1.0f - nit * nit * (1.0f - ndoti * ndoti);
        if (k >= 0.0f) {
            refract_direction.block(0, 0, 3, 1) = nit * direction - normal * (nit * ndoti + sqrt(k));
            return true;
        }
        // total internal reflection. There is no refraction in this case
        else return false;
    }
};

#endif // RAY_H
