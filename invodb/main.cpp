//
// Created by YuhangQ on 2021/9/24.
//

#include "main.h"


void testAndBenchmark(int n);

int main() {
    int t = time(0);
    //srand(1635418590);
    srand(t);
    printf("seed: %d\n", t);

    //system("rm -rf test.invodb && touch test.invodb");

    PageManager::loadDatabase("test.invodb");
    Collection::loadCollections();

    PageManager& manager = PageManager::Instance();


    Collection *col;
    try {
       col = &Collection::getCollection("hello");
    } catch(const char *error) {
        Collection::createCollection("hello");
    }

    JSON json("{\"hello\": 1}");
    col->insert(json);

    testAndBenchmark(100000);

    //btree->testAndBenchmark(100000);

    return 0;
}

void testAndBenchmark(int n) {

    auto btree = new BTree<27, std::string, 32>(PageManager::Instance().allocate());

    printf("nodeSize: %d\n", btree->getNodeSize());

    clock_t start = clock();

    std::map<std::string, int> map;

    for(int i=0; i<n; i++) {
        int opt = rand() % 4;
        // insert
        if(opt <= 1) {
            std::string uuid = generateUUID();
            int addr = rand();
            btree->insert(uuid, addr);
            map[uuid] = addr;
        }
            // update
        else if(opt == 2) {
            if(map.size() == 0) continue;
            auto it = map.begin();
            std::advance(it, rand() % map.size());
            std::string uuid = it->first;
            double addr = rand();
            map[uuid] = addr;
            btree->update(uuid, addr);
        }
            // remove
        else {
            if(map.size() == 0) continue;
            auto it = map.begin();
            std::advance(it, rand() % map.size());
            std::string uuid = it->first;
            map.erase(uuid);
            btree->remove(uuid);
        }
    }

    if(map.size() != btree->size()) {
        printf("%d %d\n", map.size(), btree->size());
        printf("BTree has BUG!\n");
        exit(0);
    }

    printf("test res k-v: %d\n", map.size());

    for(auto it=map.begin(); it != map.end(); it++) {
        if(btree->find(it->first) != it->second) {
            printf("BTree has BUG!\n");
            exit(0);
        }
    }

    clock_t end = clock();

    printf("BTree pass the test with n=%d, time=%fs!\n", n, (double)(end - start) / CLOCKS_PER_SEC);
}
