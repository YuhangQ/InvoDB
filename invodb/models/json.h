//
// Created by YuhangQ on 2021/10/9.
//

#ifndef INVODB_JSON_H
#define INVODB_JSON_H

#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

class JSON : public Document {
public:
    JSON(std::string json): Document() { this->Parse(json.c_str()); }
    JSON(const char *json): Document() { this->Parse(json); }

    std::string ToString();
private:
};


#endif //INVODB_JSON_H
