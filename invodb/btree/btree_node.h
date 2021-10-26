//
// Created by i on 2021/10/24.
//

#ifndef INVODB_BTREE_NODE_H
#define INVODB_BTREE_NODE_H

#include <iostream>
#include <cstring>
#include <algorithm>
#include <map>
#include "file/page_manager.h"

/**
 * m = 27
 * value string max
 * (32 + 4)*28 + 5 = 1013
 */


class BTreeNodeUUID {
public:
    static BTreeNodeUUID* getNode(const int& address);
    int insert(const std::string uuid);
    void print();
    void clear();
    void save();
    static const int m = 27;
    std::string key[m+1];
    int val[m+1];
    int parent;
    int left;
    int right;
    bool leaf;
    int size;
    int address;
private:
    BTreeNodeUUID(const int& address);
    static std::map<int, BTreeNodeUUID*> map;

};

#endif //INVODB_BTREE_NODE_H
