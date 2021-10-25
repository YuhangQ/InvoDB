//
// Created by YuhangQ on 2021/10/25.
//

#include "btree_uuid.h"

BTreeUUID::BTreeUUID(const int& address) {
    root = BTreeNodeUUID::getNode(address);
}

void BTreeUUID::insert(const char *uuid, int address) {

    BTreeNodeUUID* cur = root;
    BTreeNodeUUID* parent = nullptr;

    while(!cur->isLeaf()) {
        parent = cur;
        for(int i=0; i<cur->size(); i++) {
            if(uuid < cur->key[i]) {
                cur = BTreeNodeUUID::getNode(cur->val[i]);
                break;
            }
            if(i == cur->size() - 1) {
                cur = BTreeNodeUUID::getNode(cur->val[i + 1]);
                break;
            }
        }
        cur->parent = cur->getAddress();
    }

    // insert directly
    if(cur->size() < cur->m - 1) {
        cur->val[cur->insert(uuid)] = address;
        cur->save();
        return;
    }

    // split
    split(uuid, address, parent, cur);
}

void BTreeUUID::split(std::string uuid, int address, BTreeNodeUUID *parent, BTreeNodeUUID *cur) {

    cur->val[cur->insert(uuid)] = address;

    BTreeNodeUUID* lLeaf = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
    BTreeNodeUUID* rLeaf = BTreeNodeUUID::getNode(PageManager::Instance().allocate());

    int mid = (cur->m / 2);
    for(int i=0; i<mid; i++) lLeaf->val[lLeaf->insert(cur->key[i])] = cur->val[i];
    for(int i=mid; i<cur->m; i++) rLeaf->val[rLeaf->insert(cur->key[i])] = cur->val[i];

    if(cur == root) {
        BTreeNodeUUID* newRoot = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
        newRoot->insert(rLeaf->key[0]);
        newRoot->val[0] = lLeaf->getAddress();
        newRoot->val[1] = rLeaf->getAddress();
        newRoot->setLeaf(false);
        root = newRoot;
        lLeaf->parent = rLeaf->parent = root->getAddress();

        newRoot->save();
        lLeaf->save();
        rLeaf->save();
    } else insertInternal(rLeaf->key[0], parent, lLeaf, rLeaf);
}

void BTreeUUID::insertInternal(std::string uuid, BTreeNodeUUID *cur, BTreeNodeUUID *lLeaf, BTreeNodeUUID *rLeaf) {

    if(cur->size() < cur->m - 1) {
        int pos = cur->insert(uuid);
        cur->val[pos] = lLeaf->getAddress();
        cur->val[pos+1] = rLeaf->getAddress();
        lLeaf->parent = rLeaf->parent = root->getAddress();
        cur->save();
        lLeaf->save();
        rLeaf->save();
        return;
    }
    BTreeNodeUUID* newLChild = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
    BTreeNodeUUID* newRChild = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
    newLChild->setLeaf(false);
    newRChild->setLeaf(false);

    int pos = cur->insert(uuid);
    cur->val[pos] = lLeaf->getAddress();
    cur->val[pos+1] = rLeaf->getAddress();

    int mid = cur->size() / 2;

    for(int i=0; i<mid; i++) newLChild->insert(cur->key[i]);
    for(int i=0; i<=mid; i++) newLChild->val[i] = cur->val[i];

    for(int i=mid+1; i<cur->m; i++) newRChild->insert(cur->key[i]);
    for(int i=mid+1; i<=cur->m; i++) newRChild->val[i-mid-1] = cur->val[i];

    if(cur == root) {
        BTreeNodeUUID* newRoot = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
        newRoot->insert(cur->key[mid]);
        newRoot->val[0] = newLChild->getAddress();
        newRoot->val[1] = newRChild->getAddress();
        newRoot->setLeaf(false);
        root = newRoot;
        newLChild->parent = newRChild->parent = root->getAddress();

        newRoot->save();
        newLChild->save();
        newRChild->save();


    } else {
        insertInternal(cur->key[mid], BTreeNodeUUID::getNode(cur->parent), newLChild, newRChild);
    }

}

void BTreeUUID::print() {
    innerPrint(root);
}

void BTreeUUID::innerPrint(BTreeNodeUUID *cur) {
    if(cur == root) {
        cnt = 0;
    }
    if(cur->isLeaf()) cnt += cur->size();
    printf("---------%d(%d)count=%d&sum=%d--------\n", cur->getAddress(), cur->isLeaf(), cur->size(), cnt);
    for(int i=0; i<cur->size(); i++) {
        printf("%d:%s ", i, cur->key[i].substr(0, 4).c_str());
    }
    printf("\n");
    for(int i=0; i<=cur->size(); i++) {
        printf("%d:%d ", i, cur->val[i]);
    }
    printf("\n");
    if(cur->isLeaf()) return;
    for(int i=0; i<=cur->size(); i++) {
        innerPrint(BTreeNodeUUID::getNode(cur->val[i]));
    }
}
/*
void BTreeUUID::innerInsert(BTreeNodeUUID* &p, BTreeNodeUUID* f, const char *uuid, int address) {
    if(p == nullptr) {
        p = new BTreeNodeUUID(PageManager::Instance().allocate());
        p->insert(uuid, address);
        return;
    }
    p->insert(uuid, address);
    p->print();
    // full
    if(p->size() == p->m) {
        int mid = p->m / 2;
        BTreeNodeUUID* lnode = new BTreeNodeUUID(PageManager::Instance().allocate());
        BTreeNodeUUID* rnode = new BTreeNodeUUID(PageManager::Instance().allocate());
        for(int i=0; i<mid; i++) lnode->insert(p->link[i].key, p->link[i].value);
        for(int i=mid; i<p->m; i++) rnode->insert(p->link[i].key, p->link[i].value);
    }
}
*/

