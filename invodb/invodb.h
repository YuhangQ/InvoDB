//
// Created by YuhangQ on 2021/10/9.
//

#ifndef INVODB_INVODB_H
#define INVODB_INVODB_H

#include "models/json.h"
#include "models/collection.h"

class InvoDB {
public:
    Collection createCollection();
    Collection getCollection();
    void deleteCollection();

};


#endif //INVODB_INVODB_H
