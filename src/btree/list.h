//
// Created on 2021/11/1.
//

#ifndef INVODB_LIST_H
#define INVODB_LIST_H

#include "btree.h"

template<typename T, int T_SIZE>
class List {
public:
    void insert(T const& value) {
        tree->insert(value, 0);
    }
    void remove(T const& value) {
        tree->remove(value);
    }
    void print() {
        tree->print();
    }
    bool exists(T const& value) {
        return tree->exists(value);
    }
    std::vector<T> all() {
        return tree->keySet();
    }
    List(const int& address) {
        tree = new BTree<T, T_SIZE>(address);
    }

private:
    BTree<T, T_SIZE>* tree;
};


#endif //INVODB_LIST_H
