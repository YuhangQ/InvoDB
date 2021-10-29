//
// Created by YuhangQ on 2021/10/25.
//

#include "btree_uuid.h"

BTreeUUID::BTreeUUID(const int& address) {
    root = address;
}

int BTreeUUID::find(const std::string& uuid) {
    NodeUUID* cur = NodeUUID::getNode(findNode(uuid));
    for(int i=0; i<cur->size; i++) {
        if(uuid == cur->key[i]) return cur->val[i];
    }
    return -1;
}

int BTreeUUID::findNode(const std::string& uuid) {
    NodeUUID* cur = NodeUUID::getNode(root);
    while(!cur->leaf) {
        for(int i=0; i<cur->size; i++) {
            if(uuid < cur->key[i]) {
                cur = NodeUUID::getNode(cur->val[i]);
                break;
            }
            if(i == cur->size - 1) {
                cur = NodeUUID::getNode(cur->val[i + 1]);
                break;
            }
        }
    }
    return cur->address;
}

void BTreeUUID::update(const std::string &uuid, int address) {
    if(find(uuid) == -1) {
        throw "key doesn't exists.";
    }
    NodeUUID* cur = NodeUUID::getNode(findNode(uuid));
    cur->val[cur->findPos(uuid)] = address;
    cur->save();
}

void BTreeUUID::insert(const std::string& uuid, int address) {
    if(find(uuid) != -1) {
        throw "key already exists.";
    }

    n_size++;

    NodeUUID* cur = NodeUUID::getNode(findNode(uuid));

    // insert directly
    if(cur->size < cur->m - 1) {
        cur->val[cur->insert(uuid)] = address;
        cur->save();
        return;
    }

    // split
    split(uuid, address, cur->parent, cur->address);
}

void BTreeUUID::split(const std::string& uuid, int address, int parentAddr, int curAddr) {

    NodeUUID* cur = NodeUUID::getNode(curAddr);

    cur->val[cur->insert(uuid)] = address;

    NodeUUID* lLeaf = NodeUUID::getNode(PageManager::Instance().allocate());
    NodeUUID* rLeaf = NodeUUID::getNode(PageManager::Instance().allocate());

    int mid = (cur->m / 2);
    for(int i=0; i<mid; i++) lLeaf->val[lLeaf->insert(cur->key[i])] = cur->val[i];
    lLeaf->right = rLeaf->address;
    lLeaf->left = cur->left;
    for(int i=mid; i<cur->m; i++) rLeaf->val[rLeaf->insert(cur->key[i])] = cur->val[i];
    rLeaf->left = lLeaf->address;
    rLeaf->right = cur->right;

    if(cur->left) {
        NodeUUID* curLeft = NodeUUID::getNode(cur->left);
        curLeft->right = lLeaf->address;
        curLeft->save();
    }

    if(cur->right) {
        NodeUUID* curRight = NodeUUID::getNode(cur->right);
        curRight->left = rLeaf->address;
        curRight->save();
    }

    cur->release();

    if(cur->address == root) {
        NodeUUID* newRoot = NodeUUID::getNode(PageManager::Instance().allocate());
        newRoot->insert(rLeaf->key[0]);
        newRoot->val[0] = lLeaf->address;
        newRoot->val[1] = rLeaf->address;
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
        insertInternal(rLeaf->key[0], cur->parent, lLeaf->address, rLeaf->address);
    }
}

void BTreeUUID::insertInternal(const std::string& uuid, int curAddr, int lLeafAddr, int rLeafAddr) {

    NodeUUID *cur = NodeUUID::getNode(curAddr);
    NodeUUID *lLeaf = NodeUUID::getNode(lLeafAddr);
    NodeUUID *rLeaf = NodeUUID::getNode(rLeafAddr);

    if(cur->size < cur->m - 1) {
        int pos = cur->insert(uuid);
        cur->val[pos] = lLeaf->address;
        cur->val[pos+1] = rLeaf->address;
        lLeaf->parent = rLeaf->parent = curAddr;
        cur->save();
        lLeaf->save();
        rLeaf->save();
        return;
    }

    NodeUUID* newLChild = NodeUUID::getNode(PageManager::Instance().allocate());
    NodeUUID* newRChild = NodeUUID::getNode(PageManager::Instance().allocate());
    newLChild->leaf = false;
    newRChild->leaf = false;
    newLChild->right = newRChild->address;
    newLChild->left = cur->left;
    newRChild->left = newLChild->address;
    newRChild->right = cur->right;

    if(cur->left) {
        NodeUUID* curLeft = NodeUUID::getNode(cur->left);
        curLeft->right = newLChild->address;
        curLeft->save();
    }

    if(cur->right) {
        NodeUUID* curRight = NodeUUID::getNode(cur->right);
        curRight->left = newRChild->address;
        curRight->save();
    }

    int pos = cur->insert(uuid);
    cur->val[pos] = lLeaf->address;
    cur->val[pos+1] = rLeaf->address;

    int mid = cur->size / 2;

    for(int i=0; i<mid; i++) newLChild->insert(cur->key[i]);
    for(int i=0; i<=mid; i++) newLChild->val[i] = cur->val[i];

    for(int i=mid+1; i<cur->m; i++) newRChild->insert(cur->key[i]);
    for(int i=mid+1; i<=cur->m; i++) newRChild->val[i-mid-1] = cur->val[i];

    for(int i=0; i<=newLChild->size; i++) {
        NodeUUID* child = NodeUUID::getNode(newLChild->val[i]);
        child->parent = newLChild->address;
        child->save();
    }
    for(int i=0; i<=newRChild->size; i++) {
        NodeUUID* child = NodeUUID::getNode(newRChild->val[i]);
        child->parent = newRChild->address;
        child->save();
    }

    cur->release();

    if(cur->address == root) {
        NodeUUID* newRoot = NodeUUID::getNode(PageManager::Instance().allocate());
        newRoot->insert(cur->key[mid]);
        newRoot->val[0] = newLChild->address;
        newRoot->val[1] = newRChild->address;
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
        insertInternal(cur->key[mid], cur->parent, newLChild->address, newRChild->address);
    }
}

/*
void BTreeUUID::print() {
    innerPrint(NodeUUID::getNode(root));
}

void BTreeUUID::innerPrint(NodeUUID *cur) {
    if(cur->address == root) {
        cnt = 0;
    }
    if(cur->leaf) cnt += cur->size;
    printf("---------%d(%d)count=%d&sum=%d---l:%d,r:%d-parent:%d----\n", cur->address, cur->leaf, cur->size, cnt, cur->left, cur->right, cur->parent);
    for(int i=0; i<cur->size; i++) {
        printf("%d:%s ", i, cur->key[i].substr(0, 6).c_str());
    }
    printf("\n");
    for(int i=0; i<=cur->size; i++) {
        printf("%d:%d ", i, cur->val[i]);
    }
    printf("\n");


    if(cur->leaf) return;
    for(int i=0; i<=cur->size; i++) {

        if(NodeUUID::getNode(cur->val[i])->parent != cur->address) {
            printf("FUCK\n");
            //exit(0);
        }
        innerPrint(NodeUUID::getNode(cur->val[i]));
    }

}
 */

void BTreeUUID::remove(const std::string &uuid) {
    if(find(uuid) == -1) {
        throw "key doesn't exists.";
    }
    n_size--;
    NodeUUID* cur = NodeUUID::getNode(findNode(uuid));
    removeEntry(cur->address, uuid, find(uuid));
}

void BTreeUUID::removeEntry(int curAddr, const std::string& uuid, const int& pointer) {
    //printf("---removeEntry: %d %s %d\n", curAddr, uuid.c_str(), pointer);

    NodeUUID* cur = NodeUUID::getNode(curAddr);
    int pos = cur->findPos(uuid);
    if(pos == -1) return;
    for(int i=pos; i<cur->size-1; i++) {
        cur->key[i] = cur->key[i + 1];
    }
    for(int i=pos+(cur->val[pos] != pointer); i<cur->size; i++) {
        cur->val[i] = cur->val[i + 1];
    }
    cur->size--;
    cur->save();

    if(curAddr == root && !cur->leaf && cur->size == 0) {
        root = cur->val[0];
        NodeUUID* root = NodeUUID::getNode(cur->val[0]);
        root->parent = 0;
        root->save();

        cur->release();
        return;
    }

    if(cur->enough() || cur->address == root) return;


    //printf("em %d %d\n", cur->address, cur->size);


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


bool BTreeUUID::canCoalesce(int curAddr, int sibAddr) {
    if(sibAddr == 0) return false;
    NodeUUID* cur = NodeUUID::getNode(curAddr);
    NodeUUID* sib = NodeUUID::getNode(sibAddr);
    if(cur->parent != sib->parent) return false;
    return (cur->size + sib->size <= NodeUUID::m - 1 - !cur->leaf);
}

void BTreeUUID::coalesce(int curAddr, int sibAddr) {

    //printf("coalesce %d and %d\n", curAddr, sibAddr);

    NodeUUID* cur = NodeUUID::getNode(curAddr);
    NodeUUID* sib = NodeUUID::getNode(sibAddr);
    NodeUUID* parent = NodeUUID::getNode(cur->parent);
    std::string *k;
    for(int i=0; i<parent->size; i++) {
        if((parent->val[i] == curAddr && parent->val[i+1] == sibAddr)
        || (parent->val[i] == sibAddr && parent->val[i+1] == curAddr)) {
            k = &parent->key[i];
            break;
        }
    }
    NodeUUID* newNode = nullptr;
    if(cur->left == sibAddr) {
        if(!cur->leaf) sib->insert(*k);
        for(int i=0; i<cur->size; i++) {
            sib->val[sib->insert(cur->key[i])] = cur->val[i];
        }
        sib->val[sib->size] = cur->val[cur->size];
        sib->right = cur->right;
        if(cur->right) {
            NodeUUID *right = NodeUUID::getNode(cur->right);
            right->left = sib->address;
            right->save();
        }
        newNode = sib;
        newNode->save();
        removeEntry(parent->address, *k, curAddr);
        cur->release();
    } else {
        if(!cur->leaf) cur->insert(*k);
        for(int i=0; i<sib->size; i++) {
            cur->val[cur->insert(sib->key[i])] = sib->val[i];
        }
        cur->val[cur->size] = sib->val[sib->size];

        cur->right = sib->right;

        if(sib->right) {
            NodeUUID *right = NodeUUID::getNode(sib->right);
            right->left = cur->address;
            right->save();
        }
        newNode = cur;
        newNode->save();
        removeEntry(parent->address, *k, sibAddr);
        sib->release();
    }
    if(newNode->leaf) return;
    for(int i=0; i<=newNode->size; i++) {
        NodeUUID* child = NodeUUID::getNode(newNode->val[i]);
        child->parent = newNode->address;
        child->save();
    }
}

bool BTreeUUID::canRedistribute(int curAddr, int sibAddr) {
    if(sibAddr == 0) return false;
    NodeUUID* cur = NodeUUID::getNode(curAddr);
    NodeUUID* sib = NodeUUID::getNode(sibAddr);
    if(cur->parent != sib->parent) return false;
    return sib->size > ((sib->m - !sib->leaf) / 2);
}

void BTreeUUID::redistribute(int curAddr, int sibAddr) {

    //printf("redistribute %d from %d\n", curAddr, sibAddr);

    NodeUUID* cur = NodeUUID::getNode(curAddr);
    NodeUUID* sib = NodeUUID::getNode(sibAddr);
    NodeUUID* parent = NodeUUID::getNode(cur->parent);
    std::string k;
    int pos;
    for(pos=0; pos<parent->size; pos++) {
        if((parent->val[pos] == curAddr && parent->val[pos+1] == sibAddr)
           || (parent->val[pos] == sibAddr && parent->val[pos+1] == curAddr)) {
            k = parent->key[pos];
            break;
        }
    }

    if(cur->left == sibAddr) {
        if(cur->leaf) {
            cur->val[cur->insert(sib->key[sib->size-1])] = sib->val[sib->size-1];
            parent->key[pos] = cur->key[0];
        } else{
            cur->val[cur->insert(k)] = sib->val[sib->size];
            parent->key[pos] = sib->key[sib->size-1];
        }
        if(!cur->leaf) {
            NodeUUID *child = NodeUUID::getNode(sib->val[sib->size - cur->leaf]);
            child->parent = cur->address;
            child->save();
        }
        sib->size--;
    } else {
        if(cur->leaf) {
            cur->val[cur->insert(sib->key[0])] = sib->val[0];
            for(int i=0; i<sib->size; i++) {
                sib->key[i] = sib->key[i+1];
                sib->val[i] = sib->val[i+1];
            }
            parent->key[pos] = sib->key[0];
        } else {
            if(cur->size != 0) {
                cur->val[cur->insert(k)+1] = sib->val[0];
            } else {
                cur->key[0] = k;
                cur->val[1] = sib->val[0];
                cur->size++;
            }
            parent->key[pos] = sib->key[0];
            if(!cur->leaf) {
                NodeUUID* child = NodeUUID::getNode(sib->val[0]);
                child->parent = cur->address;
                child->save();
            }
            for(int i=0; i<sib->size; i++) {
                sib->key[i] = sib->key[i+1];
                sib->val[i] = sib->val[i+1];
            }
        }
        sib->size--;
    }

    cur->save();
    sib->save();
    parent->save();
}

int BTreeUUID::size() {
    return n_size;
}

void BTreeUUID::testAndBenchmark(const int& n) {

    clock_t start = clock();

    std::map<std::string, int> map;

    for(int i=0; i<n; i++) {
        int opt = rand() % 4;
        // insert
        if(opt <= 1) {
            std::string uuid = generateUUID();
            int addr = rand();
            insert(uuid, addr);
            map[uuid] = addr;
        }
        // update
        else if(opt == 2) {
            if(map.size() == 0) continue;
            auto it = map.begin();
            std::advance(it, rand() % map.size());
            std::string uuid = it->first;
            int addr = rand();
            map[uuid] = addr;
            update(uuid, addr);
        }
        // remove
        else {
            if(map.size() == 0) continue;
            auto it = map.begin();
            std::advance(it, rand() % map.size());
            std::string uuid = it->first;
            map.erase(uuid);
            remove(uuid);
        }
    }

    if(map.size() != size()) {
        printf("%d %d\n", map.size(), size());
        printf("BTree has BUG!\n");
        exit(0);
    }

    for(auto it=map.begin(); it != map.end(); it++) {
        if(find(it->first) != it->second) {
            printf("BTree has BUG!\n");
            exit(0);
        }
    }

    clock_t end   = clock();

    printf("BTree pass the test with n=%d, time=%fs!\n", n, (double)(end - start) / CLOCKS_PER_SEC);
}


