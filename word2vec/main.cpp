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
#include <pthread.h>
#include "counter.hpp"
#include "vector.hpp"
#include "word2vec.hpp"
#include "settings.h"

using namespace std;


unsigned long* BuildTable(Counter<unsigned long> counter) {
    const double power = 0.75;
    double total = 0.0, cum = 0.0;
    unsigned long *table = new unsigned long[TABLE_SIZE];
    Pair<unsigned long>** sorted = counter.sort();
    unsigned long size = counter.size();
    for (Pair<unsigned long>** ptr = sorted + size - 1; (*ptr)->count < 5; ptr--) {
        size--;
    }
    for (unsigned long i = 0; i < size; i++) {
        total += pow((double)(sorted[i]->count), power);
    }
    unsigned long j = -1;
    for (unsigned long i = 0; i < TABLE_SIZE; i++) {
        if ((double)j / TABLE_SIZE > cum) {
            cum += pow(sorted[++j]->count, power) / total;
        }
        table[i] = sorted[j]->item;
    }
    return table;
}


vector<string> BuildVocabulary(char* filePath) {
    cout << "Building Vocabulary.....";
    ifstream file(filePath);
    Counter<string> counter;
    
    string word;
    if (!file.is_open()) {
        cout << "Can't open file " << filePath << endl;
        exit(1);
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
    cout << "Finished" << endl;
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
    cout << "Building Corpus......";
    map<string, unsigned long> inverseDict = BuildInverseDict(vocabulary);
    ifstream corpusFile(filePath);
    if (!corpusFile.is_open()) {
        cout << "Can't open file " << filePath << endl;
        exit(1);
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
    cout << "Finished" << endl;
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

struct Param {
    int id;
    Word2Vec* w2v;
    vector<unsigned long>* corpus;
    unsigned long *table;
};

void* trainThread(void* args) {
    Param pargs = *((Param*)args);
    vector<unsigned long> corpus = *(pargs.corpus);
    Word2Vec w2v = *(pargs.w2v);
    unsigned long blockSize = (unsigned long)(pargs.corpus->size() / NUM_THREADS);
    unsigned long start = pargs.id * blockSize < WINDOW_RADIUS ? WINDOW_RADIUS : pargs.id * blockSize,
                  end   = (pargs.id + 1) * blockSize >= corpus.size() - WINDOW_RADIUS ? corpus.size() - WINDOW_RADIUS : (pargs.id+1) * blockSize;
    clock_t t1 = clock();
    double sumLoss = 0.0, loss;
    const unsigned long N = 100000;
    unsigned long pos_c, neg_c;
    default_random_engine generator;
    uniform_int_distribution<unsigned long> distribution(1, TABLE_SIZE);
    double lr = LR;
    for (unsigned int epoch = 0; epoch < NUM_EPOCHS; epoch++) {
        for (unsigned long long i = start; i < end; i++) {
            unsigned long word = corpus[i];
            Vector<EMBEDDING_SIZE> grad = Vector<EMBEDDING_SIZE>();
            for (int j = -WINDOW_RADIUS; j < WINDOW_RADIUS; j++) {
                if (j == 0) continue;
                pos_c = corpus[j < 0 ? i + j : i + j + 1];
                loss = w2v.update(word, pos_c, lr, false, &grad);
                sumLoss += loss;
                for (int k = 0; k < NEG_NUM; k++) {
                    neg_c = pargs.table[distribution(generator)];
                    if (neg_c == word) continue;
                    loss = w2v.update(word, neg_c, lr, true, &grad);
                    sumLoss += loss;
                }
            }
            w2v.applyGrad(word, grad);
            if (pargs.id == 0 && i % N == 0) {
                clock_t t2 = clock();
                double delta_t = (double)(t2 - t1) / CLOCKS_PER_SEC;
                t1 = t2;
                double meanLoss = sumLoss / (double)(N * (NEG_NUM + 1) * WINDOW_RADIUS * 2);
                double pct = ((double)i / (end - start + 1) + epoch) / NUM_EPOCHS * 100.0;
                lr = LR * pow(0.00001, pct / 100);
                cout << "Loss: " << meanLoss << "\trps: " << N / delta_t;
                cout << "\tTime: " << delta_t << "seconds elapsed\tProgress: " << pct << '%' << endl;
                sumLoss = 0;
            }
        }
    }
    pthread_exit(NULL);
}

void train(){
    string path("text8");
    char *p = (char*)path.data();
    vector<string> vocabulary = GetVocabulary(p);
    vector<unsigned long> corpus = GetCorpus(p, vocabulary);
    Counter<unsigned long> counter;
    for (unsigned long item : corpus) {
        counter.add(item);
    }
    unsigned long *table = BuildTable(counter);
    unsigned long maxIndex = vocabulary.size() + 1;
    Word2Vec w2v = Word2Vec(maxIndex, WINDOW_RADIUS, NEG_NUM);
    pthread_t *pt = new pthread_t[NUM_THREADS];
    for (int threadId = 0; threadId < NUM_THREADS; threadId++) {
        Param *args    = new Param();
        args->id       = threadId;
        args->w2v      = &w2v;
        args->corpus   = &corpus;
        args->table   = table;
        //trainThread(&args);
        pthread_create(&pt[threadId], NULL, trainThread, args);
    }
    for (int threadId = 0; threadId < NUM_THREADS; threadId++) {
        pthread_join(pt[threadId], NULL);
    }
    w2v.save(path + ".w2v", vocabulary);
    delete[] pt;
}

map<string, Vector<EMBEDDING_SIZE>> load(string path) {
    cout << "Loading Model" << endl;
    map<string, Vector<EMBEDDING_SIZE>> data;
    ifstream iFile(path);
    if (!iFile.is_open()) {
        cout << "Can't open file " << path << endl;
        exit(1);
    }
    while (!iFile.eof()) {
        string word;
        Vector<EMBEDDING_SIZE> vec;
        iFile >> word >> vec;
        data.insert(map<string, Vector<EMBEDDING_SIZE>>::value_type(word, vec));
    }
    cout << "Load model complete, " << data.size() << " instances loaded" << endl;
    return data;
}

string* nearest(const map<string, Vector<EMBEDDING_SIZE>> &mapping, const string &word, unsigned int n=10) {
    map<string, Vector<EMBEDDING_SIZE>>::const_iterator iter = mapping.find(word);
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
                for (unsigned int j = n-1; j > i; j--) {
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
        // delete result;
    }
}

int main(int argc, const char * argv[]) {
    cout << "Word2vec" << endl;
    string command(argv[1]);
    if (command == "train") train();
    else if (command == "show") show();
    return 0;
}
