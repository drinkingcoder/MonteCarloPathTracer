#ifndef RENDERER_H
#define RENDERER_H

#include "Type.h"
#include "Scene.h"
#include "Camera.h"

class Renderer {

private:
    Scene *m_scene;
    Camera *m_camera;
    Vector3 *m_pixel_buffer;
    Configuration config;
    //cv::Mat m_image;

public:
    Renderer(const Configuration& config);
    void setup_scene();
    void render();
    void save_image(const char * file_path);

};

#endif //RENDERER_H
