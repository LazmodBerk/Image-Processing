#pragma once
#include "matris.h"

// 2D konvolüsyon (filtre/kernel ile bulanýklaþtýrma)
Matris conv2d(const Matris& src, const Matris& kernel, int stride, int padding);

// RGB -> gri (görüntü tek kanallý gri (intensity) görüntü)
Matris rgbToIntensity(const Matris& rgb);

// Gaussian smoothing (5x5) (gaussian filtresi ile gürültü azaltma)
Matris Smoothing(const Matris& intensity);

// Gradyan (büyüklük ve açý) (görüntüdeki kenar þiddeti (büyüklük) ve yönü (açý) hesaplanýr)
Matris Gradient(const Matris& src, float*& angles);

// Non-maximum suppression (kenarlarý inceltmek için gradyanlar býrakýlýr diðerleri bastýrýlýr)
Matris MaxSuppression(const Matris& grad, const float* angles);

// Canny edge (eþikleme + histerezis) (kenarlar seçilerek nihai kenar görüntüsü)
Matris CannyEdge(const Matris& thin, float lowThreshold, float highThreshold);

// Hough Line: son parametre istenen çizgi sayýsý 
float* Hough_Line(const Matris& edge, const float* angles, int& num_lines,
    int thetaSteps, int houghThreshold, int desired_lines);

// Hough Circle: son parametre istenen çember sayýsý
struct point2D { int x, y; };
point2D* Hough_Circle(const Matris& edge, const float* angles,
    float*& radius, int& num_circles,
    int minRadius, int maxRadius, int houghThreshold, int desired_circles);

// Hough uzayýný görüntü olarak döndür (çizgi için)
Matris GetHoughSpaceImage(const Matris& binaryEdge, int thetaSteps, int rhoSteps);

// Çizim fonksiyonlarý
void DrawSelectedLines(Matris& intensity, const float* lines, int num_of_lines);
void DrawSelectedCircles(Matris& intensity, const point2D* centers, const float* radius, int num_of_circles);