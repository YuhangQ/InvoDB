//
// Created on 2021/10/25.
//

#ifndef INVODB_BTREE_H
#define INVODB_BTREE_H

#include "node.h"
#include "../utils/uuid.h"

#define M_SIZE 1000 / (K_SIZE + 4)

template<typename KT, int K_SIZE>
class BTree {
public:

    static std::shared_ptr<BTreeNode<M_SIZE, KT, K_SIZE>> getNode(const int &index);

    BTree(const int& address);
    void insert(const KT &key, const int &value);
    void update(const KT &key, const int &value);
    void remove(const KT &key);
    bool exists(const KT &key);
    int getNodeSize();
    int find(const KT &key);
    int findNode(const KT &key);
    int firstNode();
    std::vector<KT> keySet();
    std::vector<std::pair<KT, int>> all();
    void print();
    void innerPrint(const int& t);
    int size();
private:

    void removeEntry(int curAdd, const KT& key, const int& pointer);
    bool canCoalesce(int curAdd, int sibAdd);
    void coalesce(int curAdd, int sibAdd);
    bool canRedistribute(int curAdd, int sibAdd);
    void redistribute(int curAdd, int sibAdd);
    void split(const KT &key, int address, int parentAdd, int curAdd);
    void insertInternal(const KT &key, int curAdd, int lLeafAdd, int rLeafAdd);
    int root;
    int n_size;

};


template<typename KT, int K_SIZE>
std::shared_ptr<BTreeNode<M_SIZE, KT, K_SIZE>> BTree<KT, K_SIZE>::getNode(const int &index) {
    return BTreeNode<M_SIZE, KT, K_SIZE>::getNode(index);
}

template<typename KT, int K_SIZE>
BTree<KT, K_SIZE>::BTree(const int& address) {
    root = address;
    n_size = 0;
}

template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::insert(const KT &key, const int &value) {

    if(exists(key)) {
        update(key, value);
        return;
    }

    n_size++;

    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(findNode(key));

    // insert directly
    if(cur->size < cur->m - 1) {
        cur->linkSet[cur->insert(key)] = value;
        cur->save();
        return;
    }

    // split
    split(key, value, cur->parent, cur->address);
}

template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::update(const KT &key, const int &value) {
    if(!exists(key)) {
        insert(key, value);
        return;
    }
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(findNode(key));
    cur->linkSet[cur->findPos(key)] = value;
    cur->save();
}

template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::remove(const KT &key) {
    if(!exists(key)) {
        return;
    }
    n_size--;
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(findNode(key));
    removeEntry(cur->address, key, find(key));
}

template<typename KT, int K_SIZE>
int BTree<KT, K_SIZE>::find(const KT &key) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(findNode(key));
    for(int i=0; i<cur->size; i++) {
        if(key == cur->keySet[i]) return cur->linkSet[i];
    }
    return -1;
}

template<typename KT, int K_SIZE>
int BTree<KT, K_SIZE>::size() {
    return n_size;
}

template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::removeEntry(int curAdd, const KT &key, const int &pointer) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(curAdd);
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
//        root = cur->linkSet[0];
//        auto root = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->linkSet[0]);
//        root->parent = 0;
//        root->save();
//        cur->release();
        auto rootNode = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->linkSet[0]);
        *cur =  *rootNode;
        rootNode->release();
        cur->parent = 0;
        cur->address = root;
        cur->update();
        cur->save();
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

template<typename KT, int K_SIZE>
bool BTree<KT, K_SIZE>::canCoalesce(int curAdd, int sibAdd) {
    if(sibAdd == 0) return false;
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(curAdd);
    auto sib = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(sibAdd);
    if(cur->parent != sib->parent) return false;
    return (cur->size + sib->size <= BTreeNode<M_SIZE, KT, K_SIZE>::m - 1 - !cur->leaf);
}

template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::coalesce(int curAdd, int sibAdd) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(curAdd);
    auto sib = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(sibAdd);
    auto parent = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->parent);
    KT *k;
    for(int i=0; i<parent->size; i++) {
        if((parent->linkSet[i] == curAdd && parent->linkSet[i+1] == sibAdd)
           || (parent->linkSet[i] == sibAdd && parent->linkSet[i+1] == curAdd)) {
            k = &parent->keySet[i];
            break;
        }
    }

    if(cur->left == sibAdd) {
        if(!cur->leaf) sib->insert(*k);
        for(int i=0; i<cur->size; i++) {
            sib->linkSet[sib->insert(cur->keySet[i])] = cur->linkSet[i];
        }
        sib->linkSet[sib->size] = cur->linkSet[cur->size];
        sib->right = cur->right;
        if(cur->right) {
            auto right = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->right);
            right->left = sib->address;
            right->save();
        }
        sib->update();
        sib->save();
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
            auto right = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(sib->right);
            right->left = cur->address;
            right->save();
        }
        cur->update();
        cur->save();

        removeEntry(parent->address, *k, sibAdd);
        sib->release();
    }
    /*
    if(newNode->leaf) return;
    for(int i=0; i<=newNode->size; i++) {
        auto child = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(newNode->linkSet[i]);
        child->parent = newNode->address;
        child->save();
    }
     */
}

template<typename KT, int K_SIZE>
bool BTree<KT, K_SIZE>::canRedistribute(int curAdd, int sibAdd) {
    if(sibAdd == 0) return false;
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(curAdd);
    auto sib = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(sibAdd);
    if(cur->parent != sib->parent) return false;
    return sib->size > ((sib->m - !sib->leaf) / 2);
}

template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::redistribute(int curAdd, int sibAdd) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(curAdd);
    auto sib = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(sibAdd);
    auto parent = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->parent);
    KT k;
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
            auto child = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(sib->linkSet[sib->size - cur->leaf]);
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
                auto child = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(sib->linkSet[0]);
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

template<typename KT, int K_SIZE>
int BTree<KT, K_SIZE>::findNode(const KT &key) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(root);
    while(!cur->leaf) {
        for(int i=0; i<cur->size; i++) {
            if(key < cur->keySet[i]) {
                cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->linkSet[i]);
                break;
            }
            if(i == cur->size - 1) {
                cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->linkSet[i + 1]);
                break;
            }
        }
    }
    return cur->address;
}

template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::split(const KT &key, int address, int parentAdd, int curAdd) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(curAdd);

    cur->linkSet[cur->insert(key)] = address;

    auto lLeaf = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(PageManager::allocate());
    auto rLeaf = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(PageManager::allocate());

    int mid = (cur->m / 2);
    for(int i=0; i<mid; i++) lLeaf->linkSet[lLeaf->insert(cur->keySet[i])] = cur->linkSet[i];
    lLeaf->right = rLeaf->address;
    lLeaf->left = cur->left;
    for(int i=mid; i<cur->m; i++) rLeaf->linkSet[rLeaf->insert(cur->keySet[i])] = cur->linkSet[i];
    rLeaf->left = lLeaf->address;
    rLeaf->right = cur->right;

    if(cur->left) {
        auto curLeft = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->left);
        curLeft->right = lLeaf->address;
        curLeft->save();
    }

    if(cur->right) {
        auto curRight = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->right);
        curRight->left = rLeaf->address;
        curRight->save();
    }



    if(cur->address == root) {
//        auto newRoot = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(PageManager::allocate());
//        newRoot->insert(rLeaf->keySet[0]);
//        newRoot->linkSet[0] = lLeaf->address;
//        newRoot->linkSet[1] = rLeaf->address;
//        newRoot->leaf = false;
//        root = newRoot->address;
//        newRoot->parent = 0;
//        lLeaf->parent = rLeaf->parent = root;
//
//        newRoot->save();
//        lLeaf->save();
//        rLeaf->save();

        auto newRoot = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(root);
        newRoot->clear();
        newRoot->insert(rLeaf->keySet[0]);
        newRoot->linkSet[0] = lLeaf->address;
        newRoot->linkSet[1] = rLeaf->address;
        newRoot->leaf = false;
        newRoot->parent = 0;
        lLeaf->parent = rLeaf->parent = root;

        newRoot->save();
        lLeaf->save();
        rLeaf->save();

    } else {
        lLeaf->save();
        rLeaf->save();
        insertInternal(rLeaf->keySet[0], cur->parent, lLeaf->address, rLeaf->address);
        cur->release();
    }
}

template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::print() {
    innerPrint(root);
}

template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::innerPrint(const int& t) {
    auto p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(t);
    printf("----------Node: %d----Parent: %d----------\n", p->address, p->parent);
    for(int i=0; i<p->size; i++) {
        std::cout << p->keySet[i] << " ";
    }
    std::cout << std::endl;
    for(int i=0; i<=p->size; i++) {
        std::cout << p->linkSet[i] << " ";
    }
    std::cout << std::endl;
    if(p->leaf) return;
    for(int i=0; i<=p->size; i++) {
        auto p2 = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(p->linkSet[i]);
        if(p2->parent != p->address) {
            printf(">>>>>>>>>>>>>>>>>>FUCK");
            exit(0);
        }

        innerPrint(p->linkSet[i]);
    }
}


template<typename KT, int K_SIZE>
void BTree<KT, K_SIZE>::insertInternal(const KT &key, int curAdd, int lLeafAdd, int rLeafAdd) {
    auto cur = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(curAdd);
    auto lLeaf = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(lLeafAdd);
    auto rLeaf = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(rLeafAdd);

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

    auto newLChild = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(PageManager::allocate());
    auto newRChild = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(PageManager::allocate());
    newLChild->leaf = false;
    newRChild->leaf = false;
    newLChild->right = newRChild->address;
    newLChild->left = cur->left;
    newRChild->left = newLChild->address;
    newRChild->right = cur->right;

    if(cur->left) {
        auto curLeft = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->left);
        curLeft->right = newLChild->address;
        curLeft->save();
    }

    if(cur->right) {
        auto curRight = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(cur->right);
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
        auto child = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(newLChild->linkSet[i]);
        child->parent = newLChild->address;
        child->save();
    }
    for(int i=0; i<=newRChild->size; i++) {
        auto child = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(newRChild->linkSet[i]);
        child->parent = newRChild->address;
        child->save();
    }



    if(cur->address == root) {
//        auto newRoot = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(PageManager::allocate());
//        newRoot->insert(cur->keySet[mid]);
//        newRoot->linkSet[0] = newLChild->address;
//        newRoot->linkSet[1] = newRChild->address;
//        newRoot->leaf = false;
//        root = newRoot->address;
//        newRoot->parent = 0;
//        newLChild->parent = newRChild->parent = root;
//
//        newRoot->save();
//        newLChild->save();
//        newRChild->save();

        KT key = cur->keySet[mid];

        auto newRoot = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(root);
        newRoot->clear();
        newRoot->insert(key);
        newRoot->linkSet[0] = newLChild->address;
        newRoot->linkSet[1] = newRChild->address;
        newRoot->leaf = false;
        newRoot->parent = 0;
        newLChild->parent = newRChild->parent = root;

        newRoot->save();
        newLChild->save();
        newRChild->save();

    } else {
        newLChild->save();
        newRChild->save();
        insertInternal(cur->keySet[mid], cur->parent, newLChild->address, newRChild->address);
        cur->release();
    }
}

template<typename KT, int K_SIZE>
int BTree<KT, K_SIZE>::getNodeSize() {
    auto p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(root);
    return p->save();
}

template<typename KT, int K_SIZE>
bool BTree<KT, K_SIZE>::exists(const KT &key) {
    return find(key) != -1;
}

template<typename KT, int K_SIZE>
std::vector<KT> BTree<KT, K_SIZE>::keySet() {
    auto p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(root);
    while(!p->leaf) {
        p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(p->linkSet[0]);
    }
    std::vector<KT> v;
    while(true) {
        for(int i=0; i<p->size; i++) {
            v.push_back(p->keySet[i]);
        }
        if(p->right == 0) break;
        p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(p->right);
    }
    return v;
}

template<typename KT, int K_SIZE>
std::vector<std::pair<KT, int>> BTree<KT, K_SIZE>::all() {
    auto p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(root);
    while(!p->leaf) {
        p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(p->linkSet[0]);
    }
    std::vector<std::pair<KT, int>> v;
    while(true) {
        for(int i=0; i<p->size; i++) {
            v.push_back(std::make_pair(p->keySet[i], p->linkSet[i]));
        }
        if(p->right == 0) break;
        p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(p->right);
    }
    return v;
}

template<typename KT, int K_SIZE>
int BTree<KT, K_SIZE>::firstNode() {
    auto p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(root);
    while(!p->leaf) {
        p = BTreeNode<M_SIZE, KT, K_SIZE>::getNode(p->linkSet[0]);
    }
    return p.address;
}


#endif //INVODB_BTREE_H
