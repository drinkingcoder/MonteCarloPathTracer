#ifndef KDTREE_H
#define KDTREE_H

#include <vector>

#include "Material.h"
#include "Type.h"
#include "Ray.h"
#include "Objects.h"

struct Intersection;

class AABBox {
    Vector3 bl;     // Bottom left (min)
    Vector3 tr;     // Top right   (max)

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    AABBox (Vector3 bl_=Vector3(0, 0, 0), Vector3 tr_=Vector3(0, 0, 0)):
        bl(bl_), tr(tr_) {}
    virtual ~AABBox() {}

    // Expand to fit box
    void expand(const AABBox &box) {
        bl = bl.cwiseMin(box.bl);
        tr = tr.cwiseMax(box.tr);
    }

    // Expand to fit point
    void expand(const Vector3 &vec) {
        bl = bl.cwiseMin(vec);
    }

    // Returns longest axis: 0, 1, 2 for x, y, z respectively
    int get_longest_axis() {
        Vector3 diff = tr - bl;
        if (diff(0) > diff(1) && diff(0) > diff(2)) return 0;
        if (diff(1) > diff(0) && diff(1) > diff(2)) return 1;
        return 2;
    }

    // Check if ray intersects with box. Returns true/false and stores distance in t
    bool intersection(const Ray &r, double &t) {
        Vector3 t1 = (bl - r.origin).cwiseProduct(r.direction_inv);
        Vector3 t2 = (tr - r.origin).cwiseProduct(r.direction_inv);
        double tmin = t1.cwiseMin(t2).maxCoeff();
        double tmax = t1.cwiseMax(t2).minCoeff();
        return tmax + EPSILON >= tmin;
    }
};

class Triangle {
    Vector3 v0, v1, v2;     // Vertex world space coordinates
    Vector3 e1, e2;         // Edge 1 and edge 2 of triangle
    Vector3 n0, n1, n2;
    Material *m;        // Pointer to material

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Triangle(Vector3 v0_, Vector3 v1_, Vector3 v2_, Vector3 n0_=Vector3(0, 0, 0), Vector3 n1_=Vector3(0, 0, 0), Vector3 n2_=Vector3(0, 0, 0), Material *m_=NULL){
        v0=v0_, v1=v1_, v2=v2_, e1=v1-v0, e2=v2-v0;
        n0=n0_, n1=n1_, n2=n2_;
        m=m_;
    }
    virtual ~Triangle() {}

    // Returns axis aligned bounding box that contains the triangle
    inline AABBox get_bounding_box(){
        return AABBox(v0.cwiseMin(v1.cwiseMin(v2)), v0.cwiseMax(v1.cwiseMax(v2)));
    }

    // Returns the midpoint of the triangle
    inline Vector3 get_midpoint(){
        return (v0 + v1 + v2)/3;
    }

    // Checks if ray intersects with triangle. Returns true/false.
    inline bool intersect(Ray ray, double &t) const {

        double u, v, t_temp=0;

        Vector3 pvec = ray.direction.cross(e2);
        double det = e1.dot(pvec);
        if (det == 0) return false;
        double invDet = 1. / det;
        Vector3 tvec = ray.origin - v0;
        u = tvec.dot(pvec) * invDet;
        if (u < -EPSILON || u > 1+EPSILON) return false;
        Vector3 qvec = tvec.cross(e1);
        v = ray.direction.dot(qvec) * invDet;
        if (v < -EPSILON || u + v > 1+EPSILON) return false;
        t_temp = e2.dot(qvec) * invDet; // Set distance along ray to intersection
        if (t_temp < ray.tmax) {
            if (t_temp > EPSILON ){    // Fairly arbritarily small value, scared to change
                t = t_temp;         // it as it works.
                return true;
            }
        }
        return false;
    }

    // Returns barycentric coordinates of point p on the triangle
    inline Vector3 barycentric(Vector3 p){
        Vector3 v2_ = p - v0;
        double d00 = e1.dot(e1);
        double d01 = e1.dot(e2);
        double d11 = e2.dot(e2);
        double d20 = v2_.dot(e1);
        double d21 = v2_.dot(e2);
        double d = d00*d11 - d01*d01;
        double v = (d11*d20 - d01*d21) / d;
        double w = (d00*d21 - d01*d20) / d;
        double u = 1 - v - w;
        return Vector3(u, v, w);
    }


    inline Vector3 get_normal_at(const Vector3& p) {
        Vector3 b = barycentric(p);
        return (n0 * b(0) + n1 * b(1) + n2 * b(2)).normalized();
    }

    inline Material& get_material() {
        return *m;
    }

};

class KDNode {
public:
    AABBox box;
    KDNode* left;
    KDNode* right;
    std::vector<Triangle*> triangles;
    bool leaf;

    KDNode(){};
    KDNode* build(std::vector<Triangle*> &tris, int depth);
    bool hit (KDNode* node, Ray &ray, double &t, Intersection& intersection);
};

#endif // KDTREE_H
