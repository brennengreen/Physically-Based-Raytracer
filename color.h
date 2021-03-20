#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

void write_color(uint8_t * buffer, color pixel_color, int &index) {
    int ir = int(255.99f * pixel_color.x());
    int ig = int(255.99f * pixel_color.y());
    int ib = int(255.99f * pixel_color.z());
    buffer[index++] = ir;
    buffer[index++] = ig;
    buffer[index++] = ib;
}

#endif