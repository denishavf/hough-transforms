#pragma once

#include <cassert>
#include <cmath>
#include <array>

#include "matrix.hpp"
#include "utils.hpp"

namespace matrix_filter {
    Matrix<uchar> grayscale(Matrix<uchar>& src);

    Matrix<float> get_gaussian(int gauss_len, double sigma);

    Matrix<float> get_sobel_x();
    Matrix<float> get_sobel_y();

    Matrix<float> cross_correlation(
            const Matrix<float>& src, const Matrix<float>& kernel);

    Matrix<uchar> cross_correlation_3c(
            const Matrix<uchar>& src, const Matrix<float>& kernel);

    // Assumes that dims of src1, src2 and dst_* are equal
    void magnitude_angle(
            Matrix<float>& src1, Matrix<float>& src2, 
            Matrix<float>& dst_magnitude, Matrix<float>& dst_angle);
    
    Matrix<float> non_maximum_suppression(
            Matrix<float>& magnitude,
            Matrix<float>& angle);

    void double_treshold(
        Matrix<uchar>& m,
        uchar low, uchar high,
        uchar weak = 25, uchar strong = 255);
    void double_treshold(
        Matrix<float>& m,
        float low, float high,
        float weak = 0.10, float strong = 1);

    void hysteresis(
        Matrix<float>& m,
        float weak = 0.10, float strong = 1);
}

