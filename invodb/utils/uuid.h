//
// Created by YuhangQ on 2021/10/23.
//

#ifndef INVODB_UUID_H
#define INVODB_UUID_H

#include <iostream>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

std::string generateUUID() {
    boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
    std::string uuid_string = boost::uuids::to_string(a_uuid);
    std::remove(uuid_string.begin(), uuid_string.end(), '-');
    uuid_string.resize(32);
    return uuid_string;
}

#endif //INVODB_UUID_H
