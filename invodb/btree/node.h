//
// Created by i on 2021/10/24.
//

#ifndef INVODB_NODE_H
#define INVODB_NODE_H

#include <iostream>
#include <cstring>
#include <algorithm>
#include <map>
#include <type_traits>
#include "file/page_manager.h"
#include "models/cache.h"

template<int M_SIZE, typename KT, int K_SIZE>
class BTreeNode {
public:
    static std::shared_ptr<BTreeNode<M_SIZE, KT, K_SIZE>> getNode(const int &index);
    static BTreeNode<M_SIZE, KT, K_SIZE>* release(const int &index);
    int insert(KT const &key);
    int findPos(KT const &key);
    void update() {
        if(leaf) return;
        for(int i=0; i<=size; i++) {
            auto node = getNode(linkSet[i]);
            node->parent = address;
            node->save();
        }
    }
    void release();
    void clear();
    int save();
    static const int m = M_SIZE;
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
    KT keySet[m + 1];
    int linkSet[m + 1];
    int parent;
    int left;
    int right;
    bool leaf;
    int size;
    int address;
private:
    BTreeNode(const int& address);
};

template<int M_SIZE, typename KT, int K_SIZE>
BTreeNode<M_SIZE, KT, K_SIZE>::BTreeNode(const int& address): address(address) {
    clear();
    StoragePage page = PageManager::Instance().getPage(address);
    int p = 0;
    size = page.getIntStartFrom(p); p += 4;
    parent = page.getIntStartFrom(p); p += 4;
    left = page.getIntStartFrom(p); p += 4;
    right = page.getIntStartFrom(p); p += 4;
    leaf = !page.getIntStartFrom(p); p += 4;

    if(std::is_same<KT, std::string>::value) {
        for(int i=0; i<m; i++) {
            std::string *str = (std::string*)&keySet[i];
            bool flag = true;
            for(int j=0; j<K_SIZE; j++) {
                char c = page[p++];
                if(c == '\0') flag = false;
                if(!flag) continue;
                str->push_back(c);
            }
        }
    } else {
        for(int i=0; i<m; i++) {
            keySet[i] = *(KT*)(&page[p]);
            p += K_SIZE;
        }
    }

    for (int i = 0; i <= m; i++) {
        linkSet[i] = *(int*)(&page[p]);
        p += 4;
    }

}

template<int M_SIZE, typename KT, int K_SIZE>
std::shared_ptr<BTreeNode<M_SIZE, KT, K_SIZE>> BTreeNode<M_SIZE, KT, K_SIZE>::getNode(const int &index) {
    if(index == 0) {
        throw "invalid address!";
    }
    return std::make_shared<BTreeNode<M_SIZE, KT, K_SIZE>>( BTreeNode<M_SIZE, KT, K_SIZE>(index));
    static LRUCache<int, BTreeNode<M_SIZE, KT, K_SIZE>> cache(1000000);
    if(!cache.exist(index)) {
        auto p = std::make_shared<BTreeNode<M_SIZE, KT, K_SIZE>>( BTreeNode<M_SIZE, KT, K_SIZE>(index));
        cache.put(index, p);
        return p;
    } else {
        auto p = cache.get(index);
        cache.put(index, p);
        return p;
    }
}

template<int M_SIZE, typename KT, int K_SIZE>
BTreeNode<M_SIZE, KT, K_SIZE> *BTreeNode<M_SIZE, KT, K_SIZE>::release(const int &index) {
    PageManager::Instance().release(index);
    return nullptr;
}

template<int M_SIZE, typename KT, int K_SIZE>
int BTreeNode<M_SIZE, KT, K_SIZE>::insert(const KT &key) {
    int pos = 0;
    while(pos < size && key > keySet[pos]) pos++;
    linkSet[size + 1] = linkSet[size];
    for(int i=size; i>pos; i--) {
        linkSet[i] = linkSet[i - 1];
        keySet[i] = keySet[i - 1];
    }
    keySet[pos] = key;
    size++;
    return pos;
}

template<int M_SIZE, typename KT, int K_SIZE>
int BTreeNode<M_SIZE, KT, K_SIZE>::findPos(const KT &key) {
    int pos = std::lower_bound(keySet, keySet+size, key) - keySet;
    if(pos == size || keySet[pos] != key) return -1;
    return pos;
}

template<int M_SIZE, typename KT, int K_SIZE>
void BTreeNode<M_SIZE, KT, K_SIZE>::release() {
    BTreeNode<M_SIZE, KT, K_SIZE>::release(this->address);
}

template<int M_SIZE, typename KT, int K_SIZE>
void BTreeNode<M_SIZE, KT, K_SIZE>::clear() {
    for(int i=0; i<=m; i++) {
        if(std::is_same<KT, std::string>::value) {
            ((std::string *)&keySet[i])->clear();
        }
        if(std::is_same<KT, double>::value) {
            *((double *)&keySet[i]) = 0;
        }
        if(std::is_same<KT, bool>::value) {
            *((bool *)&keySet[i]) = 0;
        }
        if(std::is_same<KT, int>::value) {
            *((int *)&keySet[i]) = 0;
        }
        linkSet[i] = 0;
    }
    size = 0;
    leaf = true;
    parent = 0;
    left = 0;
    right = 0;
}

template<int M_SIZE, typename KT, int K_SIZE>
int BTreeNode<M_SIZE, KT, K_SIZE>::save() {


    StoragePage page(address);
    int p = 0;
    page.setIntStartFrom(p, size); p += 4;
    page.setIntStartFrom(p, parent); p += 4;
    page.setIntStartFrom(p, left); p += 4;
    page.setIntStartFrom(p, right); p += 4;
    page.setIntStartFrom(p, !leaf); p += 4;

    if(std::is_same<KT, std::string>::value) {
        for(int i=0; i<m; i++) {
            std::string *str = (std::string*)&keySet[i];
            page.setStartFrom(p, str->c_str(), str->size());
            p += K_SIZE;
        }
    } else {
        for(int i=0; i<m; i++) {
            page.setStartFrom(p, &keySet[i], K_SIZE);
            p += K_SIZE;
        }
    }

    for (int i = 0; i <= m; i++) {
        page.setStartFrom(p, &linkSet[i], 4);
        p += 4;
    }

    if(p > 1024) {
        throw "too big page!";
    }

    page.save();

    return p;
}

#endif //INVODB_NODE_H
