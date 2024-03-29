#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"
#include "texture.h"
#include "perlin.h"
#include "ONB.h"
#include "pdf.h"

struct hit_record;

// inline vec3 random_cosine_direction() {
//     auto r1 = random_double();
//     auto r2 = random_double();
//     auto z = sqrt(1-r2);
//     auto phi = 2*pi*r1;
//     auto x = cos(phi)*sqrt(r2);
//     auto y = sin(phi)*sqrt(r2);

//     return vec3(x, y, z);
// }

class material {
    public:

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& albedo, ray& scattered, double& pdf
        ) const {
            return false;
        }

        virtual double scattering_pdf(
            const ray& r_in, const hit_record& rec, const ray& scattered
        ) const {
            return 0;
        }

        virtual color emitted(
            const ray& r_in, const hit_record& rec, double u, double v, const point3& p
        ) const {
            return color(0,0,0);
        }
};

class lambertian : public material {
    public:
        lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
        lambertian(shared_ptr<texture> a) : albedo(a) {}

        virtual bool scatter(
    const ray& r_in, const hit_record& rec, color& alb, ray& scattered, double& pdf
        ) const override {
            onb uvw;
            uvw.build_from_w(rec.normal);
            auto direction = uvw.local(random_cosine_direction());
            scattered = ray(rec.p, unit_vector(direction), r_in.time());
            alb = albedo->value(rec.u, rec.v, rec.p);
            pdf = dot(uvw.w(), scattered.direction()) / pi;
            return true;
        }
        double scattering_pdf(
            const ray& r_in, const hit_record& rec, const ray& scattered
        ) const {
            auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
            return cosine < 0 ? 0 : cosine/pi;
        }

    public:
        shared_ptr<texture> albedo;
};

// class nayer : public material {
//     public:
//         nayer(const color& a) : albedo(make_shared<solid_color>(a)) {}
//         nayer(shared_ptr<texture> a) : albedo(a) {}

//         virtual bool scatter(
//             const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
//         ) const override {
//             auto scatter_direction = rec.normal + random_unit_vector();

//             // Catch degenerate scatter direction
//             if (scatter_direction.near_zero())
//                 scatter_direction = rec.normal;

//             scattered = ray(rec.p, scatter_direction, r_in.time());

//             auto rho = 0.25;
//             auto sigma = 0.3;
//             auto A = 1 / (pi + (pi/2 - 2/3)*sigma);
//             auto B = sigma / (pi + (pi/2 - 2/3)*sigma);

//             auto s = dot(r_in.direction(), scattered.direction()) - dot(rec.normal, r_in.direction())*dot(rec.normal, r_in.direction());
//             auto max =  dot(rec.normal, r_in.direction()) < dot(rec.normal, r_in.direction()) ? dot(rec.normal, r_in.direction()) : dot(rec.normal, r_in.direction());
//             auto t = s <= 0 ? 1 : max;

//             auto ON = rho * dot(rec.normal, r_in.direction())*(A+B*(s/t));

//             attenuation = 0.05 * ON * albedo->value(rec.u, rec.v, rec.p);
//             return true;
//         }

//     public:
//         shared_ptr<texture> albedo;
// };


// class metal : public material {
//     public:
//         metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

//         virtual bool scatter(
//             const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
//         ) const override {
//             vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
//             scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
//             attenuation = albedo;
//             return (dot(scattered.direction(), rec.normal) > 0);
//         }

//     public:
//         color albedo;
//         double fuzz;
// };

// class dielectric : public material {
//     public:
//         dielectric(double index_of_refraction, shared_ptr<texture> a) : ir(index_of_refraction), albedo(a) {}
//         dielectric(double index_of_refraction, const color & a) : ir(index_of_refraction), albedo(make_shared<solid_color>(a)) {}
//         dielectric(double index_of_refraction) : ir(index_of_refraction) {}

//         virtual bool scatter(
//             const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
//         ) const override {
//             attenuation = albedo->value(rec.u, rec.v, rec.p);
//             double refraction_ratio = rec.front_face ? (1.0/ir) : ir;

//             vec3 unit_direction = unit_vector(r_in.direction());
//             double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
//             double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

//             bool cannot_refract = refraction_ratio * sin_theta > 1.0;
//             vec3 direction;
//             if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
//                 direction = reflect(unit_direction, rec.normal);
//             else
//                 direction = refract(unit_direction, rec.normal, refraction_ratio);

//             scattered = ray(rec.p, direction, r_in.time());
//             return true;
//         }

//     public:
//         double ir; // Index of Refraction
//         shared_ptr<texture> albedo;

//     private:
//         static double reflectance(double cosine, double ref_idx) {
//             // Use Schlick's approximation for reflectance.
//             auto r0 = (1-ref_idx) / (1+ref_idx);
//             r0 = r0*r0;
//             return r0 + (1-r0)*pow((1 - cosine),5);
//         }
// };

class diffuse_light : public material  {
    public:
        diffuse_light(shared_ptr<texture> a) : emit(a) {}
        diffuse_light(color c) : emit(make_shared<solid_color>(c)) {}
        virtual bool scatter(
            const ray& r_in, const hit_record& rec, color& albedo, ray& scattered, double& pdf
        ) const override {
            return false;
        }
        virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v,
            const point3& p) const override {

            if (rec.front_face)
                return emit->value(u, v, p);
            else
                return color(0,0,0);
        }
    public:
        shared_ptr<texture> emit;
};

// class isotropic : public material {
//     public:
//         isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
//         isotropic(shared_ptr<texture> a) : albedo(a) {}

//         virtual bool scatter(
//             const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
//         ) const override {
//             scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
//             attenuation = albedo->value(rec.u, rec.v, rec.p);
//             return true;
//         }

//     public:
//         shared_ptr<texture> albedo;
// };

// class cloud : public material {
//     public:
//         cloud(color c) : albedo(make_shared<solid_color>(c)) {}
//         cloud(shared_ptr<texture> a) : albedo(a) {}

//         virtual bool scatter(
//             const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
//         ) const override {
//             perlin noise;
//             scattered = ray(rec.p, random_in_unit_sphere(), r_in.time());
//             color c_val = albedo->value(rec.u, rec.v, rec.p);
//             double r = c_val.x() * noise.turb(rec.p);
//             double g = c_val.y() * noise.turb(rec.p);
//             double b = c_val.z() * noise.turb(rec.p);

//             attenuation = color(r, g, b);
//             return true;
//         }

//     public:
//         shared_ptr<texture> albedo;
// };

#endif