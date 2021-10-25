//
// Created by YuhangQ on 2021/9/24.
//

#include "main.h"

int main() {
    srand(time(NULL));

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
    char uuid[32];
    for(int i=0; i<100000; i++) {
        generateUUID(uuid);
        btree->insert(uuid, PageManager::Instance().allocate());
    }

    btree->print();

    return 0;
}