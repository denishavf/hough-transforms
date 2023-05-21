#include <iostream>
#include <iomanip>

#include <matrix.hpp>
#include <matrix_filter.hpp>

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "usage: " << argv[0] << " /path/to/image\n";
        return -1;
    }

    Matrix<unsigned char> m(argv[1]);
    if (m.empty()) {
        std::cerr << "failed to load image\n";
        return -1;
    }
    std::cout << "loaded with size: " << m.size() << '\n';

    // get grayscale of the image and convert it to float
    auto gray = matrix_filter::grayscale(m).convert_to<float>(1/255.f);

    // blur the image by convolving it with gaussian filter
    auto gray_blurred = 
        matrix_filter::cross_correlation(gray, matrix_filter::get_gaussian(5, 1.4));

    // get image derivatives approximation convolving it with solbel filters
    auto Gx = 
        matrix_filter::cross_correlation(gray_blurred, matrix_filter::get_sobel_x());
    auto Gy = 
        matrix_filter::cross_correlation(gray_blurred, matrix_filter::get_sobel_y());

    // get image gradient and gradient's direction
    Matrix<float> G(Gx.rows(), Gx.cols(), 1);
    Matrix<float> theta(Gx.rows(), Gx.cols(), 1);
    matrix_filter::magnitude_angle(Gx, Gy, G, theta);

    // thin out edges of the image by perfomring non-maximum suppression
    auto nmsup = matrix_filter::non_maximum_suppression(G, theta);

    // appty double treshold to suppressed edge image
    auto max_el = std::max_element(nmsup.ptr(0), nmsup.ptr(0) + nmsup.size());
    auto high = *max_el * 0.1; 
    auto low = high * 0.05; 
    matrix_filter::double_treshold(nmsup, low, high);

    // convert image to bynary image based on treshold hysteresis
    matrix_filter::hysteresis(nmsup);

    // converet image to uchar image and save it
    auto edges = nmsup.convert_to<uchar>(255);
    edges.write("edges.jpg");
    std::cout << "saved to edges.jpg\n";

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
