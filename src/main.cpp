#include <iostream>

#include <matrix.hpp>
#include "matrix_filter.hpp"

void write_image_cli(Matrix<unsigned char>& out_mat) {
    std::string file_out;
    std::cout << "output img name: ";
    std::cin >> file_out;

    if (out_mat.write(file_out.data())) {
        std::cout << "writen to " << file_out << '\n';
    } 
    else  {
        std::cerr << "image wasn't written\n"; 
    }
}


int main() {
    // Load Image
    std::string file_in = "in_lena.png";

    Matrix<unsigned char> m(file_in.data());
    if (m.empty()) {
        std::cerr << "image is empty\n";
        return -1;
    }
    std::cout << "loaded with size: " << m.size() << '\n';


    Matrix<float> float_m(m.rows(), m.cols(), m.channels());
    matrix_filter::convert_to(m, float_m, 1/255.f);
    Matrix<unsigned char> uchar_m(m.rows(), m.cols(), m.channels());
    //matrix_filter::convert_to(float_m, uchar_m, static_cast<unsigned char>(255));
    matrix_filter::convert_to(float_m, uchar_m, 255);
    uchar_m.write("out.jpg");

    /*
    for (int i = 100; i < 150; ++i) {
        for (int j = 100; j < 150; ++j) {
            for (int c = 0; c < 3; ++c) {
                std::cout << float_m(i, j, c) << ' ';
            }
            std::cout << ": :";
        }
        std::cout << "\n";
    }
    */
    return 0;
}
