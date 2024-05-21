#ifndef LOG_H_
#define LOG_H_

#include <iostream>

void log() { std::cout << std::endl; }

template <typename T>
void log(T item)
{
    std::cout << item << std::endl;
}

template <typename Arg, typename... Args>
void log(Arg arg, Args... args)
{
    std::cout << arg;
    log(args...);
}

#endif // LOG_H_