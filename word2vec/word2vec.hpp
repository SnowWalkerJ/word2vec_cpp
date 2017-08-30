//
//  word2vec.hpp
//  word2vec
//
//  Created by 陈秉烜 on 2017/8/22.
//  Copyright © 2017年 陈秉烜. All rights reserved.
//

#ifndef word2vec_hpp
#define word2vec_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "vector.hpp"
#include "settings.h"

using namespace std;

class Word2Vec {
private:
    Vector<EMBEDDING_SIZE> grad;
    const unsigned windowRadius, negSize;
    const unsigned long numObjects;
    Vector<EMBEDDING_SIZE>* embIn;
    Vector<EMBEDDING_SIZE>* embOut;
    
public:
    Word2Vec(unsigned long, unsigned int, unsigned int);
    void applyGrad(unsigned long, Vector<EMBEDDING_SIZE>);
    double update(unsigned long, unsigned long, double, bool, Vector<EMBEDDING_SIZE>*);
    void save(string, const vector<string>&);
};

#endif /* word2vec_hpp */
