#include <iostream>

#include <matrix.hpp>
#include "matrix_filter.hpp"

int main() {
    // Load Image
    std::string file_in = "in_lena.png";

    Matrix<unsigned char> m(file_in.data());
    if (m.empty()) {
        std::cerr << "image is empty\n";
        return -1;
    }
    std::cout << "loaded with size: " << m.size() << '\n';

    Matrix<float> kernel(3, 3, 1);
    {
        auto p = kernel.ptr(0);
        p[0] = 1.f/16; p[1] = 2.f/16; p[2] = 1.f/16;
        p[3] = 2.f/16; p[4] = 4.f/16; p[5] = 2.f/16;
        p[6] = 1.f/16; p[7] = 2.f/16; p[8] = 1.f/16;
    }

    auto res = matrix_filter::cross_correlation(m, kernel);
    res.write("out.jpg");
    
    return 0;
}

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
