//
//  vector.hpp
//  word2vec
//
//  Created by 陈秉烜 on 2017/8/22.
//  Copyright © 2017年 陈秉烜. All rights reserved.
//

#ifndef vector_hpp
#define vector_hpp

#include <stdio.h>


template <int L>
class Vector {
private:
    double data[L];
    
public:
    Vector(){};
    Vector(double* initValues) {
        for (int i = 0; i < L; i++) {
            data[i] = initValues[i];
        }
    }
    Vector<L> operator+(const Vector<L> other) {
        Vector<L>  result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] + other.data[i];
        }
        return result;
    }
    Vector<L> operator-(const Vector<L> other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] - other.data[i];
        }
        return result;
    }
    Vector<L> operator*(const Vector<L> other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] * other.data[i];
        }
        return result;
    }
    Vector<L> operator/(const Vector<L> other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0; i < L; i++) {
            result.data[i] = this->data[i] / other.data[i];
        }
        return result;
    }
    Vector<L> operator+(const double other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0; i < L; i++) {
            result.data[i] = this->data[i] + other;
        }
        return result;
    }

    Vector<L> operator-(const double other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] - other;
        }
        return result;
    }
    Vector<L> operator*(const double other) {
        Vector<L>  result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] * other;
        }
        return result;
    }
    Vector<L> operator/(const double other) {
        Vector<L>  result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] / other;
        }
        return result;
    }
    Vector<L> operator+=(const Vector<L> other) {
        for (int i = 0 ; i < L; i++) {
            this->data[i] += other.data[i];
        }
        return *this;
    }
    Vector<L> operator-=(const Vector<L> other) {
        for (int i = 0 ; i < L; i++) {
            this->data[i] -= other.data[i];
        }
        return *this;
    }Vector<L> operator*=(const Vector<L> other) {
        for (int i = 0 ; i < L; i++) {
            this->data[i] *= other.data[i];
        }
        return *this;
    }Vector<L> operator/=(const Vector<L> other) {
        for (int i = 0 ; i < L; i++) {
            this->data[i] /= other.data[i];
        }
        return *this;
    }
    
    Vector<L> operator+=(const double other) {
        for (int i = 0; i < L; i++) {
            this->data[i] += other;
        }
        return *this;
    }
    Vector<L> operator-=(const double other) {
        for (int i = 0; i < L; i++) {
            this->data[i] -= other;
        }
        return *this;
    } Vector<L> operator*=(const double other) {
        for (int i = 0; i < L; i++) {
            this->data[i] *= other;
        }
        return *this;
    } Vector<L> operator/=(const double other) {
        for (int i = 0; i < L; i++) {
            this->data[i] /= other;
        }
        return *this;
    }
    double operator[](const int i) {
        return this->data[i];
    }
    Vector<L> Abs() {
        Vector<L>  result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] > 0 ? this->data[i] : -this->data[i];
        }
        return result;
    }
    double Sum() {
        double result = 0.0;
        for (int i = 0; i < L; ++i) {
            result += data[i];
        }
        return result;
    }
    double Mean() {
        return Sum() / Length();
    }
    double dot(const Vector<L> other) {
        double result = 0.0;
        for (int i = 0; i < L; i++) {
            result += data[i] * other.data[i];
        }
        return result;
    }
    inline int Length() {
        return L;
    }
    double set(int index, double value){
        data[index] = value;
        return value;
    }
};

#endif /* vector_hpp */
