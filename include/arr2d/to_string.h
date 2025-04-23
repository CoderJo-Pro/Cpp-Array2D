#ifndef TO_STRING_H_
#define TO_STRING_H_

#include <sstream>
#include <string>

namespace arr2d
{

    template <typename... Args>
    static std::string to_string(Args&&... args)
    {
        std::ostringstream oss;
        (oss << ... << args); // Fold expression
        return oss.str();
    }

} // namespace arr2d

#endif // TO_STRING_H_
