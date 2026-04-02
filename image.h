#pragma once

struct Matris;

struct image {
    int w;
    int h;
    int c;
    unsigned char* data;
};

Matris* load_image(const char* filename);           // okunan resmi matris formatına çevirir

// Dosya (jpg/png) → unsigned char (0–255) → float (0–1) → Matris