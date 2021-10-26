//
// Created by YuhangQ on 2021/10/25.
//

#include "btree_node.h"

std::map<int, BTreeNodeUUID*> BTreeNodeUUID::map;

BTreeNodeUUID *BTreeNodeUUID::getNode(const int &address) {
    if(true || map.count(address) == 0) {
        map[address] = new BTreeNodeUUID(address);
    }
    return map[address];
}

BTreeNodeUUID::BTreeNodeUUID(const int& address):address(address) {
    clear();
    StoragePage page = PageManager::Instance().getPage(address);
    int p = 0;
    size = page.getIntStartFrom(p);
    p += 4;
    parent = page.getIntStartFrom(p);
    p += 4;
    left = page.getIntStartFrom(p);
    p += 4;
    right = page.getIntStartFrom(p);
    p += 4;
    leaf = !page.getIntStartFrom(p);
    p += 4;
    for(int i=0; i<m; i++) {
        for(int j=0; j<32; j++) {
            key[i] += page[p++];
        }
    }
    for(int i=0; i<m+1; i++) {
        val[i] = page.getIntStartFrom(p);
        p += 4;
    }
}

int BTreeNodeUUID::insert(const std::string uuid) {
    int pos = 0;
    while(pos < size && uuid > key[pos]) pos++;
    val[size + 1] = val[size];
    for(int i=size; i>pos; i--) {
        val[i] = val[i - 1];
        key[i] = key[i - 1];
    }
    key[pos] = uuid;
    size++;
    return pos;
}

void BTreeNodeUUID::print() {
    printf("---------BTreeNode---------\n");
    for(int i=0; i<size; i++) {
        printf("%s %d\n", key[i].c_str(), val[i]);
    }
}

void BTreeNodeUUID::clear() {
    for(int i=0; i<m+1; i++) key[i].clear(), val[i] = 0;
    size = 0;
    leaf = false;
    parent = 0;
}

void BTreeNodeUUID::save() {
    StoragePage page(address);

    int p = 0;
    page.setIntStartFrom(p, size);
    p += 4;
    page.setIntStartFrom(p, parent);
    p += 4;
    page.setIntStartFrom(p, left);
    p += 4;
    page.setIntStartFrom(p, right);
    p += 4;
    page.setIntStartFrom(p, !leaf);
    p += 4;
    for(int i=0; i<m; i++) {
        for(int j=0; j<32; j++) {
            page[p++] = key[i][j];
        }
    }
    for(int i=0; i<m+1; i++) {
        page.setIntStartFrom(p, val[i]);
        p += 4;
    }
    page.save();
}
