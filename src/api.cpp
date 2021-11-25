//
// Created on 2021/9/24.
//

#include "api.h"

#include <napi.h>

using namespace Napi;


void database(const CallbackInfo& info) {
    std::string filename = info[0].As<String>();
    PageManager::loadDatabase(filename.c_str());
    Collection::loadCollections();
}

Boolean exists(const CallbackInfo& info) {
    std::string colName = info[0].As<String>();
    return Boolean::New(info.Env(), Collection::existsCollection(colName));
}

void create(const CallbackInfo& info) {
    std::string colName = info[0].As<String>();
    Collection::createCollection(colName);
}

void insert(const CallbackInfo& info) {
    std::string colName = info[0].As<String>();
    std::string json = info[1].As<String>();

    auto j = nlohmann::json::parse(json);
    Collection::getCollection(colName).insert(j);
}

Array query(const CallbackInfo& info) {
    std::string colName = info[0].As<String>();
    std::string json = info[1].As<String>();

    auto j = nlohmann::json::parse(json);
    auto result = Collection::getCollection(colName).query(j);

    auto res = Array::New(info.Env(), result.size());
    for(int i=0; i<result.size(); i++) {
        res.Set(i, String::New(info.Env(), result[i].dump()));
    }

    return res;    
}

void _remove(const CallbackInfo& info) {
    std::string colName = info[0].As<String>();
    std::string json = info[1].As<String>();

    auto j = nlohmann::json::parse(json);
    Collection::getCollection(colName).remove(j);
}

Object Init(Env env, Object exports) {
    srand(time(0));
    exports.Set("database", Function::New(env, database));
    exports.Set("exists", Function::New(env, exists));
    exports.Set("create", Function::New(env, create));
    exports.Set("insert", Function::New(env, insert));
    exports.Set("query", Function::New(env, query));
    exports.Set("remove", Function::New(env, _remove));
    return exports;
}
NODE_API_MODULE(core, Init)
