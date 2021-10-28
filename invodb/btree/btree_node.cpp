//
// Created by YuhangQ on 2021/10/25.
//

#include "btree_node.h"

std::map<int, NodeUUID*> NodeUUID::map;

NodeUUID *NodeUUID::getNode(const int &address) {
    if(map.count(address) == 0) {
        delete map[address];
        map[address] = new NodeUUID(address);
    }
    return map[address];
}

NodeUUID::NodeUUID(const int& address):address(address) {
    clear();
    StoragePage page = PageManager::Instance().getPage(address);
    int p = 0;
    size = page.getIntStartFrom(p); p += 4;
    parent = page.getIntStartFrom(p); p += 4;
    left = page.getIntStartFrom(p); p += 4;
    right = page.getIntStartFrom(p); p += 4;
    leaf = !page.getIntStartFrom(p); p += 4;
    for(int i=0; i<m; i++) {
        for(int j=0; j<32; j++) {
            key[i].push_back(page[p++]);
        }
    }
    for(int i=0; i<m+1; i++) {
        val[i] = page.getIntStartFrom(p);
        p += 4;
    }
}

int NodeUUID::insert(const std::string& uuid) {
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

void NodeUUID::print() {
    printf("---------BTreeNode---------\n");
    for(int i=0; i<size; i++) {
        printf("%s %d\n", key[i].c_str(), val[i]);
    }
}

void NodeUUID::clear() {
    for(int i=0; i<m+1; i++) key[i].clear(), val[i] = 0;
    size = 0;
    leaf = false;
    parent = 0;
}

void NodeUUID::save() {
    StoragePage page(address);

    int p = 0;
    page.setIntStartFrom(p, size); p += 4;
    page.setIntStartFrom(p, parent); p += 4;
    page.setIntStartFrom(p, left); p += 4;
    page.setIntStartFrom(p, right); p += 4;
    page.setIntStartFrom(p, !leaf); p += 4;
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

int NodeUUID::findPos(const std::string &uuid) {
    int pos = std::lower_bound(key, key+size, uuid) - key;
    if(pos == size || key[pos] != uuid) return -1;
    return pos;
}

void NodeUUID::release() {
    NodeUUID::release(this->address);
}

NodeUUID *NodeUUID::release(const int &address) {
    return nullptr;
}
