#include <iostream>

#include <matrix.hpp>
#include "matrix_filter.hpp"

void blur(Matrix<float>& m);

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
    blur(edges);

    auto edges_out = matrix_filter::convert_to<uchar>(edges, 255);
    edges_out.write("out.jpg");
    
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

void blur(Matrix<float>& m) {
    auto gauss = matrix_filter::get_gaussian<float>(11, 3);
    m = matrix_filter::cross_correlation(m, gauss);
}
