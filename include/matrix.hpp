#pragma once

#include <cstdlib>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

template <typename skalar_t>
class Matrix {
public:
    Matrix() = default;
    Matrix(unsigned rows, unsigned cols, unsigned channels = 1);
    Matrix(char* filename);
    ~Matrix();

    Matrix(const Matrix& src);
    Matrix& operator=(const Matrix& src);

    bool read(const char* filename, unsigned channels = 0);
    bool write(const char* filename);

    skalar_t& operator()(unsigned row, unsigned col, unsigned channel = 0);
    skalar_t operator()(unsigned row, unsigned col, unsigned channel = 0) const;
    //TODO: arithmetic operators overloads

    unsigned size() const;
    unsigned rows() const;
    unsigned cols() const;
    unsigned channels() const;
    bool empty() const;

    template <typename new_skalar_t>
    Matrix<new_skalar_t> convert_to(new_skalar_t alpha, new_skalar_t beta) const;

private:
    void copy_dimensions(const Matrix<skalar_t>& src);
    bool allocate(unsigned size);
    bool reallocate(unsigned size);

private:
    skalar_t* data_ = nullptr;
    unsigned rows_ = 0;
    unsigned cols_ = 0;
    unsigned channels_ = 0;
};


template <typename skalar_t>
Matrix<skalar_t>::Matrix(unsigned rows, unsigned cols, unsigned channels) :
        rows_(rows),
        cols_(cols),
        channels_(channels) 
{
    allocate(size()); 
    if (!data_) {
        std::cerr << "Data is empty after malloc call\n";
        return;
    }
    std::fill(data_, data_ + size(), 0);
}

template <typename skalar_t>
Matrix<skalar_t>::Matrix(char* filename) {
    read(filename); 
}

template <typename skalar_t>
Matrix<skalar_t>::~Matrix() {
    free(data_);
}

template <typename skalar_t>
Matrix<skalar_t>::Matrix(const Matrix<skalar_t>& src) {
    copy_dimensions(src);
    std::copy(data_, data_ + size(), src.data_);
}

template <typename skalar_t>
Matrix<skalar_t>& Matrix<skalar_t>::operator=(const Matrix<skalar_t>& src) {
    copy_dimensions(src);
    reallocate(size());
    return *this;
}

// If Matrix type is not unsinged char 
// !!! create's a temporary Matrix<unsigned char>!!!
template <typename skalar_t>
bool Matrix<skalar_t>::read(const char* filename, unsigned channels) {     
    int h = 0, w = 0, c = 0;
    data_ = stbi_load(filename, &w, &h, &c, channels);

    rows_ = static_cast<unsigned>(h);
    cols_ = static_cast<unsigned>(w);
    channels_ = static_cast<unsigned>(c);

    if (!data_) {
        std::cerr << "image wasn't loaded" << '\n';    
    }

    return data_;
}

// If Matrix type is not unsinged char 
// !!! create's a temporary Matrix<unsigned char> for writing !!!
template <typename skalar_t>
bool Matrix<skalar_t>::write(const char* filename) {
    int success = 0;
    success = stbi_write_jpg(
            filename, 
            static_cast<int>(cols_), 
            static_cast<int>(rows_),
            static_cast<int>(channels_), 
            data_, 100);

    return success;
}

template <typename skalar_t>
skalar_t& Matrix<skalar_t>::operator()(unsigned row, unsigned col, unsigned channel) {
    return data_[row*cols_*channels_ + col*channels_ + channel];
}

template <typename skalar_t>
skalar_t Matrix<skalar_t>::operator()(unsigned row, unsigned col, unsigned channel) const {
    return data_[row*cols_*channels_ + col*channels_ + channel];
}

template <typename skalar_t>
unsigned Matrix<skalar_t>::size() const {
    return rows_ * cols_ * channels_;
}
template <typename skalar_t>
unsigned Matrix<skalar_t>::rows() const {
    return rows_;
}
template <typename skalar_t>
unsigned Matrix<skalar_t>::cols() const {
    return cols_;
}
template <typename skalar_t>
unsigned Matrix<skalar_t>::channels() const {
    return channels_;
}

template <typename skalar_t>
bool Matrix<skalar_t>::empty() const {
    return data_ == nullptr;
}

template <typename skalar_t>
void Matrix<skalar_t>::copy_dimensions(const Matrix<skalar_t>& src) {
    rows_ = src.rows_;
    cols_ = src.cols_;
    channels_ = src.channels_;
}

template <typename skalar_t>
bool Matrix<skalar_t>::allocate(unsigned size) {
    data_ = (skalar_t*) malloc(sizeof(skalar_t) * size); 
    return data_;
}

template <typename skalar_t>
bool Matrix<skalar_t>::reallocate(unsigned size) {
    data_ = (skalar_t*) realloc(data_, sizeof(skalar_t) * size);
    return data_;
}

