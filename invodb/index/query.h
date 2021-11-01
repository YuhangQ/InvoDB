//
// Created by projector-user on 11/1/21.
//

#ifndef INVODB_QUERY_H
#define INVODB_QUERY_H

#include "json/json.hpp"

class Query {
    std::vector<nlohmann::json> find(const nlohmann::json &json);

};


#endif //INVODB_QUERY_H
