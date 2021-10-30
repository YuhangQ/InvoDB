//
// Created by YuhangQ on 2021/10/25.
//

#ifndef INVODB_BTREE_H
#define INVODB_BTREE_H

#include "btree/node.h"
#include "utils/uuid.h"

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
class BTree {
public:
    BTree(const int& address);
    void insert(const KT &key, const VT &value);
    void update(const KT &key, const VT &value);
    void remove(const KT &key);
    int getNodeSize();
    VT find(const KT &key);
    int size();
private:
    void removeEntry(int curAdd, const std::string& key, const int& pointer);
    bool canCoalesce(int curAdd, int sibAdd);
    void coalesce(int curAdd, int sibAdd);
    bool canRedistribute(int curAdd, int sibAdd);
    void redistribute(int curAdd, int sibAdd);
    int findNode(const KT &key);
    void split(const KT &key, int address, int parentAdd, int curAdd);
    void insertInternal(const KT &key, int curAdd, int lLeafAdd, int rLeafAdd);
    int root;
    int n_size;
};

// BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE> BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::BTree(const int& address) {
    root = address;
    n_size = 0;
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
void BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::insert(const KT &key, const VT &value) {
    if(find(key) != -1) {
        throw "keySet already exists.";
    }

    n_size++;

    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(findNode(key));

    // insert directly
    if(cur->size < cur->m - 1) {
        cur->linkSet[cur->insert(key)] = value;
        cur->save();
        return;
    }

    // split
    split(key, value, cur->parent, cur->address);
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
void BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::update(const KT &key, const VT &value) {
    if(find(key) == -1) {
        throw "keySet doesn't exists.";
    }
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(findNode(key));
    cur->linkSet[cur->findPos(key)] = value;
    cur->save();
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
void BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::remove(const KT &key) {
    if(find(key) == -1) {
        throw "keySet doesn't exists.";
    }
    n_size--;
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(findNode(key));
    removeEntry(cur->address, key, find(key));
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
VT BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::find(const KT &key) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(findNode(key));
    for(int i=0; i<cur->size; i++) {
        if(key == cur->keySet[i]) return cur->linkSet[i];
    }
    return -1;
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
int BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::size() {
    return n_size;
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
void BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::removeEntry(int curAdd, const std::string &key, const int &pointer) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(curAdd);
    int pos = cur->findPos(key);
    if(pos == -1) return;
    for(int i=pos; i<cur->size-1; i++) {
        cur->keySet[i] = cur->keySet[i + 1];
    }
    for(int i=pos+(cur->linkSet[pos] != pointer); i<cur->size; i++) {
        cur->linkSet[i] = cur->linkSet[i + 1];
    }
    cur->size--;
    cur->save();

    if(curAdd == root && !cur->leaf && cur->size == 0) {
        root = cur->linkSet[0];
        BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>* root = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->linkSet[0]);
        root->parent = 0;
        root->save();
        cur->release();
        return;
    }

    if(cur->enough() || cur->address == root) return;

    if(canCoalesce(cur->address, cur->left)) {
        coalesce(cur->address, cur->left);
    } else if(canCoalesce(cur->address, cur->right)) {
        coalesce(cur->address, cur->right);
    } else if(canRedistribute(cur->address, cur->left)) {
        redistribute(cur->address, cur->left);
    } else if(canRedistribute(cur->address, cur->right)) {
        redistribute(cur->address, cur->right);
    } else {
        throw "these is a bug!";
    }
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
bool BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::canCoalesce(int curAdd, int sibAdd) {
    if(sibAdd == 0) return false;
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(curAdd);
    auto sib = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(sibAdd);
    if(cur->parent != sib->parent) return false;
    return (cur->size + sib->size <= BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::m - 1 - !cur->leaf);
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
void BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::coalesce(int curAdd, int sibAdd) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(curAdd);
    auto sib = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(sibAdd);
    auto parent = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->parent);
    std::string *k;
    for(int i=0; i<parent->size; i++) {
        if((parent->linkSet[i] == curAdd && parent->linkSet[i+1] == sibAdd)
           || (parent->linkSet[i] == sibAdd && parent->linkSet[i+1] == curAdd)) {
            k = &parent->keySet[i];
            break;
        }
    }
    BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>* newNode = nullptr;
    if(cur->left == sibAdd) {
        if(!cur->leaf) sib->insert(*k);
        for(int i=0; i<cur->size; i++) {
            sib->linkSet[sib->insert(cur->keySet[i])] = cur->linkSet[i];
        }
        sib->linkSet[sib->size] = cur->linkSet[cur->size];
        sib->right = cur->right;
        if(cur->right) {
            BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE> *right = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->right);
            right->left = sib->address;
            right->save();
        }
        newNode = sib;
        newNode->save();
        removeEntry(parent->address, *k, curAdd);
        cur->release();
    } else {
        if(!cur->leaf) cur->insert(*k);
        for(int i=0; i<sib->size; i++) {
            cur->linkSet[cur->insert(sib->keySet[i])] = sib->linkSet[i];
        }
        cur->linkSet[cur->size] = sib->linkSet[sib->size];

        cur->right = sib->right;

        if(sib->right) {
            BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE> *right = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(sib->right);
            right->left = cur->address;
            right->save();
        }
        newNode = cur;
        newNode->save();
        removeEntry(parent->address, *k, sibAdd);
        sib->release();
    }
    if(newNode->leaf) return;
    for(int i=0; i<=newNode->size; i++) {
        auto child = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(newNode->linkSet[i]);
        child->parent = newNode->address;
        child->save();
    }
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
bool BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::canRedistribute(int curAdd, int sibAdd) {
    if(sibAdd == 0) return false;
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(curAdd);
    auto sib = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(sibAdd);
    if(cur->parent != sib->parent) return false;
    return sib->size > ((sib->m - !sib->leaf) / 2);
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
void BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::redistribute(int curAdd, int sibAdd) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(curAdd);
    auto sib = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(sibAdd);
    auto parent = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->parent);
    std::string k;
    int pos;
    for(pos=0; pos<parent->size; pos++) {
        if((parent->linkSet[pos] == curAdd && parent->linkSet[pos+1] == sibAdd)
           || (parent->linkSet[pos] == sibAdd && parent->linkSet[pos+1] == curAdd)) {
            k = parent->keySet[pos];
            break;
        }
    }

    if(cur->left == sibAdd) {
        if(cur->leaf) {
            cur->linkSet[cur->insert(sib->keySet[sib->size-1])] = sib->linkSet[sib->size-1];
            parent->keySet[pos] = cur->keySet[0];
        } else{
            cur->linkSet[cur->insert(k)] = sib->linkSet[sib->size];
            parent->keySet[pos] = sib->keySet[sib->size-1];
        }
        if(!cur->leaf) {
            BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE> *child = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(sib->linkSet[sib->size - cur->leaf]);
            child->parent = cur->address;
            child->save();
        }
        sib->size--;
    } else {
        if(cur->leaf) {
            cur->linkSet[cur->insert(sib->keySet[0])] = sib->linkSet[0];
            for(int i=0; i<sib->size; i++) {
                sib->keySet[i] = sib->keySet[i+1];
                sib->linkSet[i] = sib->linkSet[i+1];
            }
            parent->keySet[pos] = sib->keySet[0];
        } else {
            if(cur->size != 0) {
                cur->linkSet[cur->insert(k)+1] = sib->linkSet[0];
            } else {
                cur->keySet[0] = k;
                cur->linkSet[1] = sib->linkSet[0];
                cur->size++;
            }
            parent->keySet[pos] = sib->keySet[0];
            if(!cur->leaf) {
                auto child = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(sib->linkSet[0]);
                child->parent = cur->address;
                child->save();
            }
            for(int i=0; i<sib->size; i++) {
                sib->keySet[i] = sib->keySet[i+1];
                sib->linkSet[i] = sib->linkSet[i+1];
            }
        }
        sib->size--;
    }

    cur->save();
    sib->save();
    parent->save();
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
int BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::findNode(const KT &key) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(root);
    while(!cur->leaf) {
        for(int i=0; i<cur->size; i++) {
            if(key < cur->keySet[i]) {
                cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->linkSet[i]);
                break;
            }
            if(i == cur->size - 1) {
                cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->linkSet[i + 1]);
                break;
            }
        }
    }
    return cur->address;
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
void BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::split(const KT &key, int address, int parentAdd, int curAdd) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(curAdd);

    cur->linkSet[cur->insert(key)] = address;

    auto lLeaf = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(PageManager::Instance().allocate());
    auto rLeaf = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(PageManager::Instance().allocate());

    int mid = (cur->m / 2);
    for(int i=0; i<mid; i++) lLeaf->linkSet[lLeaf->insert(cur->keySet[i])] = cur->linkSet[i];
    lLeaf->right = rLeaf->address;
    lLeaf->left = cur->left;
    for(int i=mid; i<cur->m; i++) rLeaf->linkSet[rLeaf->insert(cur->keySet[i])] = cur->linkSet[i];
    rLeaf->left = lLeaf->address;
    rLeaf->right = cur->right;

    if(cur->left) {
        auto curLeft = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->left);
        curLeft->right = lLeaf->address;
        curLeft->save();
    }

    if(cur->right) {
        auto curRight = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->right);
        curRight->left = rLeaf->address;
        curRight->save();
    }

    cur->release();

    if(cur->address == root) {
        auto newRoot = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(PageManager::Instance().allocate());
        newRoot->insert(rLeaf->keySet[0]);
        newRoot->linkSet[0] = lLeaf->address;
        newRoot->linkSet[1] = rLeaf->address;
        newRoot->leaf = false;
        root = newRoot->address;
        newRoot->parent = 0;
        lLeaf->parent = rLeaf->parent = root;

        newRoot->save();
        lLeaf->save();
        rLeaf->save();
    } else {
        lLeaf->save();
        rLeaf->save();
        insertInternal(rLeaf->keySet[0], cur->parent, lLeaf->address, rLeaf->address);
    }
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
void BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::insertInternal(const KT &key, int curAdd, int lLeafAdd, int rLeafAdd) {
    BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE> *cur = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(curAdd);
    BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE> *lLeaf = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(lLeafAdd);
    BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE> *rLeaf = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(rLeafAdd);

    if(cur->size < cur->m - 1) {
        int pos = cur->insert(key);
        cur->linkSet[pos] = lLeaf->address;
        cur->linkSet[pos+1] = rLeaf->address;
        lLeaf->parent = rLeaf->parent = curAdd;
        cur->save();
        lLeaf->save();
        rLeaf->save();
        return;
    }

    auto newLChild = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(PageManager::Instance().allocate());
    auto newRChild = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(PageManager::Instance().allocate());
    newLChild->leaf = false;
    newRChild->leaf = false;
    newLChild->right = newRChild->address;
    newLChild->left = cur->left;
    newRChild->left = newLChild->address;
    newRChild->right = cur->right;

    if(cur->left) {
        auto curLeft = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->left);
        curLeft->right = newLChild->address;
        curLeft->save();
    }

    if(cur->right) {
        auto curRight = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(cur->right);
        curRight->left = newRChild->address;
        curRight->save();
    }

    int pos = cur->insert(key);
    cur->linkSet[pos] = lLeaf->address;
    cur->linkSet[pos+1] = rLeaf->address;

    int mid = cur->size / 2;

    for(int i=0; i<mid; i++) newLChild->insert(cur->keySet[i]);
    for(int i=0; i<=mid; i++) newLChild->linkSet[i] = cur->linkSet[i];

    for(int i=mid+1; i<cur->m; i++) newRChild->insert(cur->keySet[i]);
    for(int i=mid+1; i<=cur->m; i++) newRChild->linkSet[i-mid-1] = cur->linkSet[i];

    for(int i=0; i<=newLChild->size; i++) {
        auto child = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(newLChild->linkSet[i]);
        child->parent = newLChild->address;
        child->save();
    }
    for(int i=0; i<=newRChild->size; i++) {
        auto child = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(newRChild->linkSet[i]);
        child->parent = newRChild->address;
        child->save();
    }

    cur->release();

    if(cur->address == root) {
        auto newRoot = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(PageManager::Instance().allocate());
        newRoot->insert(cur->keySet[mid]);
        newRoot->linkSet[0] = newLChild->address;
        newRoot->linkSet[1] = newRChild->address;
        newRoot->leaf = false;
        root = newRoot->address;
        newRoot->parent = 0;
        newLChild->parent = newRChild->parent = root;

        newRoot->save();
        newLChild->save();
        newRChild->save();
    } else {
        newLChild->save();
        newRChild->save();
        insertInternal(cur->keySet[mid], cur->parent, newLChild->address, newRChild->address);
    }
}

template<int M_SIZE, typename KT, int K_SIZE, typename VT, int V_SIZE>
int BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNodeSize() {
    auto p = BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>::getNode(root);
    return p->save();
}

// BTree<M_SIZE, KT, K_SIZE, VT, V_SIZE> BTreeNode<M_SIZE, KT, K_SIZE, VT, V_SIZE>

#endif //INVODB_BTREE_H
