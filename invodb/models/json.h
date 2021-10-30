//
// Created by YuhangQ on 2021/10/9.
//

#ifndef INVODB_JSON_H
#define INVODB_JSON_H

#include <string>
#include "json/json.hpp"

class JSON : public nlohmann::json {
public:
    JSON() {}
    JSON(std::string j): nlohmann::json(j) {}
private:
};


#endif //INVODB_JSON_H
