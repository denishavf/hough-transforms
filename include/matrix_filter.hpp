#include <cassert>

#include "matrix.hpp"
#include "utils.hpp"

namespace matrix_filter {
    template <typename src_scalar_t, typename dst_scalar_t>
    void convert_to(
            const Matrix<src_scalar_t>& src,
            Matrix<dst_scalar_t>& dst,
            float alpha = 1,// dst_scalar_t alpha = 1 // pretty shure this is redundant
            float beta = 0) // dst_scalar_t beta = 0) 
    {
        assert(src.cols() == dst.cols() 
               and src.rows() == dst.rows()
               and src.channels() == dst.channels()
        );

        auto size = src.size();

        const auto s_ptr = src.ptr(0);   
        auto d_ptr = dst.ptr(0);   

        for (unsigned i = 0; i != size; ++i) {
            d_ptr[i] = 
                saturate_cast<dst_scalar_t>(s_ptr[i] * alpha + beta);
        }
    }
}


