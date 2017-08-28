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
#include <math.h>
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

void train(){
    string path("text8");
    char *p = (char*)path.data();
    vector<string> vocabulary = GetVocabulary(p);
    vector<unsigned long> corpus = GetCorpus(p, vocabulary);
    unsigned long maxIndex = vocabulary.size() + 1;
    Word2Vec w2v = Word2Vec(maxIndex, WINDOW_RADIUS, NEG_NUM);
    size_t size = corpus.size();
    clock_t t1 = clock();
    double sumLoss = 0.0, loss;
    const unsigned long N = 100000;
    unsigned int numEpochs = 1;
    unsigned long pos_w, pos_c, neg_w, neg_c;
    default_random_engine generator;
    uniform_int_distribution<unsigned long> distribution(1, maxIndex);
    double lr = LR;
    for (unsigned int epoch = 0; epoch < numEpochs; epoch++) {
        for (unsigned long long i = WINDOW_RADIUS; i < size - WINDOW_RADIUS; i++) {
            unsigned long word = corpus[i];
            for (int j = -WINDOW_RADIUS; j < WINDOW_RADIUS; j++) {
                if (j == 0) continue;
                pos_w = word;
                pos_c = corpus[j < 0 ? i + j : i + j + 1];
                loss = w2v.update(pos_w, pos_c, lr, false);
                sumLoss += loss;
                for (int k = 0; k < NEG_NUM; k++) {
                    neg_w = distribution(generator);
                    neg_c = distribution(generator);
                    loss = w2v.update(neg_w, neg_c, lr, true);
                    sumLoss += loss;
                }
            }
            if (i % N == 0) {
                clock_t t2 = clock();
                double delta_t = (double)(t2 - t1) / CLOCKS_PER_SEC;
                t1 = t2;
                double meanLoss = sumLoss / (double)(N * (NEG_NUM + 1) * WINDOW_RADIUS * 2);
                double pct = ((double)i / (size - WINDOW_RADIUS) + epoch) / numEpochs * 100.0;
                lr = LR * pow(0.01, pct / 100);
                cout << "Loss: " << meanLoss << "\trps: " << N / delta_t;
                cout << "\tTime: " << delta_t << "seconds elapsed\tProgress: " << pct << '%' << endl;
                sumLoss = 0;
            }
        }
    }
    w2v.save(path + ".w2v", vocabulary);
}

map<string, Vector<EMBEDDING_SIZE>> load(string path) {
    map<string, Vector<EMBEDDING_SIZE>> data;
    ifstream iFile(path);
    if (!iFile.is_open()) {
        cout << "Can't open file " << path << endl;
        exit(1);
    }
    while (!iFile.eof()) {
        string word;
        Vector<EMBEDDING_SIZE> vec;
        iFile >> word;
        for (int i = 0; i < EMBEDDING_SIZE; i++) {
            double tmp;
            iFile >> tmp;
            vec.set(i, tmp);
        }
        data.insert(map<string, Vector<EMBEDDING_SIZE>>::value_type(word, vec));
    }
    return data;
}

string* nearest(map<string, Vector<EMBEDDING_SIZE>> mapping, string word, unsigned int n=10) {
    map<string, Vector<EMBEDDING_SIZE>>::iterator iter = mapping.find(word);
    if (iter == mapping.end()) return NULL;
    string* result = new string[n];
    double* distances = new double[n];
    for (unsigned int i = 0; i < n; i ++) {
        distances[i] = 0;
    }
    Vector<EMBEDDING_SIZE> vec = iter->second;
    for (auto &&iter : mapping) {
        string w = iter.first;
        if (w == word) continue;
        Vector<EMBEDDING_SIZE> v = iter.second;
        double distance = v.dot(vec) / sqrt(v.dot(v) * vec.dot(vec));
        for (unsigned i = 0; i < n; i++) {
            if (distance > distances[i]) {
                for (unsigned int j = n; j > i; j--) {
                    distances[j] = distances[j-1];
                    result[j] = result[j-1];
                }
                distances[i] = distance;
                result[i] = w;
                break;
            }
        }
    }
    return result;
}

void show() {
    const unsigned int n = 10;
    string path("text8.w2v");
    map<string, Vector<EMBEDDING_SIZE>> data = load(path);
    while (1) {
        string word;
        cout << "Input a word, `EXIT` to quit: ";
        cin >> word;
        if (word == "EXIT") exit(0);
        string* result = nearest(data, word, n);
        if (result == NULL) {
            cout << "Can't find word " << word << endl;
            continue;
        }
        for (unsigned int i = 0; i < n; i++) {
            string w = result[i];
            cout << w << endl;
        }
    }
}

int main(int argc, const char * argv[]) {
    cout << "Word2vec" << endl;
    string command(argv[1]);
    if (command == "train") train();
    else if (command == "show") show();
    return 0;
}
