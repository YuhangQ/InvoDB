//
// Created by YuhangQ on 2021/10/25.
//

#ifndef INVODB_BTREE_UUID_H
#define INVODB_BTREE_UUID_H

#include "btree/btree_node.h"
#include "utils/uuid.h"

class BTreeUUID {
public:
    BTreeUUID(const int& address);
    void insert(const std::string& uuid, int address);
    void remove(const std::string& uuid);
    int find(const std::string& uuid);
    void print();
    int test();
private:
    void removeEntry(int curAddr, const std::string& uuid, const int& pointer);
    bool canCoalesce(int curAddr, int sibAddr);
    void coalesce(int curAddr, int sibAddr);
    bool canRedistribute(int curAddr, int sibAddr);
    void redistribute(int curAddr, int sibAddr);
    void innerPrint(NodeUUID* cur);
    int findNode(const std::string& uuid);
    void split(const std::string& uuid, int address, int parentAddr, int curAddr);
    void insertInternal(const std::string& uuid, int curAddr, int lLeafAddr, int rLeafAddr);
    int root;
    int cnt;
};


#endif //INVODB_BTREE_UUID_H
