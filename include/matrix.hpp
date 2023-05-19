#pragma once

#include <cstdlib>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

template <typename scalar_t>
class Matrix {
public:
    Matrix() = default;
    Matrix(unsigned rows, unsigned cols, unsigned channels = 1);
    Matrix(char* filename);
    ~Matrix();

    Matrix(const Matrix& src);
    Matrix& operator=(const Matrix& src);
    scalar_t* ptr(int pos);
    const scalar_t* ptr(int pos) const;

    bool read(const char* filename, unsigned channels = 0);
    bool write(const char* filename);

    scalar_t& operator()(unsigned row, unsigned col, unsigned channel = 0);
    scalar_t operator()(unsigned row, unsigned col, unsigned channel = 0) const;
    //TODO: arithmetic operators overloads

    unsigned size() const;
    unsigned rows() const;
    unsigned cols() const;
    unsigned channels() const;
    bool empty() const;

private:
    void copy_dimensions(const Matrix<scalar_t>& src);
    bool allocate(unsigned size);
    bool reallocate(unsigned size);

private:
    scalar_t* data_ = nullptr;
    unsigned rows_ = 0;
    unsigned cols_ = 0;
    unsigned channels_ = 0;
};


template <typename scalar_t>
Matrix<scalar_t>::Matrix(unsigned rows, unsigned cols, unsigned channels) :
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

template <typename scalar_t>
Matrix<scalar_t>::Matrix(char* filename) {
    read(filename); 
}

template <typename scalar_t>
Matrix<scalar_t>::~Matrix() {
    free(data_);
}

template <typename scalar_t>
Matrix<scalar_t>::Matrix(const Matrix<scalar_t>& src) {
    copy_dimensions(src);
    std::copy(data_, data_ + size(), src.data_);
}

template <typename scalar_t>
Matrix<scalar_t>& Matrix<scalar_t>::operator=(const Matrix<scalar_t>& src) {
    copy_dimensions(src);
    reallocate(size());
    return *this;
}

// If Matrix type is not unsinged char 
// !!! create's a temporary Matrix<unsigned char>!!!
template <typename scalar_t>
bool Matrix<scalar_t>::read(const char* filename, unsigned channels) {     
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
template <typename scalar_t>
bool Matrix<scalar_t>::write(const char* filename) {
    int success = 0;
    success = stbi_write_jpg(
            filename, 
            static_cast<int>(cols_), 
            static_cast<int>(rows_),
            static_cast<int>(channels_), 
            data_, 100);

    return success;
}

template <typename scalar_t>
scalar_t& Matrix<scalar_t>::operator()(unsigned row, unsigned col, unsigned channel) {
    return data_[row*cols_*channels_ + col*channels_ + channel];
}

template <typename scalar_t>
scalar_t Matrix<scalar_t>::operator()(unsigned row, unsigned col, unsigned channel) const {
    return data_[row*cols_*channels_ + col*channels_ + channel];
}

template <typename scalar_t>
scalar_t* Matrix<scalar_t>::ptr(int pos) {
    return data_ + pos; 
}

template <typename scalar_t>
const scalar_t* Matrix<scalar_t>::ptr(int pos) const {
    return data_ + pos; 
}

template <typename scalar_t>
unsigned Matrix<scalar_t>::size() const {
    return rows_ * cols_ * channels_;
}
template <typename scalar_t>
unsigned Matrix<scalar_t>::rows() const {
    return rows_;
}
template <typename scalar_t>
unsigned Matrix<scalar_t>::cols() const {
    return cols_;
}
template <typename scalar_t>
unsigned Matrix<scalar_t>::channels() const {
    return channels_;
}

template <typename scalar_t>
bool Matrix<scalar_t>::empty() const {
    return data_ == nullptr;
}

template <typename scalar_t>
void Matrix<scalar_t>::copy_dimensions(const Matrix<scalar_t>& src) {
    rows_ = src.rows_;
    cols_ = src.cols_;
    channels_ = src.channels_;
}

template <typename scalar_t>
bool Matrix<scalar_t>::allocate(unsigned size) {
    data_ = (scalar_t*) malloc(sizeof(scalar_t) * size); 
    return data_;
}

template <typename scalar_t>
bool Matrix<scalar_t>::reallocate(unsigned size) {
    data_ = (scalar_t*) realloc(data_, sizeof(scalar_t) * size);
    return data_;
}

