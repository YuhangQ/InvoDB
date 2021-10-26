//
// Created by YuhangQ on 2021/10/25.
//

#include "btree_uuid.h"

BTreeUUID::BTreeUUID(const int& address) {
    root = address;
}

void BTreeUUID::insert(const char *uuid, int address) {

    BTreeNodeUUID* cur = BTreeNodeUUID::getNode(root);
    BTreeNodeUUID* parent = nullptr;

    while(!cur->leaf) {
        parent = cur;
        for(int i=0; i<cur->size; i++) {
            if(uuid < cur->key[i]) {
                cur = BTreeNodeUUID::getNode(cur->val[i]);
                break;
            }
            if(i == cur->size - 1) {
                cur = BTreeNodeUUID::getNode(cur->val[i + 1]);
                break;
            }
        }
        cur->parent = parent->address;
        cur->save();
    }

    // insert directly
    if(cur->size < cur->m - 1) {
        cur->val[cur->insert(uuid)] = address;
        cur->save();
        return;
    }

    // split
    if(parent == nullptr) split(uuid, address, 0, cur->address);
    else split(uuid, address, parent->address, cur->address);
}

void BTreeUUID::split(std::string uuid, int address, int parentAddr, int curAddr) {

    BTreeNodeUUID* parent = BTreeNodeUUID::getNode(parentAddr);
    BTreeNodeUUID* cur = BTreeNodeUUID::getNode(curAddr);

    cur->val[cur->insert(uuid)] = address;

    BTreeNodeUUID* lLeaf = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
    BTreeNodeUUID* rLeaf = BTreeNodeUUID::getNode(PageManager::Instance().allocate());

    int mid = (cur->m / 2);
    for(int i=0; i<mid; i++) lLeaf->val[lLeaf->insert(cur->key[i])] = cur->val[i];
    lLeaf->right = rLeaf->address;
    lLeaf->left = cur->left;
    for(int i=mid; i<cur->m; i++) rLeaf->val[rLeaf->insert(cur->key[i])] = cur->val[i];
    rLeaf->left = rLeaf->address;
    rLeaf->right = cur->right;

    if(cur->address == root) {
        BTreeNodeUUID* newRoot = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
        newRoot->insert(rLeaf->key[0]);
        newRoot->val[0] = lLeaf->address;
        newRoot->val[1] = rLeaf->address;
        newRoot->leaf = false;
        root = newRoot->address;
        lLeaf->parent = rLeaf->parent = root;

        newRoot->save();
        lLeaf->save();
        rLeaf->save();
    } else {
        lLeaf->save();
        rLeaf->save();
        insertInternal(rLeaf->key[0], parent->address, lLeaf->address, rLeaf->address);
    }
}

void BTreeUUID::insertInternal(std::string uuid, int curAddr, int lLeafAddr, int rLeafAddr) {

    BTreeNodeUUID *cur = BTreeNodeUUID::getNode(curAddr);
    BTreeNodeUUID *lLeaf = BTreeNodeUUID::getNode(lLeafAddr);
    BTreeNodeUUID *rLeaf = BTreeNodeUUID::getNode(rLeafAddr);

    if(cur->size < cur->m - 1) {
        int pos = cur->insert(uuid);
        cur->val[pos] = lLeaf->address;
        cur->val[pos+1] = rLeaf->address;
        lLeaf->parent = rLeaf->parent = root;
        cur->save();
        lLeaf->save();
        rLeaf->save();
        return;
    }

    BTreeNodeUUID* newLChild = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
    BTreeNodeUUID* newRChild = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
    newLChild->leaf = false;
    newRChild->leaf = false;

    int pos = cur->insert(uuid);
    cur->val[pos] = lLeaf->address;
    cur->val[pos+1] = rLeaf->address;

    int mid = cur->size / 2;

    for(int i=0; i<mid; i++) newLChild->insert(cur->key[i]);
    for(int i=0; i<=mid; i++) newLChild->val[i] = cur->val[i];

    for(int i=mid+1; i<cur->m; i++) newRChild->insert(cur->key[i]);
    for(int i=mid+1; i<=cur->m; i++) newRChild->val[i-mid-1] = cur->val[i];

    lLeaf->save();
    rLeaf->save();

    if(cur->address == root) {
        BTreeNodeUUID* newRoot = BTreeNodeUUID::getNode(PageManager::Instance().allocate());
        newRoot->insert(cur->key[mid]);
        newRoot->val[0] = newLChild->address;
        newRoot->val[1] = newRChild->address;
        newRoot->leaf = false;
        root = newRoot->address;
        newLChild->parent = newRChild->parent = root;

        newRoot->save();
        newLChild->save();
        newRChild->save();
    } else {
        newLChild->save();
        newRChild->save();
        if(cur->parent == 0) throw "fuck";
        insertInternal(cur->key[mid], cur->parent, newLChild->address, newRChild->address);
    }
}

void BTreeUUID::print() {
    innerPrint(BTreeNodeUUID::getNode(root));
}

void BTreeUUID::innerPrint(BTreeNodeUUID *cur) {
    if(cur->address == root) {
        cnt = 0;
    }
    if(cur->leaf) cnt += cur->size;
    printf("---------%d(%d)count=%d&sum=%d---l:%d,r:%d-----\n", cur->address, cur->leaf, cur->size, cnt, cur->left, cur->right);
    for(int i=0; i<cur->size; i++) {
        printf("%d:%s ", i, cur->key[i].substr(0, 4).c_str());
    }
    printf("\n");
    for(int i=0; i<=cur->size; i++) {
        printf("%d:%d ", i, cur->val[i]);
    }
    printf("\n");


    if(cur->leaf) return;
    for(int i=0; i<=cur->size; i++) {
        innerPrint(BTreeNodeUUID::getNode(cur->val[i]));
    }


    /*
    BTreeNodeUUID test = *cur;
    test.address = PageManager::Instance().allocate();
    test.print();


    test.save();

    BTreeNodeUUID test2 = *BTreeNodeUUID::getNode(test.address);

    printf("test: size:%d l:%d r:%d\n", test2.size, test2.left, test2.right);
    test2.print();
     */


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


