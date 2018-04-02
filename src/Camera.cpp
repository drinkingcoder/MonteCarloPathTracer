#include "Ray.h"
#include "Camera.h"

Camera::Camera(Vector3 position, Vector3 target, int width, int height) {
    m_width = width;
    m_width_recp = 1./m_width;
    m_height = height;
    m_height_recp = 1./m_height;
    m_ratio = (double)m_width/m_height;

    m_position = position;
    m_direction = (target - m_position).normalized();
    m_x_direction = Vector3::UnitZ().cross(m_direction * -1).normalized();
    m_y_direction = m_x_direction.cross(m_direction).normalized();

    m_x_spacing = (2.0 * m_ratio)/(double)m_width;
    m_y_spacing = (double)2.0/(double)m_height;
    m_x_spacing_half = m_x_spacing * 0.5;
    m_y_spacing_half = m_y_spacing * 0.5;
}

int Camera::get_width() { return m_width; }
int Camera::get_height() { return m_height; }

// Returns ray from camera origin through pixel at x,y
Ray Camera::get_ray(int x, int y) {
    Vector3 pixel = m_position + m_direction*2;
    pixel = pixel - m_x_direction*m_ratio + m_x_direction*((x * 2 * m_ratio)*m_width_recp);
    pixel = pixel + m_y_direction - m_y_direction*((y * 2.0)*m_height_recp);

    return Ray(m_position, (pixel-m_position).normalized());
}
