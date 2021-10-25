//
// Created by YuhangQ on 2021/10/25.
//

#ifndef INVODB_BTREE_UUID_H
#define INVODB_BTREE_UUID_H

#include <btree/btree_node.h>

class BTreeUUID {
public:
    BTreeUUID(const int& address);
    ~BTreeUUID() { delete root; };
    void insert(const char* uuid, int address);
    void print();
    void innerPrint(BTreeNodeUUID* cur);
private:
    void split(std::string uuid, int address, BTreeNodeUUID* parent, BTreeNodeUUID* cur);
    void insertInternal(std::string uuid, BTreeNodeUUID* cur, BTreeNodeUUID* lLeaf, BTreeNodeUUID* rLeaf);
    BTreeNodeUUID *root;
    int cnt;
};


#endif //INVODB_BTREE_UUID_H
