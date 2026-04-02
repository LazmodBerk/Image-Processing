#pragma once

struct image;   // forward declaration (matris.cpp de image kullanabilmek için)

struct Matris {
    int w, h, c;                            // weight, height, channel (rgb 3 channel)
    float* data;
};

Matris make_matris(int w, int h, int c);
void free_matris(Matris m);
image matris_to_image(const Matris& m);
void free_image(image im);