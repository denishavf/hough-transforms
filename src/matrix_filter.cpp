#include "matrix_filter.hpp"

Matrix<uchar> matrix_filter::grayscale(Matrix<uchar>& src) {
    Matrix<uchar> res(src.rows(), src.cols(), 1);

    auto size = src.size();

    auto src_p = src.ptr(0);
    auto res_p = res.ptr(0);
    for (size_t i = 0, j = 0; i < size; i += 3, ++j) {
        uchar gray_val =
            (.2126 * src_p[i] + .7152 * src_p[i+1] + .0722 * src_p[i+2]);
        res_p[j] = gray_val;     
    }

    return res;
}

Matrix<float> matrix_filter::get_gaussian(int gauss_len, double sigma) {
    Matrix<float> gauss(gauss_len, gauss_len, 1);
    int gauss_radius = gauss_len / 2;

    float sum = 0;
    
    for (int x = -gauss_radius; x <= gauss_radius; ++x) {
        for (int y = -gauss_radius; y <= gauss_radius; ++y) {
            double exponent = -(x*x + y*y) / (2 * sigma * sigma);
            gauss(x + gauss_radius, y + gauss_radius) =
                std::exp(exponent) / (2 * M_PI * sigma*sigma);
            sum += gauss(x + gauss_radius, y + gauss_radius);
        }
    }

    gauss /= sum;

    return gauss;
}

Matrix<float> matrix_filter::get_sobel_x() {
    Matrix<float> sobel_x(3, 3, 1);

    auto p = sobel_x.ptr(0);
    p[0] = -1.f; p[1] = 0.f; p[2] = 1.f;
    p[3] = -2.f; p[4] = 0.f; p[5] = 2.f;
    p[6] = -1.f; p[7] = 0.f; p[8] = 1.f;

    return sobel_x;
}

Matrix<float> matrix_filter::get_sobel_y() {
    Matrix<float> sobel_y(3, 3, 1);

    auto p = sobel_y.ptr(0);
    p[0] = -1.f; p[1] = -2.f; p[2] = -1.f;
    p[3] = 0.f; p[4] = 0.f; p[5] = 0.f;
    p[6] = 1.f; p[7] = 2.f; p[8] = 1.f;

    return sobel_y;
}

Matrix<float> matrix_filter::cross_correlation(
        const Matrix<float>& src, const Matrix<float>& kernel)
{
    assert(
            src.channels() == 1 
            and kernel.channels() == 1 
            and kernel.cols() == kernel.rows());

    Matrix<float> res(src.rows(), src.cols(), src.channels());
    
    auto kernel_len = kernel.cols();
    auto kernel_radius = kernel_len / 2;

    auto rows = src.rows();
    auto cols = src.cols();

    for (uint y = 0; y != rows; ++y) {
        for (uint x = 0; x != cols; ++x) {
       
            float sum = 0;
            for (uint ky = 0; ky != kernel_len; ++ky) {
                for (uint kx = 0; kx != kernel_len; ++kx) {
                    
                    int yy = reflect(y - kernel_radius + ky, rows);
                    int xx = reflect(x - kernel_radius + kx, cols);

                    sum += kernel(ky, kx) * src(yy, xx);
                }
            }
            
            if (sum > 180)
                std::cout << sum << ' ';
            res(y, x) = sum;//saturate_cast<float>(sum);
        }
    }

    return res;
}

Matrix<uchar> matrix_filter::cross_correlation_3c(
        const Matrix<uchar>& src, const Matrix<float>& kernel)
{
    assert(kernel.channels() == 1 and kernel.cols() == kernel.rows());

    Matrix<uchar> res(src.rows(), src.cols(), src.channels());
    
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
                res(y, x, c) = saturate_cast<uchar>(sum[c]);
        }
    }

    return res;
}

void matrix_filter::magnitude_angle(
        Matrix<float>& Gx, Matrix<float>& Gy, 
        Matrix<float>& dst_magnitude, Matrix<float>& dst_angle)
{
    auto Gx_p = Gx.ptr(0);  
    auto Gy_p = Gy.ptr(0);
    auto magn_p = dst_magnitude.ptr(0);
    auto angle_p = dst_angle.ptr(0);

    auto size = Gx.size();
    
    for (size_t i = 0; i != size; ++i) {
        auto x = Gx_p[i];
        auto y = Gy_p[i];
        magn_p[i] = 
                std::sqrt(x*x + (y)*y);
        angle_p[i] = std::atan2(y, x);
    }
}

Matrix<float> matrix_filter::non_maximum_suppression(
        Matrix<float>& magnitude,
        Matrix<float>& angle) 
{
    assert(
            magnitude.cols() == angle.cols() 
            and magnitude.rows() == angle.rows()
            and magnitude.channels() == 1);

    float maxima = 1;

    auto rows = magnitude.rows();
    auto cols = magnitude.cols();

    Matrix<float> res(rows, cols);

    for (size_t i = 1; i < rows - 1; ++i) {
        for (size_t j = 1; j < cols - 1; ++j) {
            auto angle_deg = abs(angle(i, j) * (180.0 / M_PI));

            auto q = maxima, r = maxima;

            if ((0 <= angle_deg && angle_deg < 22.5) || (157.5 <= angle_deg && angle_deg <= 180)) {
                q = magnitude(i, j + 1);
                r = magnitude(i, j - 1);
            } else if (22.5 <= angle_deg && angle_deg < 67.5) {
                q = magnitude(i + 1, j - 1);
                r = magnitude(i - 1, j + 1);
            } else if (67.5 <= angle_deg && angle_deg < 112.5) {
                q = magnitude(i + 1, j);
                r = magnitude(i - 1, j);
            } else if (112.5 <= angle_deg && angle_deg < 157.5) {
                q = magnitude(i - 1, j - 1);
                r = magnitude(i + 1, j + 1);
            }
            
            auto in_val = magnitude(i, j);
            if (in_val >= q and in_val >= r)
                res(i, j) = in_val;
            else
                res(i, j) = 0;
        }
    }

    return res;
}

void matrix_filter::double_treshold(
        Matrix<uchar>& m,
        uchar low, uchar high,
        uchar weak, uchar strong) 
{           
    auto* p = m.ptr(0);
    size_t sz = m.size();

    for (size_t i = 0; i != sz; ++i) {
        if (p[i] >= high) {
            p[i] = strong;
        } else if (p[i] >= low) {
            p[i] = weak;
        } else {
            p[i] = 0;
        }
    }
}

void matrix_filter::double_treshold(
        Matrix<float>& m,
        float low, float high,
        float weak, float strong) 
{           
    auto* p = m.ptr(0);
    size_t sz = m.size();

    for (size_t i = 0; i != sz; ++i) {
        auto val = abs(p[i]);
        if (val >= high) {
            p[i] = strong;
        } else if (val >= low) {
            p[i] = weak;
        } else {
            p[i] = 0;
        }
    }
}

void matrix_filter::hysteresis(
        Matrix<float>& m,
        float weak, float strong) 
{
    auto rows = m.rows();
    auto cols = m.cols();

    const int dy[8] = {0, 1, 0, -1, -1, 1, -1, 1};    
    const int dx[8] = {1, 0, -1, 0, 1, 1, -1, -1};

    // potentionay leaws weak pixels at the borders
    for (size_t y = 1; y < rows; ++y) {
        for (size_t x = 1; x < cols; ++x) {
            if (m(y, x) == weak) {

                bool is_strong = false;
                for (int i = 0; i < 8; ++i) {
                    auto yy = y + dy[i];
                    auto xx = x + dx[i];
                    if (m(yy, xx) == strong) {
                        is_strong = true;
                        break;
                    }
                }

                if (is_strong) {
                    m(y, x) = strong;
                } else {
                    m(y, x) = 0;
                }

            }
        }
    }
}

