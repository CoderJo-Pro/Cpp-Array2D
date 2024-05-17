#ifndef LOG_H_
#define LOG_H_

#include <iostream>

void log() { std::cout << std::endl; }

template <typename T>
void log(T item)
{
    std::cout << item << std::endl;
}

#endif // LOG_H_