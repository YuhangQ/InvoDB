//
// Created by YuhangQ on 2021/10/22.
//

#ifndef INVODB_STORAGE_PAGE_H
#define INVODB_STORAGE_PAGE_H

#include <iostream>
#include <cstring>


class PageManager;

class StoragePage {
public:
    StoragePage();
    void print();
    int next();
    void setNext(const int& nextPage);
    int last();
    void setLast(const int& lastPage);
    void save();
    void clear();
    int getIntStartFrom(const int &index);
    void setIntStartFrom(const int &index, const int &value);
    void setDoubleStartFrom(const int &index, const double &value);
    void setStringStartFrom(const int &index, const std::string &str);
    void setStartFrom(const int &index, const void* content, int size);

    double getDoubleStartFrom(const int &index);

    int *intArray();
    StoragePage(const int& id) { memset(page, 0, sizeof(page)); this->address = id; }
    char& operator[] (int index) { if(index>=1024 || index < 0) throw "overflow"; else return this->page[index]; }
    operator const char *() const { return this->page; }
    operator char *() { return this->page; }
    int getAddress();
private:
    char page[1024];
    int address;
};


#endif //INVODB_STORAGE_PAGE_H
