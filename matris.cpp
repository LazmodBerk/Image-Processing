#include "matris.h"
#include "image.h"
#include <cstdlib>

Matris make_matris(int w, int h, int c) {
    Matris m;
    m.w = w;
    m.h = h;                            // her piksel için c tane değer var
    m.c = c;
    m.data = new float[w * h * c](); // sıfırla
    return m;
}

void free_matris(Matris m) {
    delete[] m.data;
}

image matris_to_image(const Matris& m) {
    image im;
    im.w = m.w;
    im.h = m.h;                                 // boyutlar kopyalanır
    im.c = m.c;
    int total = m.w * m.h * m.c;
    unsigned char* data = (unsigned char*)malloc(total);        // matris boyutu kadar yer ayırır
    if (!data) {
        im.data = nullptr;
        return im;
    }
    for (int i = 0; i < total; ++i) {
        int val = (int)(m.data[i] * 255.0f + 0.5f);         // float -> byte  (0.0-1.0  -->  0 - 255) + 0.5f ile 152.7 -> 153 olur
        if (val < 0) 
            val = 0;                               
        if (val > 255)
            val = 255;                       
        data[i] = (unsigned char)val;                       // byte a çevirir
    }
    im.data = data;                                     // image'e aktarır
    return im;
}

void free_image(image im) {
    free(im.data);
}