//
// Created by maxon on 02.05.18.
//

#ifndef UNIVERSALSTORAGE_EXCEPTIONS_H
#define UNIVERSALSTORAGE_EXCEPTIONS_H

#include <stdexcept>


class StorageException : public std::runtime_error
{
public:
    StorageException(const char *msg) : std::runtime_error(msg) {}
};


class BadPathException : public StorageException
{
public:
    BadPathException(const char *msg) : StorageException(msg) {}
};


class NoSuchPathException : public StorageException
{
public:
    NoSuchPathException(const char *msg) : StorageException(msg) {}
};


class BadValueException : public StorageException
{
public:
    BadValueException(const char *msg) : StorageException(msg) {}
};



#endif //UNIVERSALSTORAGE_EXCEPTIONS_H
