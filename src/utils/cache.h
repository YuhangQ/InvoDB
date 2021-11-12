//
// Created by i on 2021/11/2.
//

#ifndef INVODBCACHEH
#define INVODBCACHEH

#include <list>
#include <map>
#include "../file/storage_page.h"

template<typename KT, typename VT>
class LRUCache {
public:
    LRUCache(int capacity) : capacity(capacity) {}

    bool exist(KT const &key) {
        return hash.find(key) != hash.end();
    }

    void remove(KT const &key) {
        ls.erase(hash[key]);
        hash.erase(key);
    }

    VT get(KT const &key) {
        if (hash.find(key) == hash.end())
            throw "cache error";
        else {
            VT value = hash[key]->second;
            ls.erase(hash[key]);
            ls.push_front(std::make_pair(key, value));
            hash[key] = ls.begin();
            return value;
        }
    }

    VT put(KT const &key, VT const &value) {
        VT res;
        if (hash.find(key) != hash.end()) {
            ls.erase(hash[key]);
        }
        else if (ls.size() >= capacity) {
            res = ls.back().second;
            printf("fuck\n");
            exit(0);
            hash.erase(ls.back().first);
            ls.pop_back();
        }
        ls.push_front(std::make_pair(key, value));
        hash[key] = ls.begin();
        return res;
    }

private:
    int capacity;
    std::list<std::pair<KT, VT>> ls;
    std::unordered_map<KT, typename std::list<std::pair<KT, VT>>::iterator> hash;
};



#endif //INVODBCACHEH
