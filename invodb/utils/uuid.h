//
// Created by YuhangQ on 2021/10/23.
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

inline std::string appropriateString(const std::string& s, const int& offset) {
    int a[s.size()];
    for(int i=0; i<s.size(); i++) a[i] = s[s.size()-i-1];
    a[0] += offset;
    for(int i=0; i<s.size(); i++) {
        if(a[i] <= 0) {
            a[i] += 256;
            a[i+1]--;
        }
    }
    for(int i=0; i<s.size(); i++) {
        printf("%c ", a[i]);
    }
    printf("\n");
    std::string res;
    for(int i=s.size()-1; i>=0; i--) {
        res += a[i];
    }
    return res;
}

#endif //INVODB_UUID_H
