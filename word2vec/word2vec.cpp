//
//  word2vec.cpp
//  word2vec
//
//  Created by 陈秉烜 on 2017/8/22.
//  Copyright © 2017年 陈秉烜. All rights reserved.
//

#include "word2vec.hpp"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>

#define GET_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

double sigmoid(double x) {
    double ex = exp(x);
    return ex / (1.0 + ex);
}

Word2Vec::Word2Vec(unsigned long numObjects, unsigned int windowRadius, unsigned int negSize):numObjects(numObjects), windowRadius(windowRadius), negSize(negSize) {
    embIn = new Vector<EMBEDDING_SIZE>[numObjects];
    embOut = new Vector<EMBEDDING_SIZE>[numObjects];
    for (int i = 0; i < numObjects; i++) {
        embIn[i] = Vector<EMBEDDING_SIZE>();
        embOut[i] = Vector<EMBEDDING_SIZE>();
        for (int j = 0; j < EMBEDDING_SIZE; j++) {
            embIn[i].set(j, ((double)random() / RAND_MAX - 0.5) * 0.001);
            embOut[i].set(j, 0.0);
        }
    }
}

double Word2Vec::update(unsigned long word, const vector<unsigned long> &context, const vector<unsigned long> &negative, double lr=0.01) {
    Vector<EMBEDDING_SIZE> u = embIn[word];
    Vector<EMBEDDING_SIZE> grad_u = Vector<EMBEDDING_SIZE>();
    double loss = 0.0;
    for (int i = 0; i < context.size(); i++) {
        unsigned long c = context[i];
        Vector<EMBEDDING_SIZE> v = embOut[c];
        double logit = u.dot(v);
        double probability = sigmoid(logit);
        double grad0 = -1.0 * (1 - probability) * lr;
        loss += -log(probability);
        Vector<EMBEDDING_SIZE> grad_v = u * grad0;
        embOut[c] -= grad_v;
        grad_u += v * grad0;
    }
    for (int i = 0; i < negative.size(); i++) {
        unsigned long c = negative[i];
        Vector<EMBEDDING_SIZE> v = embOut[c];
        double logit = u.dot(v);
        double probability = 1 - sigmoid(logit);
        double grad0 = 1.0 * (1 - probability) * lr;
        loss += -log(probability);
        Vector<EMBEDDING_SIZE> grad_v = u * grad0;
        embOut[c] -= grad_v;
        grad_u += v * grad0;
    }
    embIn[word] -= grad_u;
    return loss / (context.size() + negative.size());
}

void Word2Vec::save(string savePath, const vector<string> &vocabulary) {
    ofstream oFile(savePath);
    if (!oFile.is_open()) {
        cout << "Can't write result vectors to file " << savePath << endl;
        return;
    }
    for (unsigned long i = 0; i < numObjects; i++) {
        //string word = i == 0 ? "UNK" : vocabulary[i-1];
        //oFile << word << ' ';
        for (unsigned int j = 0; j < EMBEDDING_SIZE; j++) {
            oFile << embIn[i][j];
            if ( i < EMBEDDING_SIZE - 1) {
                oFile << ' ';
            } else {
                oFile << endl;
            }
        }
    }
    oFile.close();
}
