//
// Created by YuhangQ on 2021/10/25.
//

#include "btree_node.h"

std::map<int, BTreeNodeUUID*> BTreeNodeUUID::map;

BTreeNodeUUID *BTreeNodeUUID::getNode(const int &address) {
    if(address == 0) {
        printf("???");
        exit(0);
    }
    if(map.count(address) == 0) {
        map[address] = new BTreeNodeUUID(address);
    }
    return map[address];
}
