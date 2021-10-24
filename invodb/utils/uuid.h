//
// Created by YuhangQ on 2021/10/23.
//

#ifndef INVODB_UUID_H
#define INVODB_UUID_H

#include <iostream>
#include <string>

std::string generateUUID() {
    srand(time(NULL));
    std::string uuid;
    for(int i=0; i<32; i++) {
        int randn = rand() % 36;
        //0~35;
        uuid += (randn < 26 ? ('a' + randn) : ('0' + (randn - 26)));
    }
    std::cout << uuid << std::endl;
    return uuid;
}

#endif //INVODB_UUID_H
