//
// Created by YuhangQ on 2021/10/23.
//

#ifndef INVODB_UUID_H
#define INVODB_UUID_H

#include <iostream>
#include <string>

void generateUUID(char *uuid) {
    srand(time(NULL));
    for(int i=0; i<32; i++) {
        int randn = rand() % 36;
        uuid[i] = (randn < 26 ? ('a' + randn) : ('0' + (randn - 26)));
    }
}

#endif //INVODB_UUID_H
