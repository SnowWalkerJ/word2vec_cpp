//
//  counter.hpp
//  word2vec
//
//  Created by 陈秉烜 on 2017/8/23.
//  Copyright © 2017年 陈秉烜. All rights reserved.
//

#ifndef counter_hpp
#define counter_hpp

#include <map>
#include <stdio.h>

using namespace std;


template <class T>
class Pair {
public:
    const T item;
    const unsigned long count;
    Pair(T item, unsigned long count):item(item), count(count) {};
    Pair(Pair<T> const &other):item(other.item),count(other.count) {};
};
    

template <class T>
    bool compare(const Pair<T>* item1, const Pair<T>* item2) {
        return item1->count > item2->count;
    }


template <typename T>
class Counter {
private:
    typedef map<T, unsigned long> MyMap;
    MyMap data;
public:
    unsigned long operator[](T item) {
        typename MyMap::iterator iter = data.find(item);
        unsigned long n = iter == data.end() ? 0 : iter->second;
        return n;
    }
    void add(T item) {
        data[item] = this->operator[](item) + 1;
    }
    Pair<T>** sort() {
        unsigned long size = data.size();
        Pair<T>** result = (Pair<T>**)malloc(size * sizeof(Pair<T>*));
        unsigned long i = 0;
        for (typename MyMap::iterator iter = data.begin(); iter != data.end(); iter++) {
            result[i++] = new Pair<T>(iter->first, iter->second);
        }
        std::sort(result, result+size, compare<T>);
        return result;
    }
    unsigned long size() {
        return data.size();
    }
};

#endif /* counter_hpp */
