#ifndef DEFS_H
#define DEFS_H

#ifdef __has_cpp_attribute
#    if __has_cpp_attribute(likely)
#        define LIKELY [[likely]]
#    endif
#endif
#ifndef LIKELY
#    define LIKELY /*[[likely]]*/
#endif

#ifdef __has_cpp_attribute
#    if __has_cpp_attribute(unlikely)
#        define UNLIKELY [[unlikely]]
#    endif
#endif
#ifndef UNLIKELY
#    define UNLIKELY /*[[unlikely]]*/
#endif

#if __cpp_constexpr >= 201907L
#    define CONSTEXPR_V constexpr
#else
#    define CONSTEXPR_V inline
#endif

#endif // DEFS_H