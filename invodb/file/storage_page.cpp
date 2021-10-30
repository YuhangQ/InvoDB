//
// Created by YuhangQ on 2021/10/22.
//

#include "storage_page.h"
#include "page_manager.h"


void StoragePage::print() {
    printf("-------------------PageInfo--------------------\n");
    for(int i=0; i<8; i++) {
        for(int j=0; j<32; j++) {
            printf("%u ", intArray()[i*32+j]);
        }
        printf("\n");
    }
}

int StoragePage::next() {
    return getIntStartFrom(1020);
}

void StoragePage::setNext(const int& nextPage) {
    setIntStartFrom(1020, nextPage);
}

int StoragePage::last() {
    return getIntStartFrom(1016);
}

void StoragePage::setLast(const int& lastPage) {
    setIntStartFrom(1016, lastPage);
}

int *StoragePage::intArray() {
    return (int *)page;
}

int StoragePage::getIntStartFrom(const int& index) {
    return *((const int *)&page[index]);
}
void StoragePage::setIntStartFrom(const int& index, const int& value) {
    *((int *)&page[index]) = value;
}

void StoragePage::setDoubleStartFrom(const int &index, const double &value) {
    *((double *)&page[index]) = value;
}

void StoragePage::setStringStartFrom(const int &index, const std::string &str) {
    for(int i=0; i<str.size(); i++) {
        page[index+i] = str[i];
    }
}

void StoragePage::save() {
    PageManager::Instance().setPage(address, *this);
}

int StoragePage::getAddress() {
    return address;
}

StoragePage::StoragePage() {
    memset(page, 0, sizeof(page));
}

double StoragePage::getDoubleStartFrom(const int &index) {
    return *((const double *)&page[index]);
}

void StoragePage::setStartFrom(const int &index, const void *content, int size) {
    for(int i=0; i<size; i++) {
        page[index+i] = ((const char *)content)[i];
    }
}