//
// Created by i on 2021/10/24.
//

#ifndef INVODB_BTREE_NODE_H
#define INVODB_BTREE_NODE_H

#include <iostream>
#include <cstring>

/**
 * m = 16
 * value string max 56
 * (56 + 4)*16 + 4 * 16 = 1024
 */

class BTreeNode {
public:
    BTreeNode() { memset(arr, 0, sizeof(arr)); }
private:
    struct NodeValue {
        char key[56];
        int value;
    };
    std::pair<char *, int> arr[127];
};

#endif //INVODB_BTREE_NODE_H
