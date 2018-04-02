#ifndef SCENE_H
#define SCENE_H

#include "Ray.h"
#include "Type.h"
#include "Objects.h"

class Scene {
private:
    std::vector<Object*> m_objects;
    Vector3 ambient_light;
    Configuration config;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Scene(const Configuration &config_):
        config(config_),
        ambient_light(config_.ambient_light)
    {}

    void add(Object *object);
    Intersection intersect(Ray &ray);
    Vector3 trace_ray(Ray &ray, int depth);
    Vector3 direct_illum(Ray &ray, Intersection& intersection);
    Ray montecarlo_sample(Ray& ray, Intersection& intersection);
    bool russian_roulette(double probability, double& survival);
    Vector3 importance_sample(const Vector3& up, double n = -1);
};

#endif //SCENE_H
