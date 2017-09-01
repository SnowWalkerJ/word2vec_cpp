//
//  word2vec.cpp
//  word2vec
//
//  Created by 陈秉烜 on 2017/8/22.
//  Copyright © 2017年 陈秉烜. All rights reserved.
//

#include "word2vec.hpp"
#include "expTable.hpp"
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>


double* table = buildExpTable();

double sigmoid(double x) {
    double ex = exp(x);
    return ex / (1 + ex);
//    return lookupExpTable(table, x);
}

Word2Vec::Word2Vec(unsigned long numObjects, unsigned int windowRadius, unsigned int negSize):numObjects(numObjects), windowRadius(windowRadius), negSize(negSize) {
    embIn = new Vector<EMBEDDING_SIZE>[numObjects];
    embOut = new Vector<EMBEDDING_SIZE>[numObjects];
    grad = Vector<EMBEDDING_SIZE>();
    for (int i = 0; i < numObjects; i++) {
        embIn[i] = Vector<EMBEDDING_SIZE>();
        embOut[i] = Vector<EMBEDDING_SIZE>();
        for (int j = 0; j < EMBEDDING_SIZE; j++) {
            embIn[i].set(j, ((double)random() / RAND_MAX - 0.5) * 0.001);
            embOut[i].set(j, 0.0);
        }
    }
}

void Word2Vec::applyGrad(unsigned long idx, Vector<EMBEDDING_SIZE> grad) {
    embIn[idx] -= grad;
}

double Word2Vec::update(unsigned long w, unsigned long c, double lr, bool isNegative, Vector<EMBEDDING_SIZE> *grad) {
    Vector<EMBEDDING_SIZE> u = embIn[w];
    Vector<EMBEDDING_SIZE> v = embOut[c];
    double sign = isNegative ? -1.0 : 1.0;
    double logit = u.dot(v * sign);
    double probability = sigmoid(logit);
    double grad0 = - sign * (1 - probability) * lr;
    double loss = -log(probability);
    Vector<EMBEDDING_SIZE> grad_v = u * grad0;
    Vector<EMBEDDING_SIZE> grad_u = v * grad0;
    embOut[c] -= grad_v;
    //embIn[w] -= grad_u;
    *grad += grad_u;
    return loss;
}

void Word2Vec::save(string savePath, const vector<string> &vocabulary) {
    ofstream oFile(savePath);
    if (!oFile.is_open()) {
        cout << "Can't write result vectors to file " << savePath << endl;
        return;
    }
    for (unsigned long i = 0; i < numObjects; i++) {
        string word = i == 0 ? "UNK" : vocabulary[i-1];
        oFile << word << ' ';
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

