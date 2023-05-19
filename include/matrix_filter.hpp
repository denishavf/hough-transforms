#include <cassert>
#include <array>

#include "matrix.hpp"
#include "utils.hpp"

namespace matrix_filter {
    
    template <typename dst_scalar_t, typename src_scalar_t>
    Matrix<dst_scalar_t> convert_to(
            const Matrix<src_scalar_t>& src,
            float alpha = 1,// dst_scalar_t alpha = 1 // pretty shure this is redundant
            float beta = 0) // dst_scalar_t beta = 0) 
    {
        Matrix<dst_scalar_t> res(src.rows(), src.cols(), src.channels());

        auto size = src.size();

        const auto s_ptr = src.ptr(0);   
        auto d_ptr = res.ptr(0);   

        for (unsigned i = 0; i != size; ++i) {
            d_ptr[i] = 
                saturate_cast<dst_scalar_t>(s_ptr[i] * alpha + beta);
        }

        return res;
    }

    // Resulting matrix hase the same type as input matrix
    template <typename scalar_t, typename kernel_t>
    Matrix<scalar_t> cross_correlation(
            const Matrix<scalar_t>& src,
            const Matrix<kernel_t>& kernel)
    {
        assert(kernel.channels() == 1 and kernel.cols() == kernel.rows());

        Matrix<scalar_t> res(src.rows(), src.cols(), src.channels());
        
        auto kernel_len = kernel.cols();
        auto kernel_radius = kernel_len / 2;

        auto channels = src.channels();
        auto rows = src.rows();
        auto cols = src.cols();

        for (uint y = 0; y != rows; ++y) {
            for (uint x = 0; x != cols; ++x) {
            
                std::array<float, 4> sum{};
                for (uint ky = 0; ky != kernel_len; ++ky) {
                    for (uint kx = 0; kx != kernel_len; ++kx) {
                        
                        for (uint c = 0; c != channels; ++c) {
                            int yy = reflect(y - kernel_radius + ky, rows);
                            int xx = reflect(x - kernel_radius + kx, cols);

                            sum[c] += kernel(ky, kx) * src(yy, xx, c);
                        }
                    }
                }

                for (uint c = 0; c != channels; ++c) 
                    res(y, x, c) = saturate_cast<scalar_t>(sum[c]);
            }
        }

        return res;
    }

    template <typename scalar_t>
    Matrix<scalar_t> get_gaussian(int gauss_len, double sigma) {
        Matrix<scalar_t> gauss(gauss_len, gauss_len, 1);
        int gauss_radius = gauss_len / 2;

        scalar_t sum = 0;
        
        for (int x = -gauss_radius; x <= gauss_radius; ++x) {
            for (int y = -gauss_radius; y <= gauss_radius; ++y) {
                double exponent = -(x*x + y*y) / (2 * sigma * sigma);
                gauss(x + gauss_radius, y + gauss_radius) =
                    exp(exponent) / (2 * M_PI * sigma*sigma);
                sum += gauss(x + gauss_radius, y + gauss_radius);
            }
        }

        gauss.for_each([sum](scalar_t a) {return a / sum;});

        return gauss;
    }

}


