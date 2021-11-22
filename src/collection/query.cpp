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

std::set<nlohmann::json> Collection::queryRange(const std::string &prefix, nlohmann::json json) {

    std::set<nlohmann::json> set;
    bool init = false;

    // grammer check
    bool isNumber;
    bool nInit = false;
    for(auto& [key, value] : json.items()) {
        if(key == "$or") {
            auto arr = value.get<nlohmann::json>();
            std::set<nlohmann::json> tmp;
            for(auto& q : arr) {
                tmp = setUnion(tmp, queryRange(prefix, q.get<nlohmann::json>()));
            }
            if(init) set = setIntersection(set, tmp);
            else set = tmp, init = true;
        } else if(key == "$ne") {
            // do nothing
        } else if(!nInit) {
            if(value.is_string()) {
                isNumber = false;
            } else if(value.is_number()) {
                isNumber = true;
            } else {
                throw "unvalid query json.";
            }
            nInit = true;
        } else {
            //printf("hello: %s %d %d\n", key.c_str(),isNumber && !value.is_number(), !isNumber && !value.is_string());
            if((isNumber && !value.is_number()) || (!isNumber && !value.is_string())) {
                throw "unvalid query json.";
            }
        }
    }

    if(nInit && isNumber) {
        if(json.contains("$gt") && json.contains("$gte")) {
                auto gt = json["$gt"].get<double>();
                auto gte = json["$gte"].get<double>();
                if(gte > gt) {
                    json.erase("$gt");
                } else {
                    json.erase("$gte");
                }
            }

        if(json.contains("$lt") && json.contains("$lte")) {
            auto lt = json["$lt"].get<double>();
            auto lte = json["$lte"].get<double>();
            if(lt <= lte) {
                json.erase("$lte");
            } else {
                json.erase("$lt");
            }
        }

        std::set<nlohmann::json> tmp;

        if(json.contains("$gt")) {
            if(json.contains("$lt")) {
                tmp = queryNumber(prefix, json["$gt"].get<double>(), json["$lt"].get<double>(), 3);
            } else if(json.contains("$lte")) {
                tmp = queryNumber(prefix, json["$gt"].get<double>(), json["$lte"].get<double>(), 1);
            } else {

                tmp = queryNumber(prefix, json["$gt"].get<double>(), 0, 6);

            }
        } else if(json.contains("$gte")) {
            if(json.contains("$lt")) {
                tmp = queryNumber(prefix, json["$gte"].get<double>(), json["$lt"].get<double>(), 2);
            } else if(json.contains("$lte")) {
                tmp = queryNumber(prefix, json["$gte"].get<double>(), json["$lte"].get<double>(), 0);
            } else {
                tmp = queryNumber(prefix, json["$gte"].get<double>(), 0, 7);
            }
        } else if(json.contains("$lt")) {
            tmp = queryNumber(prefix, 0, json["$lt"].get<double>(), 4);
        } else if(json.contains("$lte")) {
            tmp = queryNumber(prefix, 0, json["$lte"].get<double>(), 5);
        }

        if(init) set = setIntersection(set, tmp);
        else set = tmp, init = true;

    } 
    if(nInit && !isNumber){
        if(json.contains("$gt") && json.contains("$gte")) {
            auto gt = json["$gt"].get<std::string>();
            auto gte = json["$gte"].get<std::string>();
            if(gte > gt) {
                json.erase("$gt");
            } else {
                json.erase("$gte");
            }
        }

        if(json.contains("$lt") && json.contains("$lte")) {
            auto lt = json["$lt"].get<std::string>();
            auto lte = json["$lte"].get<std::string>();
            if(lt <= lte) {
                json.erase("$lte");
            } else {
                json.erase("$lt");
            }
        }

        std::set<nlohmann::json> tmp;

        if(json.contains("$gt")) {
            if(json.contains("$lt")) {
                tmp = queryString(prefix, json["$gt"].get<std::string>(), json["$lt"].get<std::string>(), 3);
            } else if(json.contains("$lte")) {
                tmp = queryString(prefix, json["$gt"].get<std::string>(), json["$lte"].get<std::string>(), 1);
            } else {
                tmp = queryString(prefix, json["$gt"].get<std::string>(), "", 6);
            }
        } else if(json.contains("$gte")) {
            if(json.contains("$lt")) {
                tmp = queryString(prefix, json["$gte"].get<std::string>(), json["$lt"].get<std::string>(), 2);
            } else if(json.contains("$lte")) {
                tmp = queryString(prefix, json["$gte"].get<std::string>(), json["$lte"].get<std::string>(), 0);
            } else {
                tmp = queryString(prefix, json["$gte"].get<std::string>(), "", 7);
            }
        } else if(json.contains("$lt")) {
            tmp = queryString(prefix, "", json["$lt"].get<std::string>(), 4);
        } else if(json.contains("$lte")) {
            tmp = queryString(prefix, "", json["$lte"].get<std::string>(), 5);
        }

        if(init) set = setIntersection(set, tmp);
        else set = tmp, init = true;
    }

    printf(">> %d queryRange  prefix: %s  query: %s\n", init, prefix.c_str(), json.dump().c_str());

    if(!init) set = queryAllByField(prefix);

    printf("result: \n");
    for(auto it=set.begin(); it!=set.end(); it++) {
        printf(" - %s\n", it->dump().c_str());
    }

    if(json.contains("$ne")) {
        if(json["$ne"].is_null()) {
            List<int, 4> tree(index->find(prefix + "$null"));
            for(auto& add : tree.all()) {
                set.erase(PageManager::readJSONFromFile(add));
            }
        } else if(json["$ne"].is_string()) {
            List<int, 4> tree(index->find(prefix + "$null"));
            for(auto& add : tree.all()) {
                set.insert(PageManager::readJSONFromFile(add));
            }
            auto value = json["$ne"].get<std::string>();
            BTree<std::string, 64> indexTree(index->find(prefix + "$string"));
            if(indexTree.find(value) != -1) {
                List<int, 4> indexList(indexTree.find(value));
                for(auto& add : indexList.all()) {
                    set.erase(PageManager::readJSONFromFile(add));
                }
            }
            
        } else if(json["$ne"].is_number()) {
            List<int, 4> tree(index->find(prefix + "$null"));
            for(auto& add : tree.all()) {
                set.insert(PageManager::readJSONFromFile(add));
            }

            auto value = json["$ne"].get<double>();
            BTree<double, 8> indexTree(index->find(prefix + "$number"));
            if(indexTree.find(value) != -1) {
                List<int, 4> indexList(indexTree.find(value));
                for(auto& add : indexList.all()) {
                    set.erase(PageManager::readJSONFromFile(add));
                }
            }
        } else {
            throw "unvalid query json.";
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
    bool init = true, flag = false;

    for(auto& [key, value] : json.items()) {

        std::set<nlohmann::json> tmp;
        std::string tPrefix = prefix + key;

        if(key == "__INVO_ID__") {
            int add = uuid->find(value);
            if(add != -1) {
                tmp.insert(PageManager::readJSONFromFile(add));
            }
        }
        else if(key == "$or") {

            std::string tPrefix = (prefix == "") ? prefix : prefix.substr(0, prefix.size()-1);
            for(auto element : json[key].get<nlohmann::json>()) {
                if(element.is_boolean()) {
                    tmp = setUnion(queryBool(tPrefix, element.get<bool>()), tmp);
                } else if(element.is_string()) {
                    tmp = setUnion(queryString(tPrefix, element.get<std::string>(), element.get<std::string>()), tmp);
                } else if(element.is_number()) {
                    tmp = setUnion(queryNumber(tPrefix, element.get<double>(), element.get<double>()), tmp);
                } else if(element.is_null()) {
                    tmp = setUnion(queryNull(tPrefix), tmp);
                } else if(element.is_object()) {
                    tmp = setUnion(innerQuery(tPrefix, element.get<nlohmann::json>()), tmp);
                }
            }
        } else if(json[key].is_object()) {
            tmp = innerQuery(prefix + key + ".", json[key].get<nlohmann::json>());
        } else if(json[key].is_array()) {
            flag = true;
            for(auto element : json[key].get<nlohmann::json>()) {
                if(element.is_boolean()) {
                    if (flag)
                    {
                        tmp = queryBool(tPrefix, element.get<bool>());
                        flag = false;
                    }
                    else
                        tmp = setIntersection(queryBool(tPrefix, element.get<bool>()), tmp);
                } else if(element.is_string()) {
                    if (flag)
                    {
                        tmp = queryString(tPrefix, element.get<std::string>(), element.get<std::string>());
                        flag = false;
                    }
                    else
                        tmp = setIntersection(queryString(tPrefix, element.get<std::string>(), element.get<std::string>()), tmp);
                } else if(element.is_number()) {
                    if (flag)
                    {
                        tmp = queryNumber(tPrefix, element.get<double>(), element.get<double>());
                        flag = false;
                    }
                    else
                        tmp = setIntersection(queryNumber(tPrefix, element.get<double>(), element.get<double>()), tmp);
                } else if(element.is_null()) {
                    if (flag)
                    {
                        tmp = queryNull(tPrefix);
                        flag = false;
                    }
                    else
                        tmp = setIntersection(queryNull(tPrefix), tmp);
                } else if(element.is_object()) {
                    if (flag)
                    {
                        tmp = innerQuery(tPrefix, element.get<nlohmann::json>());
                        flag = false;
                    }
                    else
                        tmp = setIntersection(innerQuery(tPrefix, element.get<nlohmann::json>()), tmp);
                }
            }
        } else if(json[key].is_boolean()) {
            tmp = queryBool(tPrefix, json[key].get<bool>());
        } else if(json[key].is_string()) {
            tmp = queryString(tPrefix, json[key].get<std::string>(), json[key].get<std::string>());
        } else if(json[key].is_number()) {
            tmp = queryNumber(tPrefix, json[key].get<double>(), json[key].get<double>());
        } else if(json[key].is_null()) {
            tmp = queryNull(tPrefix);
        }
        if (flag) continue;
        if(init) res = tmp, init = false;
        else res = setIntersection(res, tmp);
    }

    auto str = json.dump();
    printf("query: %s prefix: %s\n", str.c_str(), prefix.c_str());
    printf("result: \n");
    for(auto it=res.begin(); it!=res.end(); it++) {
        printf(" - %s\n", it->dump().c_str());
    }

    if (init) return queryAllByField(prefix == "" ? prefix : prefix.substr(0, prefix.size()-1));

    return res;
}



std::set<nlohmann::json>  Collection::queryAllByField(const std::string &fieldName) {
    std::set<nlohmann::json> res;

    // return all
    if(fieldName == "") {
        for(auto& [key, value] : uuid->all()) {
            res.insert(PageManager::readJSONFromFile(value));
        }
    } else {

        int treeID;

        if((treeID = index->find(fieldName + "$string")) != -1) {
            BTree<std::string, 64> tree(treeID);
            for(auto &[key, value] : tree.all()) {
                List<int, 4> list(value);
                for(auto& add : list.all()) {
                    res.insert(PageManager::readJSONFromFile(add));
                }
            }
        }

        if((treeID = index->find(fieldName + "$number")) != -1) {
            BTree<double, 8> tree(treeID);
            for(auto &[key, value] : tree.all()) {
                List<int, 4> list(value);
                for(auto& add : list.all()) {
                    res.insert(PageManager::readJSONFromFile(add));
                }
            }
        }

        if((treeID = index->find(fieldName + "$boolean")) != -1) {
            BTree<bool, 1> tree(treeID);
            for(auto &[key, value] : tree.all()) {
                List<int, 4> list(value);
                for(auto& add : list.all()) {
                    res.insert(PageManager::readJSONFromFile(add));
                }
            }
        }

        if((treeID = index->find(fieldName + "$null")) != -1) {
            List<int, 4> list(treeID);
            for(auto& add : list.all()) {
                res.insert(PageManager::readJSONFromFile(add));
            }
        }
        
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
    //printf("query string >>>> %s %s %s\n", prefix.c_str(), minValue.c_str(), maxValue.c_str());
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
                res.insert(PageManager::readJSONFromFile(add));
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
    printf(">>>> %s %f %f %d\n", prefix.c_str(), minValue, maxValue, mod);

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
                res.insert(PageManager::readJSONFromFile(add));
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
                res.insert(PageManager::readJSONFromFile(add));
            }
        }
        if(!flag) break;
        if(node->right == 0) break;
        node = tree.getNode(node->right);
    }
    return res;
}

std::set<nlohmann::json> Collection::queryNull(const std::string &prefix) {
    std::set<nlohmann::json> res;
    auto treeName = prefix + "$null";
    if(!index->exists(treeName)) {
        return res;
    }
    List<int, 4> tree(index->find(treeName));
    for(auto& add : tree.all()) {
        res.insert(PageManager::readJSONFromFile(add));
    }
    return res;
}