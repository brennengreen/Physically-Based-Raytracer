#include <iostream>
#include <math.h>
#include <chrono>
#include "bvh.h"
#include "rtweekend.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "aarect.h"
#include "box.h"
#include "pdf.h"
//#include "constant_medium.h"
//#include "turbulent_medium.h"
#include "moving_sphere.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define NUM_CHANNELS 3 // number of channels (r, g, b, or a)
#define WHITE color(1.0,1.0,1.0)
#define BLUE color(0.5, 0.7, 1.0)
#define RED color(1,0,0);

color lerp(color a, color b, double t) {
    return (1.0 - t) * a + t*b;
}

color ray_color(const ray& r, const color & background, const hittable & world, shared_ptr<hittable>& lights, int depth) {
    hit_record rec;

    if (depth <= 0) return color(0,0,0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    color attenuation;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
    double pdf_val;
    color albedo;

    if (!rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf_val))
        return emitted;

    auto p0 = make_shared<hittable_pdf>(lights, rec.p);
    auto p1 = make_shared<cosine_pdf>(rec.normal);
    mixture_pdf mixed_pdf(p0, p1);

    scattered = ray(rec.p, mixed_pdf.generate(), r.time());
    pdf_val = mixed_pdf.value(scattered.direction());

    return emitted
         + albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered)
                  * ray_color(scattered, background, world, lights, depth-1) / pdf_val;
}

// hittable_list moon() {
//     hittable_list objects;

//     auto light = make_shared<diffuse_light>(color(10, 10, 10));

//     objects.add(make_shared<sphere>(point3(30,0,0), 5, light));
//     //objects.add(make_shared<sphere>(point3(0,0,5), 1, light));


//     auto moon_texture = make_shared<image_texture>("moon.jpg");
//     auto moon_surface = make_shared<nayer>(moon_texture);
//     objects.add(make_shared<sphere>(point3(0,0,0), 2, moon_surface));

//     return objects;
// }


hittable_list cornell_box() {
    hittable_list objects;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0,0,0), point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0,0,0), point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    objects.add(box2);

    return objects;
}

// hittable_list simple_light() {
//     hittable_list objects;

//     auto pertext = make_shared<noise_texture>(10);
//     shared_ptr<hittable> sphere1 = make_shared<sphere>(point3(0,100,0), 100, make_shared<lambertian>(pertext));
//     objects.add(make_shared<constant_medium>(sphere1, 0.01, pertext));
//     objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));

//     objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<dielectric>(1.5)));
//     shared_ptr<hittable> boundary = make_shared<sphere>(point3(0,2,0), 1.99, make_shared<lambertian>(pertext)); // 0.94 0.5 0.5
//     objects.add(make_shared<constant_medium>(boundary, .2, pertext));


//     auto difflight = make_shared<diffuse_light>(color(5,5,5));
//     objects.add(make_shared<xy_rect>(3, 7, 1, 5, -5, difflight));

//     return objects;
// }

// hittable_list bubble() {
//     hittable_list objects;

//     objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(color(.50, .8, 0.23))));

//     auto bubbletex = make_shared<bubble_texture>(pi);
//     objects.add(make_shared<sphere>(point3(0,2,0), -1.99, make_shared<dielectric>(1.0, bubbletex)));
//     objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<dielectric>(1.0, bubbletex)));

//     auto difflight = make_shared<diffuse_light>(color(5,5,5));
//     objects.add(make_shared<xy_rect>(3, 7, 1, 5, -5, difflight));
    
//     return objects;
// }

// hittable_list clouds() {
//     hittable_list objects;

//     shared_ptr<hittable> boundary = make_shared<sphere>(point3(0,2,0), 1.99, make_shared<lambertian>(color(1.0, 1.0, 1.0)));

//     objects.add(make_shared<turbulent_medium>(boundary, .5, color(1., 0., 0.)));

//     objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(color(.50, .8, 0.23))));
//     objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<dielectric>(1.0, color(1.0, 1.0, 1.0))));

//     //auto difflight = make_shared<diffuse_light>(color(5,5,5));
//     //objects.add(make_shared<xy_rect>(3, 13, 1, 11, -50, difflight));
    
//     return objects;
// }

// hittable_list two_perlin_spheres() {
//     hittable_list objects;

//     auto pertext = make_shared<noise_texture>(pi);
//     objects.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(pertext)));
//     objects.add(make_shared<sphere>(point3(0,2,0), 2, make_shared<lambertian>(pertext)));

//     return objects;
// }

// hittable_list two_spheres() {
//     hittable_list objects;

//     auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

//     auto moon_texture = make_shared<image_texture>("moon.jpg");
//     auto moon_surface = make_shared<nayer>(moon_texture);

//     objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<nayer>(checker)));
//     objects.add(make_shared<sphere>(point3(1, 1, 0), 1, moon_surface));
//     objects.add(make_shared<sphere>(point3(-1, 1, 0), 1, make_shared<lambertian>(color(0.5, 0.5, 0.5))));

//     return objects;
// }

// hittable_list random_scene() {
//     hittable_list world;

//     // auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
//     // world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

//     auto checker = make_shared<checker_texture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
//     world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

//     for (int a = -11; a < 11; a++) {
//         for (int b = -11; b < 11; b++) {
//             auto choose_mat = random_double();
//             point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

//             if ((center - point3(4, 0.2, 0)).length() > 0.9) {
//                 shared_ptr<material> sphere_material;

//                 if (choose_mat < 0.8) {
//                     // diffuse
//                     auto albedo = color::random() * color::random();
//                     sphere_material = make_shared<lambertian>(albedo);
//                     world.add(make_shared<sphere>(center, 0.2, sphere_material));
//                 } else if (choose_mat < 0.95) {
//                     // metal
//                     auto albedo = color::random(0.5, 1);
//                     auto fuzz = random_double(0, 0.5);
//                     sphere_material = make_shared<metal>(albedo, fuzz);
//                     world.add(make_shared<sphere>(center, 0.2, sphere_material));
//                 } else {
//                     // glass
//                     sphere_material = make_shared<dielectric>(1.5);
//                     world.add(make_shared<sphere>(center, 0.2, sphere_material));
//                 }
//             }
//         }
//     }

//     auto material1 = make_shared<dielectric>(1.5);
//     world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

//     auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
//     world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

//     auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
//     world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

//     return hittable_list(make_shared<bvh_node>(world, 1.0, 1.0));
// }

// // hittable_list final_scene() {
//     hittable_list boxes1;
//     auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

//     const int boxes_per_side = 20;
//     for (int i = 0; i < boxes_per_side; i++) {
//         for (int j = 0; j < boxes_per_side; j++) {
//             auto w = 100.0;
//             auto x0 = -1000.0 + i*w;
//             auto z0 = -1000.0 + j*w;
//             auto y0 = 0.0;
//             auto x1 = x0 + w;
//             auto y1 = random_double(1,101);
//             auto z1 = z0 + w;

//             boxes1.add(make_shared<box>(point3(x0,y0,z0), point3(x1,y1,z1), ground));
//         }
//     }

//     hittable_list objects;

//     objects.add(make_shared<bvh_node>(boxes1, 0, 1));

//     auto light = make_shared<diffuse_light>(color(7, 7, 7));
//     objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

//     auto center1 = point3(400, 400, 200);
//     auto center2 = center1 + vec3(30,0,0);
//     auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
//     objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

//     objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
//     objects.add(make_shared<sphere>(
//         point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
//     ));

//     auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
//     objects.add(boundary);
//     objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
//     boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
//     objects.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

//     auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
//     objects.add(make_shared<sphere>(point3(400,200,400), 100, emat));
//     auto pertext = make_shared<noise_texture>(0.1);
//     objects.add(make_shared<sphere>(point3(220,280,300), 80, make_shared<lambertian>(pertext)));

//     hittable_list boxes2;
//     auto white = make_shared<lambertian>(color(.73, .73, .73));
//     int ns = 1000;
//     for (int j = 0; j < ns; j++) {
//         boxes2.add(make_shared<sphere>(point3::random(0,165), 10, white));
//     }

//     objects.add(make_shared<translate>(
//         make_shared<rotate_y>(
//             make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
//             vec3(-100,270,395)
//         )
//     );

//     return objects;
// }

int main() {
    // image configurations
    auto aspect_ratio = 1.0;
    int image_width = 600;
    int image_height = static_cast<int>(image_width / aspect_ratio);
    int samples_per_pixel = 100;
    int max_depth = 50;

    hittable_list world;

    point3 lookfrom;
    point3 lookat;
    auto vfov = 45.0;
    auto aperture = 0.0;
    color background(0,0,0);


    world = cornell_box();
    shared_ptr<hittable> lights = make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>());
    aspect_ratio = 1.0;
    image_width = 600;
    samples_per_pixel = 1000;
    lookfrom = point3(278, 278, -800);
    lookat = point3(278, 278, 0);
    vfov = 40.0;

//    switch (6) {
//         case 1:
//             world = random_scene();
//             background = color(0.70, 0.80, 1.00);
//             lookfrom = point3(13,2,3);
//             lookat = point3(0,0,0);
//             vfov = 20.0;
//             aperture = 0.1;
//             break;

//         case 2:
//             world = two_spheres();
//             background = color(0.70, 0.80, 1.00);
//             lookfrom = point3(0,1,-10);
//             lookat = point3(0,1,0);
//             vfov = 20.0;
//             break;

//         case 3:
//             world = two_perlin_spheres();
//             background = color(0.70, 0.80, 1.00);
//             lookfrom = point3(13,2,3);
//             lookat = point3(0,0,0);
//             vfov = 20.0;
//             break;

//         case 4:
//             world = bubble();
//             background = color(0.70, 0.80, 1.00);
//             samples_per_pixel = 250;
//             lookfrom = point3(26,3,6);
//             lookat = point3(0,2,0);
//             vfov = 10.0;
//             break;
//         case 5:
//             world = moon();
//             samples_per_pixel = 1000;
//             lookfrom = point3(13,2,3);
//             lookat = point3(0,0,0);
//             vfov = 20.0;
//             break;          
//         case 6:
//             world = cornell_box();
//             aspect_ratio = 1.0;
//             image_width = 600;
//             samples_per_pixel = 250;
//             lookfrom = point3(278, 278, -800);
//             lookat = point3(278, 278, 0);
//             vfov = 40.0;
//             break;
//         case 7:
//             world = final_scene();
//             aspect_ratio = 1.0;
//             image_width = 800;
//             samples_per_pixel = 5000;
//             background = color(0,0,0);
//             lookfrom = point3(478, 278, -600);
//             lookat = point3(278, 278, 0);
//             vfov = 40.0;
//             break;
//         default:
//         case 8:
//             world = clouds();
//             background = color(0.70, 0.80, 1.00);
//             samples_per_pixel = 100;
//             lookfrom = point3(26,3,6);
//             lookat = point3(0,2,0);
//             vfov = 20.0;
//             break;        
//     }

    // Camera

    vec3 vup(0,1,0);
    auto dist_to_focus = 10.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    // create buffer of pixel data
    uint8_t * pixels = new uint8_t [ image_width * image_height * NUM_CHANNELS];
    
    int index = 0;
    for (int j = image_height-1; j >= 0; j--) {
        int percent_left = static_cast<int>(100*((float)j/(float)image_height));
        std::cerr << "\rScanlines Remaining: " << j << " | " << percent_left << "\% left" << " | "<< std::flush;
        for (int i = 0; i < image_width; i++) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r  = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, lights, max_depth);
            }
            write_color(pixels, pixel_color, index, samples_per_pixel);
        } // iterate over width
    } // iterate over height

    // Write Image Using stbi_image_write
    stbi_write_jpg("out.jpg", image_width, image_height, NUM_CHANNELS, pixels, 100);
    std::cerr << "\nDone.\n";
}