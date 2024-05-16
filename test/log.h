#ifndef LOG_H
#define LOG_H

#include <iostream>

void log() { std::cout << std::endl; }

template <typename T>
void log(T item)
{
    std::cout << item << std::endl;
}

#endif // LOG_H