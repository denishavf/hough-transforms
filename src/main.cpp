#include <iostream>

#include <matrix.hpp>

int main() {
    std::string file_in, file_out;
    std::cout << "input img name: ";
    std::cin >> file_in;

    Matrix<unsigned char> m(file_in.data());
    if (m.empty()) {
        std::cerr << "image is empty\n";
        return -1;
    }
    std::cout << "loaded with size: " << m.size() << '\n';


    for (int i = 100; i < 150; ++i) {
        for (int j = 100; j < 150; ++j) {
            for (int c = 0; c < 3; ++c) {
                m(i, j, c) = 255;
            }
        }
    }

    std::cout << "output img name: ";
    std::cin >> file_out;

    if (m.write(file_out.data())) {
        std::cout << "writen to " << file_out << '\n';
    } 
    else  {
        std::cerr << "image wasn't written\n"; 
    }

    return 0;
}
