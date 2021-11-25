#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "../collection/collection.h"
#include "../btree/list.h"

Collection *col;

void terminal() {

    srand(time(0));
    PageManager::loadDatabase("test.invodb");
    Collection::loadCollections();

    if (!Collection::existsCollection("test"))
        Collection::createCollection("test");
    col = &Collection::getCollection("test");

    printf("--------INVODB TERMINAL--------\n");
    printf("insert\t<JSON>\tinsert a one line json to database.\n");
    printf("query\t<JSON>\tquery all jsons satisfying the query json.\n");
    printf("-------------------------------\n");

    while(true) {
        std::string input;
        printf("INVODB > ");
        std::getline(std::cin, input);
        if(input.size() == 0) continue;
        if(input.find("insert ") == 0) {
            input = input.substr(7, input.size());

            nlohmann::json json;
            try {
                json = nlohmann::json::parse(input);
            } catch(const char *err) {
                printf("ERROR: %s\n", err);
            } catch(...) {
                printf("ERROR: your insert input isn't a json.\n");
                continue;
            }

            try {
                col->insert(json);
            } catch(const char *err) {
                printf("ERROR: %s\n", err);
            } catch(...) {
                printf("ERROR: insert failed. check your input.\n");
                continue;
            }
            //printf("You insert json: %s\n", json.dump().c_str());

        } else if(input.find("query ") == 0) {
            input = input.substr(6, input.size());
            nlohmann::json json;
            try {
                json = nlohmann::json::parse(input);
            } catch(const char *err) {
                printf("ERROR: %s\n", err);
            } catch(...) {
                printf("ERROR: your query input isn't a json.\n");
                continue;
            }

            try {
                auto res = col->query(json);
                printf("query result: \n");
            for(auto& j : res) {
                printf(" - %s\n", j.dump().c_str());
            }
            } catch(const char *err) {
                printf("ERROR: %s\n", err);
            } catch(...) {
                printf("ERROR: query failed. check your input.\n");
                continue;
            }
        }
    }
}


void testAndBenchmark(int n) {

    auto btree = new BTree<std::string, 128>(PageManager::allocate());

    printf("nodeSize: %d\n", btree->getNodeSize());

    clock_t start = clock();

    std::map<std::string, int> map;

    for(int i=0; i<n; i++) {
        int opt = rand() % 4;

        if(i%(n/100) == 0) {
            printf("[%d/%d] eeeeee\n", i, n);
        }

        // insert
        if(opt <= 1) {
            std::string uuid = generateUUID();
            int addr = rand();
            btree->insert(uuid, addr);
            map[uuid] = addr;
        }
            // update
        else if(opt == 2) {
            if(map.size() == 0) continue;
            auto it = map.begin();
            std::advance(it, rand() % map.size());
            std::string uuid = it->first;
            double addr = rand();
            map[uuid] = addr;
            btree->update(uuid, addr);
        }
            // remove
        else {
            if(map.size() == 0) continue;
            auto it = map.begin();
            std::advance(it, rand() % map.size());
            std::string uuid = it->first;
            map.erase(uuid);
            btree->remove(uuid);
        }

        //printf("opt: %d\n", opt);
        //btree->print();
    }

    if(map.size() != btree->size()) {
        printf("%d %d\n", map.size(), btree->size());
        printf("BTree has BUG!\n");
        exit(0);
    }

    printf("test res k-v: %d\n", map.size());

    for(auto it=map.begin(); it != map.end(); it++) {
        if(btree->find(it->first) != it->second) {
            printf("BTree has BUG!\n");
            exit(0);
        }
    }

    clock_t end = clock();

    printf("BTree pass the test with n=%d, time=%fs!\n", n, (double)(end - start) / CLOCKS_PER_SEC);
}
