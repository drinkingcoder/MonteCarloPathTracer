#ifndef CAMERA_H
#define CAMERA_H

#include "Type.h"
#include "Ray.h"

class Camera {

private:
    int m_width;
    double m_width_recp;
    int m_height;
    double m_height_recp;
    double m_ratio;
    double m_x_spacing;
    double m_x_spacing_half;
    double m_y_spacing;
    double m_y_spacing_half;
    Vector3 m_position;
    Vector3 m_direction;
    Vector3 m_x_direction;
    Vector3 m_y_direction;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Camera(Vector3 position, Vector3 target, int width, int height);
    int get_width();
    int get_height();
    Ray get_ray(int x, int y);

};

#endif //CAMERA_H
