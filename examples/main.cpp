#include <iostream>
#include <iomanip>

#include <matrix.hpp>
#include "matrix_filter.hpp"

void blur(Matrix<float>& m);
void gradient(Matrix<float>& m);

int main() {
    // Load Image
    std::string file_in = "in_lena.png";

    Matrix<unsigned char> m(file_in.data());
    if (m.empty()) {
        std::cerr << "image is empty\n";
        return -1;
    }
    std::cout << "loaded with size: " << m.size() << '\n';

    auto edges = matrix_filter::convert_to<float>(m, 1/255.f);
    auto edges_gray = matrix_filter::grayscale(edges);

    blur(edges_gray);
    gradient(edges_gray);

    auto uchar_edges = matrix_filter::convert_to<uchar>(edges_gray, 255);
    uchar_edges.write("edges.jpg");
    
    return 0;
}

void gradient(Matrix<float>& m) {
    Matrix<float> sobel_x(3, 3, 1);
    {
        auto p = sobel_x.ptr(0);
        p[0] = -1.f; p[1] = 0.f; p[2] = 1.f;
        p[3] = -2.f; p[4] = 0.f; p[5] = 2.f;
        p[6] = -1.f; p[7] = 0.f; p[8] = 1.f;
    }
    Matrix<float> sobel_y(3, 3, 1);
    {
        auto p = sobel_y.ptr(0);
        p[0] = -1.f; p[1] = -2.f; p[2] = -1.f;
        p[3] = 0.f; p[4] = 0.f; p[5] = 0.f;
        p[6] = 1.f; p[7] = 2.f; p[8] = 1.f;
    }

    auto dx = matrix_filter::cross_correlation(m, sobel_x);
    auto dy = matrix_filter::cross_correlation(m, sobel_y);

    // store magnitude in dx and angle in dy
    matrix_filter::magnitude_angle(dx, dy, dx, dy);

    auto uchar_grad = matrix_filter::convert_to<uchar>(dx, 255);
    uchar_grad.write("grad.jpg");

    m = matrix_filter::non_maximum_suppression(dx, dy, 1.f);
}

void blur(Matrix<float>& m) {
    auto gauss = matrix_filter::get_gaussian<float>(5, 1.5);
    m = matrix_filter::cross_correlation(m, gauss);
}
