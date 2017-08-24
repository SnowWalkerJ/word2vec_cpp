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
        cout << "Can't open file " << filePath;
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
    string path("/Users/snowwalkerj/Downloads/text8");
    char *p = (char*)path.data();
    vector<string> vocabulary = GetVocabulary(p);
    vector<unsigned long> corpus = GetCorpus(p, vocabulary);
    unsigned long maxIndex = vocabulary.size() + 1;
    Word2Vec w2v = Word2Vec(maxIndex, WINDOW_RADIUS, NEG_NUM);
    size_t size = corpus.size();
    clock_t t1 = clock();
    double sumLoss = 0.0;
    for (unsigned long long i = WINDOW_RADIUS; i < size - WINDOW_RADIUS; i++) {
        unsigned long word = corpus[i];
        vector<unsigned long> context, negative;
        for (int j = -WINDOW_RADIUS; j < WINDOW_RADIUS; j++) {
            if (j == 0) continue;
            context.push_back(j < 0 ? corpus[i+j] : corpus[i+j+1]);
        }
        default_random_engine generator;
        uniform_int_distribution<unsigned long> distribution(1, maxIndex);
        for (unsigned int j = 0; j < NEG_NUM; j++) {
            negative.push_back(distribution(generator));
        }
        double loss = w2v.update(word, context, negative, 0.025);
        sumLoss += loss;
        if (i % 10000 == 0) {
            clock_t t2 = clock();
            double delta_t = (double)(t2 - t1) / CLOCKS_PER_SEC;
            t1 = t2;
            double meanLoss = sumLoss / 10000.0;
            double pct = i * 100.0 / (size - WINDOW_RADIUS);
            cout << "Loss: " << meanLoss << "\trpm: " << 1000.0 * 60.0 / delta_t;
            cout << "\tTime: " << delta_t << "seconds elapsed\tProgress: " << pct << '%' << endl;
            sumLoss = 0;
        }
    }
    w2v.save(path + ".w2v", vocabulary);
    return 0;
}
