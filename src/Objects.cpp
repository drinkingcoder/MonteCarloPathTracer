#include <math.h>
#include <iostream>
#include <stdio.h>
#include <map>

#include "Type.h"
#include "Ray.h"
#include "Material.h"
#include "Objects.h"
//#include "../lib/fastbvh/BVH.h"

// Check if ray intersects with sphere. Returns Intersection data structure
void Sphere::get_intersection(Ray &ray, Intersection& intersection) {
	// Solve t^2*d.d + 2*t*(o-p).d + (o-p).(o-p)-R^2 = 0
	bool hit = false;
	double distance = 0;
	Vector3 n = Vector3(0, 0, 0);

	Vector3 op = m_p-ray.origin;
	double t, eps=1e-4, b=op.dot(ray.direction), det=b*b-op.dot(op)+m_r*m_r;
    if (det<0) {
        intersection = Intersection(hit, distance, n, m_m);
        return;
    } else {
        det=sqrt(det);
    }

	distance = (t=b-det)>eps ? t : ((t=b+det)>eps ? t : 0);
    if (distance != 0){
        hit = true;
		n = ((ray.origin + ray.direction * distance) - m_p).normalized();
    }

	intersection = Intersection(hit, distance, n, m_m);
    return;
}


Mesh::Mesh(const Vector3& p_, const char* file_path, const Material& m_):
    Object(p_), m_m(m_) {

    std::string mtlbasepath;
    std::string inputfile = file_path;
    unsigned long pos = inputfile.find_last_of("/");
    mtlbasepath = inputfile.substr(0, pos+1);

    printf("Loading %s...\n", file_path);
    // Attempt to load mesh
	std::string err = tinyobj::LoadObj(m_shapes, m_materials, inputfile.c_str(), mtlbasepath.c_str());

	if (!err.empty()) {
		std::cerr << err << std::endl;
		exit(1);
	}
	printf(" - Generating k-d tree...\n\n");

    long shapes_size, indices_size, materials_size;
    shapes_size = m_shapes.size();
    materials_size = m_materials.size();
    std::cout << "triangles size = " << shapes_size << std::endl;
    std::cout << "material size = " << materials_size << std::endl;

    std::map<std::string, int> material_map;
    for (int i=0; i<materials_size; i++) {
        auto& material = m_materials[i];
        material_map[material.name] = i;
        const auto ambient = Vector3(material.ambient[0], material.ambient[1], material.ambient[2]);
        const auto diffuse = Vector3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
        const auto specular = Vector3(material.specular[0], material.specular[1], material.specular[2]);
        const auto emission = Vector3(material.emission[0], material.emission[1], material.emission[2]);
        const auto transmittance = Vector3(material.transmittance[0], material.transmittance[1], material.transmittance[2]);
        const auto ior = material.ior;
        const auto shininess = material.shininess;

        materials.push_back( Material(DIFF, ambient, diffuse, specular, emission, transmittance, ior, shininess) );

    }

    // Load triangles from obj
    for (int i = 0; i < shapes_size; i++) {
        indices_size = m_shapes[i].mesh.indices.size() / 3;
        for (size_t f = 0; f < indices_size; f++) {

            // Triangle vertex coordinates
            Vector3 v0_ = Vector3(
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f] * 3     ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f] * 3 + 1 ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f] * 3 + 2 ]
            ) + m_p;

            Vector3 v1_ = Vector3(
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 1] * 3     ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 1] * 3 + 1 ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 1] * 3 + 2 ]
            ) + m_p;

            Vector3 v2_ = Vector3(
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 2] * 3     ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 2] * 3 + 1 ],
                    m_shapes[i].mesh.positions[ m_shapes[i].mesh.indices[3*f + 2] * 3 + 2 ]
            ) + m_p;

            Vector3 n0_, n1_, n2_;

            if (m_shapes[i].mesh.indices[3 * f + 2] * 2 + 1 < m_shapes[i].mesh.normals.size()) {
                n0_ = Vector3(
                        m_shapes[i].mesh.normals[m_shapes[i].mesh.indices[3 * f] * 3],
                        m_shapes[i].mesh.normals[m_shapes[i].mesh.indices[3 * f] * 3 + 1],
                        m_shapes[i].mesh.normals[m_shapes[i].mesh.indices[3 * f] * 3 + 2]
                );

                n1_ = Vector3(
                        m_shapes[i].mesh.normals[m_shapes[i].mesh.indices[3 * f + 1] * 3],
                        m_shapes[i].mesh.normals[m_shapes[i].mesh.indices[3 * f + 1] * 3 + 1],
                        m_shapes[i].mesh.normals[m_shapes[i].mesh.indices[3 * f + 1] * 3 + 2]
                );

                n2_ = Vector3(
                        m_shapes[i].mesh.normals[m_shapes[i].mesh.indices[3 * f + 2] * 3],
                        m_shapes[i].mesh.normals[m_shapes[i].mesh.indices[3 * f + 2] * 3 + 1],
                        m_shapes[i].mesh.normals[m_shapes[i].mesh.indices[3 * f + 2] * 3 + 2]
                );
            }
            else {
                printf("Error: normal disappeared.!\n");
                n0_=Vector3(0, 0, 0);
                n1_=Vector3(0, 0, 0);
                n2_=Vector3(0, 0, 0);
            }

            if (m_shapes[i].mesh.material_ids[ f  ] < materials.size())
                tris.push_back(new Triangle(v0_, v1_, v2_, n0_, n1_, n2_, &materials[m_shapes[i].mesh.material_ids[f]]));
            else
                tris.push_back(new Triangle(v0_, v1_, v2_, n0_, n1_, n2_, &m_m));

        }
    }

    // Clean up
    m_shapes.clear();
    m_materials.clear();
    node = KDNode().build(tris, 0);
    printf("\n");
	//bvh = BVH(&tris);
}

// Check if ray intersects with mesh. Returns Intersection data structure
void Mesh::get_intersection(Ray &ray, Intersection& intersection) {
    double t=0;
#ifdef TIMER_COUNT
    global_timer::timers[KDTREE_INTERSECTION_TIMER].start();
#endif
    bool hit = node->hit(node, ray, t, intersection);
#ifdef TIMER_COUNT
    global_timer::timers[KDTREE_INTERSECTION_TIMER].stop();
#endif
    return;

}
