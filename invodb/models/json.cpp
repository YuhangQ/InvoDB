//
// Created by YuhangQ on 2021/10/9.
//

#include "json.h"

std::string JSON::ToString() {
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    this->Accept(writer);
    return buffer.GetString();
}
