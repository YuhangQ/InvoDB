//
// Created by projector-user on 11/1/21.
//

#include "collection.h"

std::vector<nlohmann::json> Collection::query(const nlohmann::json &json) {
    std::vector<nlohmann::json> res;
    for(auto& json : innerQuery("", json)) {
        res.push_back(json);
    }
    return res;
}

/*
 * range search mod explain
 * mod = 0:   minValue <= value <= maxValue
 * mod = 1:   minValue < value <= maxValue
 * mod = 2:   minValue <= value < maxValue
 * mod = 3:   minValue < value < maxValue
 * mod = 4:   value < maxValue
 * mod = 5:   value <= maxValue
 * mod = 6:   value > minValue
 * mod = 7:   value >= minValue
 */

std::set<nlohmann::json> Collection::queryRange(const std::string &prefix, const nlohmann::json &json) {

    //printf(">> queryRange  prefix: %s  query: %s\n", prefix.c_str(), json.dump().c_str());

    std::set<nlohmann::json> set;

    for(auto& [key, value] : json.items()) {
        if(value.is_number()) {

            if(json.contains("$ne")) {
                if(!index->exists(prefix+"$number")) {
                    return set;
                }
                BTree<double, 8> tree(index->find(prefix+"$number"));
                for(auto& key: tree.keySet()) {
                    if(key != json["$ne"].get<double>()) continue;
                    List<int, 4> list(tree.find(key));
                    for(auto& add : list.all()) {
                        set.insert(PageManager::Instance().readJSONFromFile(add));
                    }
                }
            }
            else if(json.contains("$gt")) {
                if(json.contains("$lt")) {
                    set = queryNumber(prefix, json["$gt"].get<double>(), json["$lt"].get<double>(), 3);
                } else if(json.contains("$lte")) {
                    set = queryNumber(prefix, json["$gt"].get<double>(), json["$lte"].get<double>(), 1);
                } else {

                    set = queryNumber(prefix, json["$gt"].get<double>(), 0, 6);

                }
            } else if(json.contains("$gte")) {
                if(json.contains("$lt")) {
                    set = queryNumber(prefix, json["$gte"].get<double>(), json["$lt"].get<double>(), 2);
                } else if(json.contains("$lte")) {
                    set = queryNumber(prefix, json["$gte"].get<double>(), json["$lte"].get<double>(), 0);
                } else {
                    set = queryNumber(prefix, json["$gte"].get<double>(), 0, 7);
                }
            } else if(json.contains("$lt")) {
                set = queryNumber(prefix, 0, json["$lt"].get<double>(), 4);
            } else if(json.contains("$lte")) {
                set = queryNumber(prefix, 0, json["$lte"].get<double>(), 5);
            }
            return set;
        }
        else if(value.is_string()) {

            if(json.contains("$ne")) {
                if(!index->exists(prefix+"$string")) {
                    return set;
                }
                BTree<std::string, 64> tree(index->find(prefix+"$string"));
                for(auto& key: tree.keySet()) {
                    if(key != json["$ne"].get<std::string>()) continue;
                    List<int, 4> list(tree.find(key));
                    for(auto& add : list.all()) {
                        set.insert(PageManager::Instance().readJSONFromFile(add));
                    }
                }
            }
            else if(json.contains("$gt")) {
                if(json.contains("$lt")) {
                    set = queryString(prefix, json["$gt"].get<std::string>(), json["$lt"].get<std::string>(), 3);
                } else if(json.contains("$lte")) {
                    set = queryString(prefix, json["$gt"].get<std::string>(), json["$lte"].get<std::string>(), 1);
                } else {
                    set = queryString(prefix, json["$gt"].get<std::string>(), "", 6);
                }
            } else if(json.contains("$gte")) {
                if(json.contains("$lt")) {
                    set = queryString(prefix, json["$gte"].get<std::string>(), json["$lt"].get<std::string>(), 2);
                } else if(json.contains("$lte")) {
                    set = queryString(prefix, json["$gte"].get<std::string>(), json["$lte"].get<std::string>(), 0);
                } else {
                    set = queryString(prefix, json["$gte"].get<std::string>(), "", 7);
                }
            } else if(json.contains("$lt")) {
                set = queryString(prefix, "", json["$lt"].get<std::string>(), 4);
            } else if(json.contains("$lte")) {
                set = queryString(prefix, "", json["$lte"].get<std::string>(), 5);
            }
            return set;
        }
    }

    return set;
}

std::set<nlohmann::json> Collection::innerQuery(const std::string &prefix, const nlohmann::json &json) {

    for(auto& [key, value] : json.items()) {
        if(key == "$lt" || key == "$lte" || key == "$gt" || key == "$gte" || key == "$ne") {
            return queryRange(prefix.substr(0, prefix.size()-1), json);
        }
    }

    std::set<nlohmann::json> res;
    bool init = true;

    for(auto& [key, value] : json.items()) {

        std::set<nlohmann::json> tmp;
        std::string tPrefix = prefix + key;

        if(key == "$or") {
            nlohmann::json line = json[key].get<nlohmann::json>();
            for(auto& obj : line) {
                tmp = setUnion(tmp, innerQuery(prefix, obj.get<nlohmann::json>()));
            }
        } else if(json[key].is_object()) {
            tmp = innerQuery(prefix + key + ".", json[key].get<nlohmann::json>());
        } else if(json[key].is_array()) {

        } else if(json[key].is_boolean()) {
            tmp = queryBool(tPrefix, json[key].get<bool>());
        } else if(json[key].is_string()) {
            tmp = queryString(tPrefix, json[key].get<std::string>(), json[key].get<std::string>());
        } else if(json[key].is_number()) {
            tmp = queryNumber(tPrefix, json[key].get<double>(), json[key].get<double>());
        }
        if(init) res = tmp, init = false;
        else res = setIntersection(res, tmp);
    }

    auto str = json.dump();

    printf("query: %s prefix: %s\n", str.c_str(), prefix.c_str());
    printf("result: \n");
    for(auto it=res.begin(); it!=res.end(); it++) {
        printf(" - %s\n", it->dump().c_str());
    }

    return res;
}

std::set<nlohmann::json>
Collection::setIntersection(const std::set<nlohmann::json> &a, const std::set<nlohmann::json> &b) {
    std::set<nlohmann::json> res;
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
    return res;
}

std::set<nlohmann::json>
Collection::setUnion(const std::set<nlohmann::json> &a, const std::set<nlohmann::json> &b) {
    std::set<nlohmann::json> res;
    std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(res, res.end()));
    return res;
}

/*
 * range search mod explain
 * mod = 0:   minValue <= value <= maxValue
 * mod = 1:   minValue < value <= maxValue
 * mod = 2:   minValue <= value < maxValue
 * mod = 3:   minValue < value < maxValue
 */

std::set<nlohmann::json>
Collection::queryString(const std::string &prefix, const std::string &minValue, const std::string &maxValue, const int &mod) {
    std::set<nlohmann::json> res;
    auto treeName = prefix + "$string";
    //printf(">>>> %s %s %s\n", prefix.c_str(), minValue.c_str(), maxValue.c_str());
    if(!index->exists(treeName)) {
        return res;
    }
    BTree<std::string, 64> tree(index->find(treeName));
    const int K_SIZE = 64;
    std::shared_ptr<BTreeNode<M_SIZE, std::string , K_SIZE>> node;

    if(mod == 4 || mod == 5) {
        node = tree.getNode(tree.findNode(maxValue));
    } else {
        node = tree.getNode(tree.findNode(minValue));
    }

    while(true) {
        bool flag = true;
        for(int i=0; i<node->size; i++) {
            if(mod == 0) {
                if(node->keySet[i] < minValue) continue;
                if(node->keySet[i] > maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 1) {
                if(node->keySet[i] <= minValue) continue;
                if(node->keySet[i] > maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 2) {
                if(node->keySet[i] < minValue) continue;
                if(node->keySet[i] >= maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 3) {
                if(node->keySet[i] <= minValue) continue;
                if(node->keySet[i] >= maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 4) {
                if(node->keySet[i] >= maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 5) {
                if(node->keySet[i] > maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 6) {
                if(node->keySet[i] <= minValue) continue;
            } else if(mod == 7) {
                if(node->keySet[i] < minValue) continue;
            }
            List<int, 4> list(node->linkSet[i]);
            for(auto& add : list.all()) {
                res.insert(PageManager::Instance().readJSONFromFile(add));
            }
        }
        if(!flag) break;
        if(node->right == 0) break;
        node = tree.getNode(node->right);
    }
    return res;
}

/*
 * range search mod explain
 * mod = 0:   minValue <= value <= maxValue
 * mod = 1:   minValue < value <= maxValue
 * mod = 2:   minValue <= value < maxValue
 * mod = 3:   minValue < value < maxValue
 */

std::set<nlohmann::json>
Collection::queryNumber(const std::string &prefix, const double &minValue, const double &maxValue, const int &mod) {
    std::set<nlohmann::json> res;
    auto treeName = prefix + "$number";
    printf(">>>> %s %f %f\n", prefix.c_str(), minValue, maxValue);

    if(!index->exists(treeName)) {
        return res;
    }

    BTree<double, 8> tree(index->find(treeName));
    const int K_SIZE = 8;
    std::shared_ptr<BTreeNode<M_SIZE, double , K_SIZE>> node;

    if(mod == 4 || mod == 5) {
        node = tree.getNode(tree.findNode(maxValue));
    } else {
        node = tree.getNode(tree.findNode(minValue));
    }

    while(true) {
        bool flag = true;
        for(int i=0; i<node->size; i++) {

            if(mod == 0) {
                if(node->keySet[i] < minValue) continue;
                if(node->keySet[i] > maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 1) {
                if(node->keySet[i] <= minValue) continue;
                if(node->keySet[i] > maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 2) {
                if(node->keySet[i] < minValue) continue;
                if(node->keySet[i] >= maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 3) {
                if(node->keySet[i] <= minValue) continue;
                if(node->keySet[i] >= maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 4) {
                if(node->keySet[i] >= maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 5) {
                if(node->keySet[i] > maxValue) {
                    flag = false;
                    break;
                }
            } else if(mod == 6) {
                if(node->keySet[i] <= minValue) continue;
            } else if(mod == 7) {
                if(node->keySet[i] < minValue) continue;
            }


            List<int, 4> list(node->linkSet[i]);
            for(auto& add : list.all()) {
                res.insert(PageManager::Instance().readJSONFromFile(add));
            }
        }
        if(!flag) break;
        if(node->right == 0) break;
        node = tree.getNode(node->right);
    }
    return res;
}

std::set<nlohmann::json>
Collection::queryBool(const std::string &prefix, const bool &value) {
    std::set<nlohmann::json> res;
    auto treeName = prefix + "$boolean";
    //printf(">>>> %s %s %s\n", prefix.c_str(), minValue.c_str(), maxValue.c_str());
    if(!index->exists(treeName)) {
        return res;
    }
    BTree<bool, 1> tree(index->find(treeName));
    auto node = tree.getNode(tree.findNode(value));
    while(true) {
        bool flag = true;
        for(int i=0; i<node->size; i++) {
            if(node->keySet[i] < value) continue;
            if(node->keySet[i] > value) {
                flag = false;
                break;
            }
            List<int, 4> list(node->linkSet[i]);
            for(auto& add : list.all()) {
                res.insert(PageManager::Instance().readJSONFromFile(add));
            }
        }
        if(!flag) break;
        if(node->right == 0) break;
        node = tree.getNode(node->right);
    }
    return res;
}
