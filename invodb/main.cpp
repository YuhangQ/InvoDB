//
// Created by YuhangQ on 2021/9/24.
//

#include "main.h"

int main() {

    PageManager::loadDatabase("test.invodb");
    Collection::loadCollections();

    PageManager& manager = PageManager::Instance();

    //Collection::createCollection("hello");
    Collection &col = Collection::getCollection("hello");

    JSON json("{\"hello\": 1}");
    col.insert(json);

    return 0;
}