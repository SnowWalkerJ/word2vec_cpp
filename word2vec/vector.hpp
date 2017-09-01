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
#include <iostream>

using namespace std;


template <int L>
class Vector {
private:
    double data[L];
    
public:
    Vector(){}
    Vector(double x){
        for (int i = 0; i < L; i++) {
            data[i] = x;
        }
    }
    Vector(double* initValues) {
        for (int i = 0; i < L; i++) {
            data[i] = initValues[i];
        }
    }
    inline Vector<L> operator+(const Vector<L> other) {
        Vector<L>  result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] + other.data[i];
        }
        return result;
    }
    inline Vector<L> operator-(const Vector<L> other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] - other.data[i];
        }
        return result;
    }
    inline Vector<L> operator*(const Vector<L> other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] * other.data[i];
        }
        return result;
    }
    inline Vector<L> operator/(const Vector<L> other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0; i < L; i++) {
            result.data[i] = this->data[i] / other.data[i];
        }
        return result;
    }
    inline Vector<L> operator+(const double other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0; i < L; i++) {
            result.data[i] = this->data[i] + other;
        }
        return result;
    }

    inline Vector<L> operator-(const double other) {
        Vector<L> result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] - other;
        }
        return result;
    }
    inline Vector<L> operator*(const double other) {
        Vector<L>  result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] * other;
        }
        return result;
    }
    inline Vector<L> operator/(const double other) {
        Vector<L>  result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] / other;
        }
        return result;
    }
    inline Vector<L> operator+=(const Vector<L> other) {
        for (int i = 0 ; i < L; i++) {
            this->data[i] += other.data[i];
        }
        return *this;
    }
    inline Vector<L> operator-=(const Vector<L> other) {
        for (int i = 0 ; i < L; i++) {
            this->data[i] -= other.data[i];
        }
        return *this;
    }
    inline Vector<L> operator*=(const Vector<L> other) {
        for (int i = 0 ; i < L; i++) {
            this->data[i] *= other.data[i];
        }
        return *this;
    }
    inline Vector<L> operator/=(const Vector<L> other) {
        for (int i = 0 ; i < L; i++) {
            this->data[i] /= other.data[i];
        }
        return *this;
    }
    
    inline Vector<L> operator+=(const double other) {
        for (int i = 0; i < L; i++) {
            this->data[i] += other;
        }
        return *this;
    }
    inline Vector<L> operator-=(const double other) {
        for (int i = 0; i < L; i++) {
            this->data[i] -= other;
        }
        return *this;
    }
    inline Vector<L> operator*=(const double other) {
        for (int i = 0; i < L; i++) {
            this->data[i] *= other;
        }
        return *this;
    }
    inline Vector<L> operator/=(const double other) {
        for (int i = 0; i < L; i++) {
            this->data[i] /= other;
        }
        return *this;
    }
    inline double operator[](const int i) {
        return this->data[i];
    }
    inline Vector<L> Abs() {
        Vector<L>  result = Vector<L> ();
        for (int i = 0 ; i < L; i++) {
            result.data[i] = this->data[i] > 0 ? this->data[i] : -this->data[i];
        }
        return result;
    }
    inline double Sum() {
        double result = 0.0;
        for (int i = 0; i < L; ++i) {
            result += data[i];
        }
        return result;
    }
    inline double Mean() {
        return Sum() / Length();
    }
    inline double dot(const Vector<L> other) {
        double result = 0.0;
        for (int i = 0; i < L; i++) {
            result += data[i] * other.data[i];
        }
        return result;
    }
    inline int Length() {
        return L;
    }
    inline double set(int index, double value){
        data[index] = value;
        return value;
    }
    friend inline istream & operator >> (istream & inStream, Vector<L> & v) {
        for (unsigned int i = 0; i < L; i++ ) {
            inStream >> v.data[i];
        }
        return inStream;
    }
};

#endif /* vector_hpp */
