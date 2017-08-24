//
//  main.cpp
//  word2vec
//
//  Created by 陈秉烜 on 2017/8/22.
//  Copyright © 2017年 陈秉烜. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <time.h>
#include "counter.hpp"
#include "vector.hpp"
#include "word2vec.hpp"
#include "settings.h"

using namespace std;


vector<string> BuildVocabulary(char* filePath) {
    
    ifstream file(filePath);
    Counter<string> counter;
    
    string word;
    if (!file.is_open()) {
        cout << "Can't open file " << filePath << endl;
        return vector<string>();
    }
    while(!file.eof()) {
        file >> word;
        counter.add(word);
    }
    Pair<string>** sorted = counter.sort();
    unsigned long size = counter.size();
    for (Pair<string>** ptr = sorted + size - 1; (*ptr)->count < 5; ptr--) {
        size--;
    }
    vector<string> vocabulary;
    for (unsigned long i = 0; i < size; i ++) {
        Pair<string>* pair = sorted[i];
        vocabulary.push_back(pair->item);
        //cout << pair->item << ' ' << pair->count << endl;
    }
    delete sorted;
    return vocabulary;
}

vector<string> GetVocabulary(char* filePath) {
    vector<string> vocabulary;
    
    string savePath(filePath);
    savePath += ".vocab";
    ifstream saveFile(savePath);
    if (saveFile.is_open()) {
        string word;
        while (!saveFile.eof()) {
            saveFile >> word;
            vocabulary.push_back(word);
        }
        saveFile.close();
    } else {
        vocabulary = BuildVocabulary(filePath);
        ofstream saveFile2(savePath);
        if (saveFile2.is_open()) {
            for (unsigned long i = 0; i < vocabulary.size(); i ++) {
                saveFile2 << vocabulary[i] << ' ';
            }
            saveFile2.close();
        } else {
            cout << "Can't write to file " << savePath << endl;
        }
    }
    return vocabulary;
}

map<string, unsigned long> BuildInverseDict(const vector<string>& vocabulary) {
    map<string, unsigned long> result;
    result["UNK"] = 0;
    for (unsigned long i = 0; i < vocabulary.size(); i++) {
        result[vocabulary[i]] = i + 1;
    }
    return result;
}

vector<unsigned long> BuildCorpus(const char* filePath, const vector<string> &vocabulary) {
    map<string, unsigned long> inverseDict = BuildInverseDict(vocabulary);
    ifstream corpusFile(filePath);
    if (!corpusFile.is_open()) {
        cout << "Can't open file " << filePath << endl;
        return vector<unsigned long>();
    }
    vector<unsigned long> result;
    while (!corpusFile.eof()) {
        string word;
        corpusFile >> word;
        map<string, unsigned long>::iterator iter = inverseDict.find(word);
        unsigned long wordId = iter == inverseDict.end() ? 0 : iter->second;
        result.push_back(wordId);
    }
    corpusFile.close();
    return result;
}

vector<unsigned long> GetCorpus(const char* filePath, const vector<string> &vocabulary) {
    string savePath(filePath);
    savePath += ".corpus";
    ifstream ifile(savePath);
    vector<unsigned long> result;
    if (ifile.is_open()) {
        while (!ifile.eof()) {
            unsigned long wordId;
            ifile >> wordId;
            result.push_back(wordId);
        }
        ifile.close();
    } else {
        result = BuildCorpus(filePath, vocabulary);
        ofstream ofile(savePath);
        if (!ofile.is_open()) {
            cout << "Can't write to corpus file " << savePath << endl;
        } else {
            for (vector<unsigned long>::iterator iter = result.begin(); iter != result.end(); iter++) {
                ofile << *iter << ' ';
            }
            ofile.close();
        }
    }
    return result;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    //L = 4;
    std::cout << "Word2Vec\n";
    string path("text8");
    char *p = (char*)path.data();
    vector<string> vocabulary = GetVocabulary(p);
    vector<unsigned long> corpus = GetCorpus(p, vocabulary);
    unsigned long maxIndex = vocabulary.size() + 1;
    Word2Vec w2v = Word2Vec(maxIndex, WINDOW_RADIUS, NEG_NUM);
    size_t size = corpus.size();
    clock_t t1 = clock();
    double sumLoss = 0.0, loss;
    const unsigned long N = 1000;
    unsigned long pos_w, pos_c, neg_w, neg_c;
    default_random_engine generator;
    uniform_int_distribution<unsigned long> distribution(1, maxIndex);
    for (unsigned long long i = WINDOW_RADIUS; i < size - WINDOW_RADIUS; i++) {
        unsigned long word = corpus[i];
        for (int j = -WINDOW_RADIUS; j < WINDOW_RADIUS; j++) {
            if (j == 0) continue;
            pos_w = word;
            pos_c = corpus[j < 0 ? i + j : i + j + 1];
            loss = w2v.update(pos_w, pos_c, LR, false);
            sumLoss += loss;
            for (int k = 0; k < NEG_NUM; k++) {
                neg_w = distribution(generator);
                neg_c = distribution(generator);
                loss = w2v.update(neg_w, neg_c, LR, true);
                sumLoss += loss;
            }
        }
        if (i % N == 0) {
            clock_t t2 = clock();
            double delta_t = (double)(t2 - t1) / CLOCKS_PER_SEC;
            t1 = t2;
            double meanLoss = sumLoss / (double)(N * (NEG_NUM + 1));
            double pct = i * 100.0 / (size - WINDOW_RADIUS);
            cout << "Loss: " << meanLoss << "\trps: " << N / delta_t;
            cout << "\tTime: " << delta_t << "seconds elapsed\tProgress: " << pct << '%' << endl;
            sumLoss = 0;
        }
    delete pos_w, pos_c, neg_w, neg_c;
    w2v.save(path + ".w2v", vocabulary);
    return 0;
}
