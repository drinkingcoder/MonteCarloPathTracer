#include <vector>
#include <stdio.h>
#include <iostream>
#include <chrono>

#include "Renderer.h"
#include "Timer.h"
#include "../lib/lodepng/lodepng.h"

using namespace std;

//using namespace cv;

// Clamp double to min/max of 0/1
inline double clamp(double x){ return x<0 ? 0 : x>1 ? 1 : x; }
// Clamp to between 0-255
inline int toInt(double x){ return int(clamp(x)*255+.5); }

Renderer::Renderer(const Configuration& config):
    config(config) {
    m_scene = new Scene(config);
    m_camera = new Camera(config.camera_position, config.look_at_point, config.width, config.height);     // Create camera

    m_pixel_buffer = new Vector3[config.width * config.height];
    for (auto i = 0; i < config.width * config.height; i++) {
        m_pixel_buffer[i] = Vector3(0, 0, 0);
    }
}

void Renderer::setup_scene() {
    timer t = timer();
    t.start();
    cout << "load obj.." << endl;

    if (config.light == true) {
        m_scene->add( dynamic_cast<Object*>(new Sphere(Vector3(0, 110, 0), 50, Material::EmissionMaterial())) );
    }
    m_scene->add( dynamic_cast<Object*>(new Mesh(Vector3(0, 0, 0), config.obj.c_str(), Material::DefaultMaterial())) );

    t.stop();
    cout << "load object consumes: " << t.get_seconds() << " secs." << endl;
}

void Renderer::render() {
#ifdef _OPEN_MP_
    printf("Accelerate with openmp.\n");
#else
    printf("No openmp found.\n");
#endif
    const auto width = config.height;
    const auto height = config.width;

    const auto samples = config.samples;
    const auto batch_size = config.batch_size;
    const auto batch_recp = 1./batch_size;

    timer render_timer = timer();
    timer batch_timer = timer();
    char fn[100];

    setup_scene();
    // Main Loop
    int itr_times = config.samples/config.batch_size;
    for (int itr = 0; itr < itr_times; itr++) {
        render_timer.start();
        batch_timer.reset();
        batch_timer.start();
        fprintf(stderr, "\rRendering (%i samples): %.2f%% ",      // Prints
                config.samples, (double)itr/itr_times*100);                   // progress

#ifdef _OPEN_MP_
#pragma omp parallel for schedule(dynamic, 1)       // OpenMP
#endif
        for (int y=0; y<height; y++){
            for (int x=0; x<width; x++){
                Vector3 col = Vector3(0, 0, 0);

                for (int a=0; a<batch_size; a++){
                    Ray ray = m_camera->get_ray(x, y);
                    col = col + m_scene->trace_ray(ray, 0);
                }
                auto& pixel = m_pixel_buffer[(y)*width + x];
                pixel = (pixel * itr + col * batch_recp)/(itr + 1);
            }
        }
        sprintf(fn, "%s_%d.png", config.output_name.c_str(), itr * batch_size);
        batch_timer.stop();
        printf("%lf secs per batch.\n", batch_timer.get_seconds());
        render_timer.stop();
#ifdef TIMER_COUNT
        printf("kdtree intersection per patch: %lf secs.\n", global_timer::timers[KDTREE_INTERSECTION_TIMER].get_seconds());
        //printf("triangle intersection per patch: %lf secs.\n", global_timer::timers[TRIANGLE_INTERSECTION_TIMER].get_seconds());
        global_timer::timers[KDTREE_INTERSECTION_TIMER].reset();
        global_timer::timers[TRIANGLE_INTERSECTION_TIMER].reset();
#endif
        save_image(fn);
    }
    printf("\rRendering (%i samples): Complete!\nTime Taken: %lf secs\n\n", config.samples, render_timer.get_seconds());
}

void Renderer::save_image(const char *file_path) {
    int width = m_camera->get_width();
    int height = m_camera->get_height();

    std::vector<unsigned char> pixel_buffer;

    int pixel_count = width*height;

    for (int i=0; i<pixel_count; i++) {
        pixel_buffer.push_back(toInt(m_pixel_buffer[i](0)));
        pixel_buffer.push_back(toInt(m_pixel_buffer[i](1)));
        pixel_buffer.push_back(toInt(m_pixel_buffer[i](2)));
        pixel_buffer.push_back(255);
    }

    //Encode the image
    unsigned error = lodepng::encode(file_path, pixel_buffer, width, height);
    //if there's an error, display it
    if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;

    pixel_buffer.clear();
}
