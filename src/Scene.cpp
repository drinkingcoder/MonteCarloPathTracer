#include "Scene.h"
#include "Objects.h"
#include "Timer.h"

void Scene::add(Object *object) {
    m_objects.push_back( object );
}

Intersection Scene::intersect(Ray &ray) {
    Intersection isct = Intersection();
    Intersection tmp = Intersection();
    long size = m_objects.size();

    for (int i=0; i<size; i++){
        m_objects.at((unsigned)i)->get_intersection(ray, tmp);

        if (tmp.hit) {
            if (isct.u == 0 || tmp.u < isct.u) isct = tmp;
        }
    }
    return isct;
}

bool Scene::russian_roulette(double probability, double& survival) {
    survival = (double)std::rand() / RAND_MAX;
    if (survival > probability) return true;
    return false;
}

Vector3 Scene::importance_sample(const Vector3& up, double n)
{
    //srand(0);
    double phi, theta;
    double r1 = (double)std::rand() / RAND_MAX, r2 = (double)std::rand() / RAND_MAX;

    phi = r1 * 2 * M_PI;
    theta = n<0 ? asin(sqrt(r2)) : acos(pow(r2, 1 / (n + 1)));
    Vector3 sample(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));

    Vector3 front, right;//

    if (fabs(up.x()) > fabs(up.y()))
        front = Vector3(up.z(), 0, -up.x());
    else
        front = Vector3(0, -up.z(), up.y());

    front = front.normalized();
    right = up.cross(front);

    return (sample.x()*right + sample.y()*up + sample.z()*front).normalized();
}

Ray Scene::montecarlo_sample(Ray& ray, Intersection& intersection) {
    auto& m = intersection.m;
    Vector3 direction;
    double num[2];
    num[0] = m.m_diffuse.dot(Vector3(1, 1, 1));
    num[1] = m.m_specular.dot(Vector3(1, 1, 1)) + num[0];

    if (num[1] <= 0) {
        return Ray(intersection.point, direction);
    }

    if (m.m_type == TRANS) {
        //printf("mior = %lf\n",m.m_ior);
        double ni, nt;
        double costheta = ray.direction.dot( intersection.n );

        Vector3 n = costheta <= 0 ? intersection.n : -intersection.n;

        if (costheta > 0) {
            ni = m.m_ior;
            nt = 1.0;
        } else {
            ni = 1.0;
            nt = m.m_ior;
        }

        double f = (ni - nt) / (ni + nt);
        f = f*f;
        f = f + (1 - f) * pow(1 - fabs(costheta), 5);
        //printf("ni = %lf, nt = %lf, cos = %lf, f = %lf\n", ni, nt, costheta, f);
        double transmission_survival;
        if (russian_roulette(f, transmission_survival)) {
            if (ray.refract(n, ni / nt, direction)) {
                //printf("transparent.\n");
                return Ray(intersection.point, direction, TRANS_REFRACT);
            } else {
                direction = ray.reflect(n);
                return Ray(intersection.point, direction, SPECULAR_REFLECT);
            }
        }
    }

    double specularSurvival;
    if (russian_roulette(num[0] / num[1], specularSurvival))
    {
        Vector3 reflect_direction = ray.reflect(intersection.n);
        direction = importance_sample(reflect_direction, m.m_shiness);
        return Ray(intersection.point, direction, SPECULAR_REFLECT);
    }
    else
    {
        direction = importance_sample(intersection.n);
        return Ray(intersection.point, direction, DIFFUSE_REFLECT);
    }

}

Vector3 Scene::trace_ray(Ray &ray, int depth) {
    Intersection intersection = intersect(ray);
    if (!intersection.hit) {
        return Vector3(0, 0, 0);
    }

    auto& m = intersection.m;
    if (depth > 5) {
        return m.get_emission() + m.m_ambient.cwiseProduct(ambient_light);
    }

    Vector3 color = Vector3(0, 0, 0);
    Ray newRay = montecarlo_sample(ray, intersection);

    if (newRay.type !=  INVALID) {
        color = trace_ray(newRay, depth+1);

        Vector3 reflect_direction = (newRay.direction - intersection.n * 2 * newRay.direction.dot(intersection.n)).normalized();

        switch (newRay.type) {
            case DIFFUSE_REFLECT:
                color = m.m_diffuse.cwiseProduct(color);
                break;
            case SPECULAR_REFLECT:
                color = m.m_specular.cwiseProduct(color);
            case TRANS_REFRACT:
            default:
                break;

        }
    }

    return m.m_emission + color + m.m_ambient.cwiseProduct(ambient_light);
}
