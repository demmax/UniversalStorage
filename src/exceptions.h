//
// Created by maxon on 02.05.18.
//

#ifndef UNIVERSALSTORAGE_EXCEPTIONS_H
#define UNIVERSALSTORAGE_EXCEPTIONS_H

#include <stdexcept>


class BadPathException : public std::runtime_error
{
public:
    BadPathException(const char *msg) : std::runtime_error(msg) {}
};


class NoSuchPathException : public std::runtime_error
{
public:
    NoSuchPathException(const char *msg) : std::runtime_error(msg) {}
};

#endif //UNIVERSALSTORAGE_EXCEPTIONS_H
