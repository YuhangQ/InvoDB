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

    std::vector<std::string> v;

    for(int i=0; i<10000; i++) {
        generateUUID(uuid);
        v.push_back(std::string(uuid, 32));
        btree->insert(uuid, PageManager::Instance().allocate());
    }

    btree->print();

    printf("%d\n", btree->find("123"));

    return 0;
}