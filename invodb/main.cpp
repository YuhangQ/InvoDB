//
// Created by YuhangQ on 2021/9/24.
//

#include "main.h"


void benchmark() {
    BTreeUUID *btree = new BTreeUUID(PageManager::Instance().allocate());
    char uuid[33]; uuid[32] = '\0';

    std::vector<std::pair<std::string, int>> v;

    const int n = 1000000;

    for(int i=0; i<n; i++) {
        generateUUID(uuid);
        int addr = PageManager::Instance().allocate();
        v.push_back(std::make_pair(uuid, addr));
        btree->insert(uuid, addr);
    }

    for(int i=0; i<1000000; i++) {
        std::swap(v[rand()%v.size()], v[rand()%v.size()]);
    }

    for(int i=0; i<v.size(); i++) {
        int addr = btree->find(v[0].first);
        if(addr != v[0].second) {
            printf("fuck\n");
            exit(0);
        }
    }
}

int main() {
    int t = time(0);
    //srand(1635418590);
    srand(1635423140);
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

    benchmark();

    return 0;
}