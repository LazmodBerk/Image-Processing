#include "image.h"
#include "matris.h"   // Matris yapýsýnýn tam tanýmý için

#define STB_IMAGE_IMPLEMENTATION
#include "stb/include/stb_image.h"
#include <cstdlib>

Matris* load_image(const char* filename) {                  
    int w, h, c;
    int channel = 3; // 3 kanal olarak yükle (RGB)
    unsigned char* data = stbi_load(filename, &w, &h, &c, channel);
    if (!data) {
        exit(EXIT_FAILURE);
    }
    Matris* m = new Matris;
    m->w = w;
    m->h = h;
    m->c = c;
    int total = w * h * c;
    m->data = new float[total];
    for (int i = 0; i < total; ++i) {
        m->data[i] = data[i] / 255.0f;              // 0-255 ten 0-1 e çevrilir 
    }
    stbi_image_free(data);
    return m;
}