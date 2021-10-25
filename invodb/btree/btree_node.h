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
 * m = 28
 * value string max
 * (32 + 4)*28 + 5 = 1013
 */


class BTreeNodeUUID {
public:
    static BTreeNodeUUID* getNode(const int& address);
    BTreeNodeUUID(const int& address):address(address) {
        clear();
        StoragePage page = PageManager::Instance().getPage(address);
        for(int i=0; i<28; i++) {
            for(int j=0; j<32; j++) {
                key[i] += page[i * 32 + j];
            }
        }
        int p = 28 * 32;
        for(int i=0; i<m+1; i++) {
            val[i] = page.getIntStartFrom(p);
            p += 4;
        }
        leaf = !page[1013];
        parent = page.getIntStartFrom(1014);
    }
    bool isLeaf() {
        return leaf;
    }

    void setLeaf(const bool& value) {
        leaf = value;
    }
    void updateParent() {
        if(isLeaf()) return;
        for(int i=0; i<=size(); i++) {
            BTreeNodeUUID child = getNode(val[i])->parent;
            child.parent = address;
            child.save();
        }
    }
    int size() {
        return n_size;
    }
    int insert(const std::string uuid) {

//        static int cnt = 0;
//        if(!isLeaf() && size() >= 27) {
//            if(cnt) throw "fuck";
//            cnt++;
//        }
        int pos = 0;
        while(pos < n_size && uuid > key[pos]) pos++;
        val[n_size + 1] = val[n_size];
        for(int i=n_size; i>pos; i--) {
            val[i] = val[i - 1];
            key[i] = key[i - 1];
        }
        key[pos] = uuid;
        n_size++;
        return pos;
    }
    void print() {
        printf("---------BTreeNode---------\n");
        for(int i=0; i<size(); i++) {
            printf("%s %d\n", key[i].c_str(), val[i]);
        }
    }
    void clear() {
        for(int i=0; i<m; i++) key[i].clear(), val[i] = 0;
        n_size = 0;
        leaf = false;
        parent = 0;
    }
    int getAddress() { return address; }
    void save() {
        StoragePage page(address);
        for(int i=0; i<28; i++) {
            for(int j=0; j<32; j++) {
                page[i * 32 + j] = key[i][j];
            }
        }
        int p = 28 * 32;
        for(int i=0; i<m+1; i++) {
            page.setIntStartFrom(p, val[i]);
            p += 4;
        }
        page[1013] = !leaf;
        page.setIntStartFrom(1014, parent);
        page.save();
    }
    static const int m = 28;
    std::string key[m+100];
    int val[m+200];
    int parent;
private:
    static std::map<int, BTreeNodeUUID*> map;
    int address;
    int n_size;
    bool leaf;
};

#endif //INVODB_BTREE_NODE_H
