#ifndef OBJECTS_H
#define OBJECTS_H

#include "Timer.h"
#include "Type.h"
#include "Ray.h"
#include "Kdtree.h"
#include "Material.h"
#include "../lib/tiny_obj_loader/tiny_obj_loader.h"
//#include "../lib/fastbvh/BVH.h"

class KDNode;
class Triangle;

struct Intersection {
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	bool hit;	// If there was an intersection
	double u;	// Distance to intersection along ray
	Material m;	// Material of intersected face
	Vector3 n;		// Normal of intersected face
    Vector3 point;

	Intersection(bool hit_, double u_, Vector3 n_, Material m_):
        hit(hit_), u(u_), n(n_), m(m_) {
        }

    Intersection():
        hit(false), u(0), n(Vector3(0, 0, 0)), m(Material::DefaultMaterial()) {
        }

    Intersection(const Intersection& rhs):
        hit(rhs.hit),
        u(rhs.u),
        n(rhs.n),
        m(rhs.m) { }
};

class Object {

public:
    Object() {}
    Object(const Vector3& p):m_p(p) {}

	Vector3 m_p; // Position
	virtual void get_intersection(Ray &r, Intersection& intersection) = 0;
};


class Sphere : public Object {

private:
	double m_r;	// Radius
	Material m_m;	// Material

public:
    Sphere(const Vector3& p_, double r_, const Material& m_ ):
        Object(p_), m_r(r_), m_m(m_) { }

    inline virtual double get_radius() const {
        return m_r;
    }
    inline virtual const Material& get_material() {
        return m_m;
    }

	virtual void get_intersection(Ray &r, Intersection& intersection);
};


class Mesh : public Object {

private:
	std::vector<tinyobj::shape_t> m_shapes;
    std::vector<tinyobj::material_t> m_materials;
    std::vector<Material> materials;
    std::vector<Triangle*> tris;
    Material m_m;	// Material
    KDNode *node;
	//BVH bvh;

public:
	Mesh(const Vector3& p_, const char* file_path, const Material& m_);
	virtual void get_intersection(Ray &r, Intersection& intersection);

};


#endif // OBJECTS_H
