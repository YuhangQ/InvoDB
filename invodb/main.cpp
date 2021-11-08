//
// Created by YuhangQ on 2021/9/24.
//

#include "main.h"


void testAndBenchmark(int n);

Collection *col;

void terminal() {
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
            } catch(...) {
                printf("ERROR: your insert input isn't a json.\n");
                continue;
            }
            col->insert(json);
            printf("You insert json: %s\n", json.dump().c_str());

        } else if(input.find("query ") == 0) {
            input = input.substr(6, input.size());
            nlohmann::json json;
            try {
                json = nlohmann::json::parse(input);
            } catch(...) {
                printf("ERROR: your query input isn't a json.\n");
                continue;
            }

            auto res = col->query(json);

            printf("query result: \n");
            for(auto& j : res) {
                printf(" - %s\n", j.dump().c_str());
            }
        }
    }
}

int main() {
    int t = time(0);
    srand(1635418590);
    //srand(t);
    printf("seed: %d\n", t);

    system("rm -rf test.invodb && touch test.invodb");

    PageManager::loadDatabase("test.invodb");

    Collection::loadCollections();

    PageManager& manager = PageManager::Instance();

    try {
       col = &Collection::getCollection("hello");
    } catch(const char *error) {
        Collection::createCollection("hello");
        col = &Collection::getCollection("hello");
    }

//    nlohmann::json j = nlohmann::json::parse(R"(
//{
//        "title" : "MongoDB 教程",
//        "description" : "MongoDB 是一个 Nosql 数据库",
//        "by" : "菜鸟教程",
//        "url" : "http://www.runoob.com",
//        "tags" : [
//                "mongodb",
//                "database",
//                "NoSQL"
//        ],
//        "likes" : 100
//}
//    )");
//
//    col->insert(j);
//
//    col->query(nlohmann::json::parse(R"(
//{
//	"likes": {"$gt":50},
//	"$or": [
//		{"by": "菜鸟教程"},
//		{"title": "MongoDB 教程"}
//	]
//}
//    )"));

    terminal();


//    freopen("qq.txt", "r", stdin);
//    const int n = 1000000;
//    char qq[100], phone[100];
//
//    clock_t start = clock();
//    for(int i=0; i<n; i++) {
//        scanf("%s%s", qq, phone);
//        nlohmann::json json;
//        json["qq"] = qq;
//        json["phone"] = phone;
//        col->insert(json);
//
//        if(i % (n/1000) == 0) {
//            printf("[%d/%d] time=%fs!\n", i, n, (double)(clock() - start) / CLOCKS_PER_SEC);
//            start = clock();
//        }
//    }

    //col->test();




    //testAndBenchmark(20000);

    return 0;
}

void testAndBenchmark(int n) {

    auto btree = new BTree<std::string, 128>(PageManager::Instance().allocate());

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
