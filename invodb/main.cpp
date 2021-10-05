//
// Created by YuhangQ on 2021/9/24.
//

#include "main.h"


int main() {
    PageManager& manager = PageManager::Instance();
    manager.loadDatabase("test.invodb");

    StoragePage page = manager.getPage(1);

    for(int i=0; i<100; i++) printf("%c", page[i]);

    return 0;
}