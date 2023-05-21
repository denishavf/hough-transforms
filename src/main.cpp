#include <iostream>
#include <iomanip>

#include <matrix.hpp>
#include <matrix_filter.hpp>

int main() {
    // Load Image
    std::string file_in = "door_med_res.jpg";

    Matrix<unsigned char> m(file_in.data());
    if (m.empty()) {
        std::cerr << "image is empty\n";
        return -1;
    }
    std::cout << "loaded with size: " << m.size() << '\n';

    // get grayscale of the image and convert it to float
    auto gray = matrix_filter::grayscale(m).convert_to<float>(1/255.f);

    // blur the image by convolving it with gaussian filter
    auto gray_blurred = 
        matrix_filter::cross_correlation(gray, matrix_filter::get_gaussian(5, 1));

    // get image derivatives approximation convolving it with solbel filters
    auto Gx = 
        matrix_filter::cross_correlation(gray_blurred, matrix_filter::get_sobel_x());
    auto Gy = 
        matrix_filter::cross_correlation(gray_blurred, matrix_filter::get_sobel_y());

    // get image gradient and gradient's direction
    Matrix<float> G(Gx.rows(), Gx.cols(), 1);
    Matrix<float> theta(Gx.rows(), Gx.cols(), 1);
    matrix_filter::magnitude_angle(Gx, Gy, G, theta);

    // this out edges of the image perfomring non-maximum suppression
    auto nmsup = matrix_filter::non_maximum_suppression(G, theta);

    nmsup.convert_to_abs<uchar>(255).write("out.jpg");

    return 0;
}




































    /*
    std::cout << std::setprecision(2);
    for (int i = 0; i != 100; ++i) {
        for (int j = 0 ; j != 100; ++j) {
            std::cout << Gx(i, j) << ' ';
        }
        std::cout << '\n';
    }
    */
