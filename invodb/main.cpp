//
// Created by YuhangQ on 2021/9/24.
//

#include "main.h"

int main() {
    int t = time(0);
    //srand(1635418590);
    srand(t);
    printf("seed: %d\n", t);

    system("rm -rf test.invodb && touch test.invodb");

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

    BTreeUUID *btree = new BTreeUUID(PageManager::Instance().allocate());
    btree->testAndBenchmark(100000);

    return 0;
}