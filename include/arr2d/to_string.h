#ifndef TO_STRING_H_
#define TO_STRING_H_

#include <string>

namespace arr2d
{

    template <typename Arg>
    inline std::string to_string(Arg arg)
    {
        return std::to_string(arg);
    }

    inline std::string to_string(const std::string& s)
    {
        return s;
    }

    inline std::string to_string(const char* const s)
    {
        return std::string{s};
    }

    template <typename Arg, typename... Args>
    inline std::string to_string(Arg arg, Args... args)
    {
        return to_string(arg) + to_string(args...);
    }

} // namespace arr2d

#endif // TO_STRING_H_
