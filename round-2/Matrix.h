#pragma once
#include <algorithm>
#include <stdexcept>


template<class T>
class Matrix {
public:
    typedef T               value_type;
    typedef T*              pointer;
    typedef const T*        const_pointer;
    typedef T&              reference;
    typedef const T&        const_reference;
    typedef T*              iterator;
    typedef const T*        const_iterator;
    typedef std::size_t     size_type;
    typedef std::ptrdiff_t  difference_type;
public:
    Matrix();
    Matrix(const Matrix& o);
    Matrix(size_type rows, size_type cols, const T& init_value = T());

    Matrix& operator=(const Matrix& o);

    virtual ~Matrix();

    bool operator==(const Matrix& o) const;
    bool operator!=(const Matrix& o) const;

    reference at(size_type r, size_type c);
    const_reference at(size_type r, size_type c) const;

    reference operator()(size_type r, size_type c);
    const_reference operator()(size_type r, size_type c) const;

    void set_if_inbounds(size_type r, size_type c, const value_type& val);

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    size_type rows() const;
    size_type cols() const;

    void swap(Matrix<T>& rhs);
protected:
    size_type rows_;
    size_type cols_;
    T *data_;
};

template<class T>
Matrix<T>::Matrix() : rows_(0), cols_(0), data_(0) {}

template<class T>
Matrix<T>::Matrix(const Matrix<T>& o) : rows_(o.rows_), cols_(o.cols_) {
    if ( o.data_ ) {
        data_ = new T[rows_ * cols_];
        std::copy( o.begin(), o.end(), begin() );
    } else {
        data_ = 0;
    }
}

template<class T>
Matrix<T>::Matrix(size_type rows, size_type cols, const T& init_value) :
    rows_(rows),
    cols_(cols) {
    if ( rows_ == 0 || cols_ == 0 ) {
        data_ = 0;
    } else {
        data_ = new T[rows_ * cols_];
        std::fill(data_, data_ + (rows_*cols_), init_value);
    }
}

template<class T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& o) {
    if ( this == &o ) {
        return *this;
    }

    delete[] data_;

    rows_ = o.rows_;
    cols_ = o.cols_;
    if ( o.data_ ) {
        data_ = new T[rows_ * cols_];
        std::copy( o.begin(), o.end(), begin() );
    } else {
        data_ = 0;
    }
    return *this;
}

template<class T>
Matrix<T>::~Matrix() {
    delete[] data_;
}

template<class T> inline
bool Matrix<T>::operator==(const Matrix<T>& o) const {
    return rows_ == o.rows_ && cols_ == o.cols && std::equal( data_, data_ + rows_*cols_, o.data_);
}

template<class T> inline
bool Matrix<T>::operator!=(const Matrix<T>& o) const {
    return !(*this == o);
}

template<class T> inline
typename Matrix<T>::reference Matrix<T>::at(size_type r, size_type c) {
    if ( r >= rows_ || c >= cols_ ) {
        throw std::out_of_range("Matrix : Out of range");

    }
    return this->operator()(r,c);
}

template<class T> inline
typename Matrix<T>::const_reference Matrix<T>::at(size_type r, size_type c) const {
    if ( r >= rows_ || c >= cols_ ) {
        throw std::out_of_range("Matrix : Out of range");
    }
    return this->operator()(r,c);
}

template<class T> inline
typename Matrix<T>::reference Matrix<T>::operator()(size_type r, size_type c) {
    return data_[c*rows_+r];
}

template<class T> inline
typename Matrix<T>::const_reference Matrix<T>::operator()(size_type r, size_type c) const {
    return data_[c*rows_+r];
}

template<class T> inline
void Matrix<T>::set_if_inbounds(size_type r, size_type c, const value_type& val) {
    if ( r < rows_ && c < cols_ ) {
        this->operator()(r, c) = val;
    } else {
        //std::cout << r << ", " << c << std::endl;
    }
}

template<class T> inline
typename Matrix<T>::iterator Matrix<T>::begin() {
    return data_;
}

template<class T> inline
typename Matrix<T>::iterator Matrix<T>::end() {
    return data_ + (rows_*cols_);
}

template<class T> inline
typename Matrix<T>::const_iterator Matrix<T>::begin() const {
    return data_;
}

template<class T> inline
typename Matrix<T>::const_iterator Matrix<T>::end() const {
    return data_ + (rows_*cols_);
}

template<class T> inline
typename Matrix<T>::size_type Matrix<T>::rows() const {
    return rows_;
}

template<class T> inline
typename Matrix<T>::size_type Matrix<T>::cols() const {
    return cols_;
}

template<class T> inline
void Matrix<T>::swap(Matrix<T>& rhs) {
    std::swap(rows_, rhs.rows_);
    std::swap(cols_, rhs.cols_);
    std::swap(data_, rhs.data_);
}
