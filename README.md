# Image-Processing
This project is a custom-built computer vision engine in standard C++ without external libraries. It implements convolution, Gaussian filtering, Canny edge detection, and Hough Transform for line and circle detection, with visualization tools and optimized memory usage for educational purposes.

# Custom Computer Vision Engine (C++)

## Overview
This project is a custom-built computer vision and shape detection engine developed entirely from scratch in standard C++, without relying on external libraries such as OpenCV. It is designed as an educational resource for developers and students who want to gain a deeper understanding of the mathematical foundations, convolution mechanics, and manual memory management behind core image processing techniques.

The pipeline begins with fundamental matrix operations, including a custom 2D convolution system with boundary-preserving padding. It then converts RGB images to grayscale using perceptual weighting aligned with human vision, followed by 3×3 Gaussian smoothing to reduce noise effectively.

For edge detection, the engine implements the full Canny algorithm. This includes gradient magnitude and direction computation using the Sobel operator, edge thinning via Non-Maximum Suppression, and edge tracking through hysteresis thresholding to preserve meaningful structures.

On top of this foundation, the project features a detailed implementation of the Hough Transform. The Hough Line algorithm uses a rho-theta accumulator to detect linear patterns by identifying local maxima. The Hough Circle algorithm extends this approach with gradient direction guidance and an optimized 3D accumulator, enabling efficient circle detection within configurable radius ranges.

To support debugging and learning, the engine provides visualization tools that export accumulator voting densities as Hough Space maps and overlay detected lines and circles onto the original images.

Built entirely with standard C++ libraries, the project emphasizes performance and low-level control by using 1D arrays for multidimensional indexing. Overall, it serves as a comprehensive reference for understanding and implementing classical computer vision algorithms from first principles.


The pipeline includes filtering, edge detection, and shape recognition stages.

## Features
- Custom matrix structure using 1D arrays
- 2D convolution with padding
- RGB to Grayscale conversion
- Gaussian blur (3x3)
- Canny Edge Detection
- Hough Transform (Line & Circle)
- Visualization tools

## Pipeline

### Matrix Representation
2D indexing:
index = y * width + x

3D indexing:
index = (r * height + y) * width + x

### Convolution
Custom convolution is applied for filtering operations.
Padding is used to preserve borders.

### Grayscale Conversion
gray = 0.299R + 0.587G + 0.114B

### Gaussian Blur
Kernel:
1 2 1
2 4 2
1 2 1  / 16

### Canny Edge Detection
Steps:
- Gradient calculation (Sobel)
- Direction calculation (atan2)
- Non-Maximum Suppression
- Hysteresis Thresholding

Gradient:
G = sqrt(Gx^2 + Gy^2)
theta = atan2(Gy, Gx)

### Hough Line Transform
rho = x cos(theta) + y sin(theta)

Accumulator:
acc[rho, theta]++

### Hough Circle Transform
a = x - r cos(theta)
b = y - r sin(theta)

3D accumulator:
acc[r, y, x]++

### Visualization
- Hough Space output
- Detected shapes drawn on image

## Purpose
This project is useful for:
- Learning computer vision fundamentals
- Understanding algorithms from scratch
- Practicing low-level C++ optimization
