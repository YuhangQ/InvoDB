//
// Created by YuhangQ on 2021/10/22.
//

#ifndef INVODB_LOGGER_H
#define INVODB_LOGGER_H

#include <iostream>
#include <string>

class Logger {
public:
    template<typename T>
    static void info(const T& msg);
    template<typename T, typename K>
    static void info(const T& first, const K& second);
    template<typename T>
    static void warn(const T& msg);
    template<typename T, typename K>
    static void warn(const T& first, const K& second);
    template<typename T>
    static void error(const T& msg);
    template<typename T, typename K>
    static void error(const T& first, const K& second);
};

template<typename T>
void Logger::info(const T &msg) {
    std::cout << "[INFO] " << msg << std::endl;
}

template<typename T, typename K>
void Logger::info(const T &first, const K &second) {
    std::cout << "[INFO] " << first << second << std::endl;
}

template<typename T>
void Logger::warn(const T &msg) {
    std::cout << "[WARN] " << msg << std::endl;
}

template<typename T, typename K>
void Logger::warn(const T &first, const K &second) {
    std::cout << "[WARN] " << first << second << std::endl;
}

template<typename T>
void Logger::error(const T &msg) {
    std::cout << "[ERROR] " << msg << std::endl;
}

template<typename T, typename K>
void Logger::error(const T &first, const K &second) {
    std::cout << "[ERROR] " << first << second << std::endl;
}

#endif //INVODB_LOGGER_H
