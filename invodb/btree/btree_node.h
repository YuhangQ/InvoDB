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


class NodeUUID {
public:
    static NodeUUID* getNode(const int& address);
    static NodeUUID* release(const int& address);
    int insert(const std::string& uuid);
    int findPos(const std::string& uuid);
    void print();
    void release();
    void clear();
    void save();
    //static const int m = 27;
    static const int m = 3;
    static const int maxCount = m - 1;
    static const int minLeafCount = m / 2;
    static const int minLinkCount = (m - 1) / 2;
    bool enough() {
        if(leaf) return size >= minLeafCount;
        else return size >= minLinkCount;
    }
    bool full() {
        return size == maxCount;
    }
    std::string key[m+1];
    int val[m+1];
    int parent;
    int left;
    int right;
    bool leaf;
    int size;
    int address;
private:
    NodeUUID(const int& address);
    static std::map<int, NodeUUID*> map;

};

#endif //INVODB_BTREE_NODE_H
