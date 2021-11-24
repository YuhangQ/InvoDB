//
// Created on 2021/10/23.
//

#ifndef INVODB_UUID_H
#define INVODB_UUID_H

#include <iostream>
#include <string>

inline std::string generateUUID() {
    char uuid[33]; uuid[32] = '\0';
    for(int i=0; i<32; i++) {
        int randn = rand() % 36;
        uuid[i] = (randn < 26 ? ('a' + randn) : ('0' + (randn - 26)));
    }
    return std::string(uuid, 32);
}

#endif //INVODB_UUID_H
