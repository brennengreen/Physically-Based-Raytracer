#include <iostream>
#include <math.h>
#include "rtweekend.h"
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable_list.h"
#include "sphere.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 1080 // image width
#define NUM_CHANNELS 3 // number of channels (r, g, b, or a)
#define WHITE color(1.0,1.0,1.0)
#define BLUE color(0.5, 0.7, 1.0)
#define RED color(1,0,0);

double hit_sphere(const point3& center, double radius, const ray& r) {
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius*radius;
    auto discriminant = half_b*half_b - a * c;
    if (discriminant < 0) {
        return -1.0;
    } else {
        return (-half_b - sqrt(discriminant)) / (a);
    }
}

color lerp(color a, color b, double t) {
    return (1.0 - t) * a + t*b;
}

color ray_color(const ray& r, const hittable & world) {
    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5*(unit_direction.y() + 1.0);
    return lerp(WHITE, BLUE, t);
}

int main() {
    // image configurations
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = WIDTH;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // world configurations
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(1, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(-1, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));


    // camera configurations
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focal_length);

    // create buffer of pixel data
    uint8_t * pixels = new uint8_t [ image_width * image_height * NUM_CHANNELS];
    
    int index = 0;
    for (int j = image_height-1; j >= 0; j--) {
        std::cerr << "\rScalines Remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height -1);
            ray r(origin, lower_left_corner + u*horizontal + v*vertical - origin);
            color pixel_color = ray_color(r, world);
            write_color(pixels, pixel_color, index);

        } // iterate over width
    } // iterate over height


    // Write Image Using stbi_image_write
    stbi_write_jpg("out.jpg", image_width, image_height, NUM_CHANNELS, pixels, 100);
    std::cerr << "\nDone.\n";
}