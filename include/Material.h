#ifndef MATERIAL_H
#define MATERIAL_H

#include "Type.h"
#include "Ray.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum MaterialType { DIFF, TRANS, EMIT };


class Material {

private:


public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	MaterialType m_type;
    double m_shiness;
    double m_ior;
	Vector3 m_emission;
    Vector3 m_ambient;
    Vector3 m_diffuse;
    Vector3 m_specular;
    Vector3 m_transmittance;

    Material( MaterialType t,
            const Vector3& ambient,
            const Vector3& diffuse,
            const Vector3& specular,
            const Vector3& emission,
            const Vector3& transmittance,
            const double ior,
            const double shininess):m_type(t),
                                    m_ambient(ambient),
                                    m_diffuse(diffuse),
                                    m_specular(specular),
                                    m_emission(emission),
                                    m_transmittance(transmittance),
                                    m_ior(ior),
                                    m_shiness(shininess) {
                                        if (ior != 1) {
                                            m_type = TRANS;
                                            //printf("TRANS Material\n");
                                        }
                                    }

    Material( MaterialType t,
            const Vector3& ambient,
            const Vector3& diffuse,
            const Vector3& specular,
            const Vector3& emission,
            const double shininess):m_type(t),
                                    m_ambient(ambient),
                                    m_diffuse(diffuse),
                                    m_specular(specular),
                                    m_emission(emission),
                                    m_transmittance(Vector3(0, 0, 0)),
                                    m_ior(1.0),
                                    m_shiness(shininess) {}

    Material( MaterialType t,
            const Vector3& ambient,
            const Vector3& diffuse,
            const Vector3& specular,
            const Vector3& emission):m_type(t),
                                    m_ambient(ambient),
                                    m_diffuse(diffuse),
                                    m_specular(specular),
                                    m_emission(emission),
                                    m_transmittance(Vector3(0, 0, 0)),
                                    m_ior(1.0),
                                    m_shiness(60) {}

    Material(MaterialType t):
        m_type(t),
        m_ambient(Vector3(0, 0, 0)),
        m_diffuse(Vector3(1, 1, 1)),
        m_specular(Vector3(0, 0, 0)),
        m_emission(Vector3(0, 0, 0)),
        m_transmittance(Vector3(0, 0, 0)),
        m_ior(1.0),
        m_shiness(60) { }

    Material(const Material& m):
        m_type(m.m_type),
        m_ambient(m.m_ambient),
        m_diffuse(m.m_diffuse),
        m_specular(m.m_specular),
        m_emission(m.m_emission),
        m_transmittance(m.m_transmittance),
        m_ior(m.m_ior),
        m_shiness(m.m_shiness){}


    Material& operator=(const Material& m) {
        m_type = m.m_type;
        m_ambient = m.m_ambient;
        m_diffuse = m.m_diffuse;
        m_specular = m.m_specular;
        m_emission = m.m_emission;
        m_transmittance = m.m_transmittance;
        m_ior = m.m_ior;
        m_shiness = m.m_shiness;
    }


    inline static Material EmissionMaterial() {
        return Material(EMIT, Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(1,1,1));
    }

    inline static Material DefaultMaterial() {
        return Material(DIFF, Vector3(0, 0, 0.1), Vector3(0, 0, 0.9), Vector3(0, 0, 0), Vector3(1, 1, 1));
    }

    inline MaterialType get_type() const {
        return m_type;
    }

    inline Vector3 get_emission() const {
        return m_emission;
    }
};


#endif // MATERIAL_H
