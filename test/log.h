#ifndef LOG_H
#define LOG_H

#include <iostream>

void log() { std::cout << std::endl; }

template <typename T>
void log(T item)
{
    std::cout << item << std::endl;
}

// template<typename T>
// void log(T* item)
// {
//     std::cout << (void*)item << std::endl;
// }

// void log(char* item)
// {
//     std::cout << item << std::endl;
// }

#endif // LOG_H