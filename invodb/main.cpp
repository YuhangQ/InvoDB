//
// Created by YuhangQ on 2021/9/24.
//

#include "main.h"


int main() {
    PageManager& manager = PageManager::Instance();
    manager.loadDatabase("test.invodb");

    manager.getPage(0);

    return 0;
}