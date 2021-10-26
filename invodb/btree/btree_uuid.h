//
// Created by YuhangQ on 2021/10/25.
//

#ifndef INVODB_BTREE_UUID_H
#define INVODB_BTREE_UUID_H

#include <btree/btree_node.h>

class BTreeUUID {
public:
    BTreeUUID(const int& address);
    void insert(const char* uuid, int address);
    void print();
private:
    void innerPrint(BTreeNodeUUID* cur);
    void split(std::string uuid, int address, int parentAddr, int curAddr);
    void insertInternal(std::string uuid, int curAddr, int lLeafAddr, int rLeafAddr);
    int root;
    int cnt;
};


#endif //INVODB_BTREE_UUID_H
