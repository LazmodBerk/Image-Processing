#include "goruntu_isleme.h"
#include <cmath>
#include <algorithm>
#include <cstring>
#include <vector>

// Yardımcı: derece-radyan dönüşümü
#define _USE_MATH_DEFINES
#include <math.h>
static inline float deg2rad(float deg) { return deg * (float)M_PI / 180.0f; }


// 2D Konvolüsyon    (PADDING boyut koruma sağlar)

Matris conv2d(const Matris& src, const Matris& kernel, int stride, int padding) {
    int imgecol = src.w;
    int imgerow = src.h;                                           
    int kcol = kernel.w;
    int krow = kernel.h;
        
    int ocol = (imgecol - kcol + 2 * padding) / stride + 1;                                                                                                                       // konvolüsyon (çıktı satır sütun hesabı)
    int orow = (imgerow - krow + 2 * padding) / stride + 1;

    Matris out = make_matris(ocol, orow, 1);                  

    for (int r = 0; r < orow; ++r) {
        for (int c = 0; c < ocol; ++c) {                                                                                                                                                // kerneli görüntü üzerinde gezdir
            float sum = 0.0f;
            for (int m = 0; m < krow; ++m) {
                for (int n = 0; n < kcol; ++n) {               
                    int onr = r * stride + m - padding;                                                                                                      // kernelin baktığı satır sütun
                    int onc = c * stride + n - padding;
                    if (onr >= 0 && onr < imgerow && onc >= 0 && onc < imgecol) {                                                                                         // sınır kontrolü (padding)
                        sum += src.data[onr * imgecol + onc] * kernel.data[m * kcol + n];                                                                                        // sum += image * kernel
                    }           
                }                           
            }
            out.data[r * ocol + c] = sum;
        }
    }
    return out;             // filtrelenmiş görüntüyü çevir
}



// RGB -> Gri (kanal ortalaması değil, luminance hesabı)           (luminance = ağırlıklı gri (son) hesaplama)

Matris rgbToIntensity(const Matris& rgb) {
    Matris gray = make_matris(rgb.w, rgb.h, 1);
    for (int i = 0; i < rgb.w * rgb.h; ++i) {           // tüm pikselleri tek tek gezer
        float r = rgb.data[i * 3 + 0];                          // r renk pikseli
        float g = rgb.data[i * 3 + 1];                          // g renk pikseli
        float b = rgb.data[i * 3 + 2];                          // b renk pikseli
        gray.data[i] = 0.299f * r + 0.587f * g + 0.114f * b;
    }
    return gray;
}

// Gaussian Smoothing (3x3 kernel, conv2d kullanır)

Matris Smoothing(const Matris& intensity) {
    // 3x3 Gaussian kernel (yaklaşık sigma=0.8)
    float k[3][3] = {                               
        {1, 2, 1},
        {2, 4, 2},                                                   
        {1, 2, 1}
    };

    Matris kernel = make_matris(3, 3, 1);                                 
    float sum = 0.0f;

    // tek boyuta çevirme
    for (int i = 0; i < 9; ++i) {
        kernel.data[i] = k[i / 3][i % 3];           
        sum += kernel.data[i];                                               
    }

    // normalizasyon (yoksa piksel beyazlaşır)
    for (int i = 0; i < 9; ++i)
        kernel.data[i] /= sum;                      

    // convolution işlemi uygulanır. 
    // 3x3 kernel için padding = 1 olmalı giriş matrisiyle çıkış matrisinin (resmin) boyutları aynı kalsın. kernel 1 piksel dışarı taşar.
    Matris smooth = conv2d(intensity, kernel, 1, 1);

    free_matris(kernel);                           
    return smooth;                        
}

// Sobel gradyan (conv2d ile) // conv2d x ve y yönünde türev bulmak için (sağ piksel - sol piksel (türev tanımı sağ - sol))

Matris Gradient(const Matris& src, float*& angles) {                              
    // Sobel X
    float sobelX[3][3] = { {-1,0,1}, {-2,0,2}, {-1,0,1} };                          // x yönü (sağa doğru) değişim (dikey kenarları bulur)
    Matris kenarx = make_matris(3, 3, 1);

    // 3*3 lük kerneli tek boyuta çevirme
    for (int i = 0; i < 9; ++i) 
        kenarx.data[i] = sobelX[i / 3][i % 3];                      

    // Sobel Y
    float sobelY[3][3] = { {-1,-2,-1}, {0,0,0}, {1,2,1} };                                // y yönü (yukarı doğru) değişim (yatay kenarları bulur)
    Matris kenary = make_matris(3, 3, 1);
    for (int i = 0; i < 9; ++i) 
        kenary.data[i] = sobelY[i / 3][i % 3];

    // conv2d ile türev (dx / dy) , padding = 1 (kenar kaybı yok)
    Matris gradx = conv2d(src, kenarx, 1, 1);                          
    Matris grady = conv2d(src, kenary, 1, 1);

    free_matris(kenarx);
    free_matris(kenary);

    Matris grad = make_matris(src.w, src.h, 1);                                     // gradyan büyüklüğü
    angles = new float[src.w * src.h];                                               // gradyan yönü

    for (int y = 0; y < src.h; ++y) {
        for (int x = 0; x < src.w; ++x) {
            float dx = gradx.data[y * src.w + x];
            float dy = grady.data[y * src.w + x];
            grad.data[y * src.w + x] = sqrtf(dx * dx + dy * dy);                     // gradyan büyüklüğü
            angles[y * src.w + x] = atan2f(dy, dx); // radyan                            // gradyan yönü
        }
    }

    free_matris(gradx);
    free_matris(grady);
    return grad;
}


// Non-maximum suppression (gradyan yönüne göre)  AMAÇ: Kenarları inceltmek... gradyan yönüne bak 2 komşuyla karşılaştır

Matris MaxSuppression(const Matris& grad, const float* angles) {
    int w = grad.w, h = grad.h;
    Matris result = make_matris(w, h, 1);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float angle = angles[y * w + x];                                           // her pikselin bulunan gradient yönü tutulur dizide.
            // radyanı [0, pi) arasına getir   (NORMALİZASYON)
            angle = fmod(angle + M_PI, M_PI);
            // yön sınıflandırma (0°, 45°, 90°, 135°)
            int dir = 0;
            if ((angle >= 0 && angle < 22.5 * M_PI / 180) || (angle >= 157.5 * M_PI / 180 && angle < 180 * M_PI / 180))
                dir = 0;      // yatay
            else if (angle >= 22.5 * M_PI / 180 && angle < 67.5 * M_PI / 180)
                dir = 1;      // 45°
            else if (angle >= 67.5 * M_PI / 180 && angle < 112.5 * M_PI / 180)
                dir = 2;      // dikey
            else if (angle >= 112.5 * M_PI / 180 && angle < 157.5 * M_PI / 180)
                dir = 3;      // 135°

            float val = grad.data[y * w + x];
            float left = 0, right = 0;                                                 // komşuları seçme kısmı
            switch (dir) {
            case 0: // yatay (sağ-sol)
                if (x > 0)
                    left = grad.data[y * w + x - 1];
                if (x < w - 1) 
                    right = grad.data[y * w + x + 1];
                break;
            case 1: // 45° (çapraz)
                if (x > 0 && y > 0) 
                    left = grad.data[(y - 1) * w + x - 1];
                if (x < w - 1 && y < h - 1)
                    right = grad.data[(y + 1) * w + x + 1];
                break;
            case 2: // dikey (yukarı aşağı)
                if (y > 0)
                    left = grad.data[(y - 1) * w + x];
                if (y < h - 1)
                    right = grad.data[(y + 1) * w + x];
                break;
            case 3: // 135° (ters çapraz)
                if (x < w - 1 && y > 0) 
                    left = grad.data[(y - 1) * w + x + 1];
                if (x > 0 && y < h - 1)
                    right = grad.data[(y + 1) * w + x - 1];
                break;
            }
            if (val >= left && val >= right)                                      // pikselleri bastırma işlemi (komşulardan büyükse koru değilse sil)
                result.data[y * w + x] = val;
            else
                result.data[y * w + x] = 0;
        }
    }
    return result;
}


// Canny Edge (çift eşik + histerezis)   AMAÇ: gerçek kenarları seçmek

Matris CannyEdge(const Matris& thin, float lowThresh, float highThresh) {                                   
    int w = thin.w, h = thin.h;                                                                             
    Matris edge = make_matris(w, h, 1);

    // Maksimum gradyan bul (normalizasyon için)
    float maxVal = 0;                                                                            
    for (int i = 0; i < w * h; ++i) 
        if (thin.data[i] > maxVal) 
            maxVal = thin.data[i];
    if (maxVal == 0)                                                                              // hiç kenar yoksa
        maxVal = 1;
    lowThresh *= maxVal;                                                                         // max gradyana göre ölçeklenen eşikler doğru orantılı olur
    highThresh *= maxVal;

    // İşaretleme: strong = 1 (kesin kenar), weak = 0.5 (kenar olabilir,bağlanmalı), background = 0 (kenar değil, sil)
    for (int i = 0; i < w * h; ++i) {
        if (thin.data[i] >= highThresh)
            edge.data[i] = 1.0f;
        else if (thin.data[i] >= lowThresh)
            edge.data[i] = 0.5f;
        else
            edge.data[i] = 0.0f;
    }

    // Histerezis için yığın (stack) – maksimum nokta sayısı kadar yer ayır
    int maxStrong = w * h;
    int* strongX = new int[maxStrong];                                                   // kenarların x koordinatı
    int* strongY = new int[maxStrong];                                                   // kenarların y koordinatı
    int strongCount = 0;

    // Strong pikselleri topla
    for (int y = 0; y < h; ++y) {                                                            // satırları gez 
        for (int x = 0; x < w; ++x) {                                                        // sütunları gez
            if (edge.data[y * w + x] == 1.0f) {
                strongX[strongCount] = x;
                strongY[strongCount] = y;
                strongCount++;
            }
        }
    }
    
    // histerezis ile weakleri stronga yükseltme

    int dirx[] = { -1,0,1,-1,1,-1,0,1 };                                                  // yukarı,aşağı,sağ,sol,4 çarpraz komşuyu temsil eder
    int diry[] = { -1,-1,-1,0,0,1,1,1 };

    // stack mantığı LIFO
    while (strongCount > 0) {
        strongCount--;                                                                  // son eleman alınır
        int x = strongX[strongCount];
        int y = strongY[strongCount];
        for (int k = 0; k < 8; ++k) {
            int nx = x + dirx[k];                                                         // komşu koordinatı hesapla
            int ny = y + diry[k];
            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                if (edge.data[ny * w + nx] == 0.5f) {
                    edge.data[ny * w + nx] = 1.0f;                                          // eğer ara değerse ve güçlü komşu varsa elemanı güçlü yap
                    strongX[strongCount] = nx;
                    strongY[strongCount] = ny;
                    strongCount++;
                }
            }
        }
    }

    delete[] strongX;
    delete[] strongY;

    // Weak olarak kalanları kaldır
    for (int i = 0; i < w * h; ++i)
        if (edge.data[i] != 1.0f)
            edge.data[i] = 0.0f;

    return edge;
}


// Hough Line (isteğe bağlı adet – arayüzde desired_lines)

struct Peak { int rhoIdx; int thetaIdx; int votes; };                           // en yüksek oy alan hücreleri tutar (gerçek doğru olduğunu söyler)

float* Hough_Line(const Matris& edge, const float* angles, int& num_lines,
    int thetaSteps, int houghThreshold, int desired_lines) {                            // houghThreshold line lar için 150 olmalı

    if (desired_lines <= 0) {                                                       
        num_lines = 0;
        return nullptr;
    }

    int w = edge.w, h = edge.h;
    float rhoMax = sqrtf(w * w + h * h);                                              // max köşegen uzunluğu
    float rhoMin = -rhoMax;
    float rhoStep = (2 * rhoMax) / (houghThreshold * 2);                              // hücre genişliği payda hücre sayısını verir
    int rhoSteps = (int)((rhoMax - rhoMin) / rhoStep) + 1;                            // kaç hücre olacağı

    int* accumulator = new int[rhoSteps * thetaSteps]();                               // () ile değerler sıfırlanır

    float thetaStep = (float)M_PI / thetaSteps;

    // Oylama aşaması
    for (int y = 0; y < h; ++y) {                                                  // image satırları
        for (int x = 0; x < w; ++x) {                                              // image sütunları
            if (edge.data[y * w + x] > 0) {                                        // kenar pikseliyse işlem yap
                for (int t = 0; t < thetaSteps; ++t) {
                    float theta = t * thetaStep;
                    float rho = x * cosf(theta) + y * sinf(theta);
                    int rhoIdx = (int)((rho - rhoMin) / rhoStep);                           // hangi hücreye denk gelir

                    if (rhoIdx >= 0 && rhoIdx < rhoSteps) {
                        accumulator[rhoIdx * thetaSteps + t]++;                         // t = theta
                    }
                }
            }
        }
    }

    // sadece en iyi N sonucu tutacak dizi
    Peak* top_peaks = new Peak[desired_lines](); // Oyları 0 olarak başlar
    int found_peaks = 0;

    // Tepe noktalarını bul ve anında sıralı diziye ekle
    for (int r = 0; r < rhoSteps; ++r) {
        for (int t = 0; t < thetaSteps; ++t) {
            int val = accumulator[r * thetaSteps + t];
            
            // Non-max suppression  (-4 -> 4 gider yakın doğruların çakışmasını önler)
            if (val >= houghThreshold) {
                bool isMax = true;
                for (int dr = -4; dr <= 4 && isMax; ++dr) {                                    // rho ekseninde komşuluk kayması  
                    for (int dt = -4; dt <= 4; ++dt) {                                         // theta ekseninde komşuluk kayması
                        int nr = r + dr, nt = t + dt; 
                        if (nr >= 0 && nr < rhoSteps && nt >= 0 && nt < thetaSteps) {          
                            int nVal = accumulator[nr * thetaSteps + nt];
                            if (nVal > val) {                                                   
                                isMax = false;
                                break;
                            }
                            else if (nVal == val && (dr != 0 || dt != 0)) {                 
                                if (nr > r || (nr == r && nt > t))                          // indeksi daha büyük olan alınır (Tie-Breaker)
                                    isMax = false;
                            }
                        }
                    }
                }

                // Eğer bu nokta bir tepe noktasıysa ve dizideki en küçükten daha iyiyse
                if (isMax && val > top_peaks[desired_lines - 1].votes) {            
                    int pos = desired_lines - 1;                                            // listenin sonundan taramaya başlanır
                    // Doğru pozisyonu bul ve diğerlerini kaydır  (insertion sort)
                    while (pos > 0 && top_peaks[pos - 1].votes < val) {
                        top_peaks[pos] = top_peaks[pos - 1];
                        pos--;
                    }
                    top_peaks[pos] = { r, t, val };
                    if (found_peaks < desired_lines) 
                        found_peaks++;
                }
            }
        }
    }

    num_lines = found_peaks;
    float* lines = nullptr;

    if (num_lines > 0) {
        lines = new float[num_lines * 2];                                   // rho1, theta1, rho2, theta2, ... bu yüzden indeks *2 oldu
        for (int i = 0; i < num_lines; ++i) {
            lines[i * 2] = rhoMin + top_peaks[i].rhoIdx * rhoStep;
            lines[i * 2 + 1] = top_peaks[i].thetaIdx * thetaStep;
        }
    }

    // Belleği temizle
    delete[] accumulator;
    delete[] top_peaks;

    return lines;
}


// Hough Circle (isteğe bağlı adet – arayüzde desired_circles) ÇEMBER TESPİTİ KENARLARDAN YAPILIR

struct CirclePeak { int x, y, r, votes; };

point2D* Hough_Circle(const Matris& edge, const float* angles,
    float*& radii, int& num_circles,                                            // houghThreshold çemberler için 17 civarı olmalı çember çevresi kısa alacağı oy sayısı düşük
    int minR, int maxR, int houghThreshold, int desired_circles) {                      

    if (desired_circles <= 0) {
        num_circles = 0;
        radii = nullptr;
        return nullptr;
    }

    int w = edge.w, h = edge.h;
    int rRange = maxR - minR + 1;                                           // yarıçap aralğının kaç farklı değer aldığı

    int acc_size = w * h * rRange;
    int* accumulator = new int[acc_size](); // () sayesinde tüm elemanlar 0 ile başlar

    // 3D koordinatı (x, y, r) 1D indexe çeviren pratik yardımcı fonksiyon
    auto getAccIdx = [&](int x, int y, int r_idx) {
        return (y * w + x) * rRange + r_idx;                                        // her (x,y) için rRange kadar yer ayırma
    };

    // Gradyan yönlerini kullanarak oylama
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (edge.data[y * w + x] > 0) {                                     // sadece kenar pikselleri kullanılır çünkü çember tespiti kenardan yapılır
                float angle = angles[y * w + x];
                float dx = cosf(angle);                                        
                float dy = sinf(angle);
                for (int r = minR; r <= maxR; ++r) {                            // her yarıçap için merkez tahmin edilir
                    int r_idx = r - minR;
                    int xc1 = (int)(x + r * dx);
                    int yc1 = (int)(y + r * dy);                                // merkez her 2 tarafta da olabilir (sağda (dış) veya solda (iç))
                    int xc2 = (int)(x - r * dx);
                    int yc2 = (int)(y - r * dy);

                    if (xc1 >= 0 && xc1 < w && yc1 >= 0 && yc1 < h)
                        accumulator[getAccIdx(xc1, yc1, r_idx)]++;
                    if (xc2 >= 0 && xc2 < w && yc2 >= 0 && yc2 < h)
                        accumulator[getAccIdx(xc2, yc2, r_idx)]++;
                }
            }
        }
    }

    int max_peaks = w * h; // Olabilecek maksimum aday sayısı (her piksel için en fazla 1 tepe)
    CirclePeak* peaks = new CirclePeak[max_peaks];
    int peakCount = 0;

    // Tepe noktalarını bul (basit 3x3x3 komşuluk)
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {                                                           // image pikselleri gezilir
            for (int ri = 0; ri < rRange; ++ri) {
                int val = accumulator[getAccIdx(x, y, ri)];

                if (val >= houghThreshold) {
                    bool isMax = true;
                    for (int dx = -1; dx <= 1 && isMax; ++dx) {
                        for (int dy = -1; dy <= 1; ++dy) {                                                  // x,y ekseninde sol sağ aşağı yukardaki ve önceki sonraki yarıçap toplam 3*3*3 27 komşu
                            for (int dr = -1; dr <= 1; ++dr) {
                                int nx = x + dx, ny = y + dy, nr = ri + dr;
                                if (nx >= 0 && nx < w && ny >= 0 && ny < h && nr >= 0 && nr < rRange) {
                                    if (accumulator[getAccIdx(nx, ny, nr)] > val) {
                                        isMax = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if (isMax&& peakCount < max_peaks) {
                        peaks[peakCount++] = { x, y, ri + minR, val };
                    }
                }
            }
        }
    }

    // sort(start pointer, end pointer, comparator function)
    // Oy sayısına göre sırala sadece sıralar bişe döndürmez geri    (void function)   (pointerlar üzerinden standart sıralama yapılabilir)  (LAMBDA FUNCTİON KULLANILDI)
    std::sort(peaks, peaks + peakCount, [](const CirclePeak& a, const CirclePeak& b) {
        return a.votes > b.votes;
    });

    int take = (desired_circles < peakCount) ? desired_circles : peakCount;                     // kaç çember alıncak onu söyler
    num_circles = take;

    point2D* centers = nullptr;
    radii = nullptr;

    if (take > 0) {
        centers = new point2D[take];
        radii = new float[take];
        for (int i = 0; i < take; ++i) {
            centers[i].x = peaks[i].x;
            centers[i].y = peaks[i].y;
            radii[i] = (float)peaks[i].r;
        }
    }

    delete[] accumulator;
    delete[] peaks;

    return centers;
}


// Hough uzayını görüntü olarak döndür (çizgi için)
Matris GetHoughSpaceImage(const Matris& binaryEdge, int thetaSteps, int rhoSteps) {
    int w = binaryEdge.w, h = binaryEdge.h;
    float rhoMax = sqrtf(w * w + h * h);
    float rhoMin = -rhoMax;
    float rhoStep = (2 * rhoMax) / rhoSteps;

    int* accumulator = new int[rhoSteps * thetaSteps]();

    float thetaStep = (float)M_PI / thetaSteps;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (binaryEdge.data[y * w + x] > 0) {
                for (int t = 0; t < thetaSteps; ++t) {
                    float theta = t * thetaStep;
                    float rho = x * cosf(theta) + y * sinf(theta);
                    int rhoIdx = (int)((rho - rhoMin) / rhoStep);

                    if (rhoIdx >= 0 && rhoIdx < rhoSteps) {
                        // 2D indeksi 1D array üzerinde hesaplıyoruz: (satır * genişlik + sütun)
                        accumulator[rhoIdx * thetaSteps + t]++;
                    }
                }
            }
        }
    }

    int maxVote = 0;
    int totalCells = rhoSteps * thetaSteps;
    for (int i = 0; i < totalCells; ++i) {
        if (accumulator[i] > maxVote) {
            maxVote = accumulator[i];
        }
    }

    if (maxVote == 0) 
        maxVote = 1;

    // Normalize edip img matrisine aktarıyoruz
    Matris img = make_matris(thetaSteps, rhoSteps, 1);
    for (int r = 0; r < rhoSteps; ++r) {
        for (int t = 0; t < thetaSteps; ++t) {
            float val = (float)accumulator[r * thetaSteps + t] / maxVote * 255.0f;
            img.data[r * thetaSteps + t] = val;
        }
    }
    
    delete[] accumulator;

    return img;
}

// Çizgi çizme (RGB görüntü üzerine kırmızı)

void DrawSelectedLines(Matris& img, const float* lines, int num_lines) {
    int w = img.w, h = img.h;
    for (int i = 0; i < num_lines; ++i) {
        float rho = lines[i * 2];
        float theta = lines[i * 2 + 1];
        float a = cosf(theta), b = sinf(theta);
        float x0 = a * rho, y0 = b * rho;
        // X ekseni boyunca çiz
        for (int x = 0; x < w; ++x) {
            float y = (rho - x * a) / b;
            if (y >= 0 && y < h) {
                int idx = (int)y * w + x;
                img.data[idx * 3 + 0] = 255;
                img.data[idx * 3 + 1] = 0;
                img.data[idx * 3 + 2] = 0;
            }
        }
        // Y ekseni boyunca çiz
        for (int y = 0; y < h; ++y) {
            float x = (rho - y * b) / a;
            if (x >= 0 && x < w) {
                int idx = y * w + (int)x;
                img.data[idx * 3 + 0] = 255;
                img.data[idx * 3 + 1] = 0;
                img.data[idx * 3 + 2] = 0;
            }
        }
    }
}


// Çember çizme (RGB görüntü üzerine kırmızı)

void DrawSelectedCircles(Matris& img, const point2D* centers, const float* radii, int num_circles) {
    int w = img.w, h = img.h;
    for (int i = 0; i < num_circles; ++i) {
        int cx = centers[i].x, cy = centers[i].y;
        int r = (int)radii[i];
        for (int angle = 0; angle < 360; ++angle) {
            float rad = angle * M_PI / 180.0f;
            int x = cx + (int)(r * cosf(rad));
            int y = cy + (int)(r * sinf(rad));
            if (x >= 0 && x < w && y >= 0 && y < h) {
                int idx = y * w + x;
                img.data[idx * 3 + 0] = 255;
                img.data[idx * 3 + 1] = 0;
                img.data[idx * 3 + 2] = 0;
            }
        }
    }
}
