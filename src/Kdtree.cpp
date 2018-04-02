#include <vector>

#include "Timer.h"
#include "Kdtree.h"
#include "Type.h"
#include "Ray.h"

// Build KD tree for tris
KDNode* KDNode::build(std::vector<Triangle*> &tris, int depth){
    KDNode* node = new KDNode();
    node->leaf = false;
    node->triangles = std::vector<Triangle*>();
    node->left = NULL;
    node->right = NULL;
    node->box = AABBox();

    if (tris.size() == 0) return node;

    if (depth > 25 || tris.size() <= 6) {
        node->triangles = tris;
        node->leaf = true;
        node->box = tris[0]->get_bounding_box();

        for (long i=1; i<tris.size(); i++) {
            node->box.expand(tris[i]->get_bounding_box());
        }

        node->left = new KDNode();
        node->right = new KDNode();
        node->left->triangles = std::vector<Triangle*>();
        node->right->triangles = std::vector<Triangle*>();

        return node;
    }

    node->box = tris[0]->get_bounding_box();
    Vector3 midpt = Vector3(0, 0, 0);
    double tris_recp = 1.0/tris.size();

    for (long i=1; i<tris.size(); i++) {
        node->box.expand(tris[i]->get_bounding_box());
        midpt = midpt + (tris[i]->get_midpoint() * tris_recp);
    }

    std::vector<Triangle*> left_tris;
    std::vector<Triangle*> right_tris;
    int axis = node->box.get_longest_axis();

    for (long i=0; i<tris.size(); i++) {
        midpt(axis) >= tris[i]->get_midpoint()(axis) ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
    }

    if (tris.size() == left_tris.size() || tris.size() == right_tris.size()) {
        node->triangles = tris;
        node->leaf = true;
        node->box = tris[0]->get_bounding_box();

        for (long i=1; i<tris.size(); i++) {
            node->box.expand(tris[i]->get_bounding_box());
        }

        node->left = new KDNode();
        node->right = new KDNode();
        node->left->triangles = std::vector<Triangle*>();
        node->right->triangles = std::vector<Triangle*>();

        return node;
    }

    node->left = build(left_tris, depth+1);
    node->right = build(right_tris, depth+1);

    return node;
}

// Finds nearest triangle in kd tree that intersects with ray.
bool KDNode::hit(KDNode *node, Ray &ray, double &t, Intersection& intersection) {
    double dist;
    if (node->box.intersection(ray, dist)){
        if (dist + EPSILON > ray.tmax) {
            return false;
        }

        bool hit_tri = false;
        bool hit_left = false;
        bool hit_right = false;
        long tri_idx;

        if (!node->leaf) {
            hit_left = hit(node->left, ray, t, intersection);
            hit_right = hit(node->right, ray, t, intersection);

            return hit_left || hit_right;
        }
        else {
            long triangles_size = node->triangles.size();
            for (long i=0; i<triangles_size; i++) {
#ifdef TIMER_COUNT
                //global_timer::timers[TRIANGLE_INTERSECTION_TIMER].start();
#endif
                if (node->triangles[i]->intersect(ray, t)){
                    hit_tri = true;
                    ray.tmax = t;
                    tri_idx = i;
                }
#ifdef TIMER_COUNT
                //global_timer::timers[TRIANGLE_INTERSECTION_TIMER].stop();
#endif
            }
            if (hit_tri) {
                Vector3 p = ray.origin + ray.direction * ray.tmax;
                intersection.point = p;
                intersection.n = node->triangles[tri_idx]->get_normal_at(p);
                intersection.u = ray.tmax;
                intersection.hit = true;
                intersection.m = node->triangles[tri_idx]->get_material();
                return true;
            }
        }
    }
    return false;
}
