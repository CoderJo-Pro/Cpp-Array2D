#ifndef DEFS_H
#define DEFS_H

#ifdef __has_cpp_attribute
#    if __has_cpp_attribute(likely)
#        define LIKELY [[likely]]
#    endif
#    if __has_cpp_attribute(nodiscard)
#        define NODISCARD [[nodiscard]]
#    endif
#    if __has_cpp_attribute(unlikely)
#        define UNLIKELY [[unlikely]]
#    endif
#endif

#ifndef LIKELY
#    define LIKELY /*[[likely]]*/
#endif
#ifndef UNLIKELY
#    define UNLIKELY /*[[unlikely]]*/
#endif
#ifndef NODISCARD
#    define NODISCARD /*[[nodiscard]]*/
#endif

#if __cpp_constexpr >= 201907L
#    define CONSTEXPR_V constexpr
#else
#    define CONSTEXPR_V inline
#endif

// Should always inline no matter what.
#ifndef _ALWAYS_INLINE_
#    if defined(__GNUC__)
#        define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
#    elif defined(_MSC_VER)
#        define _ALWAYS_INLINE_ __forceinline
#    else
#        define _ALWAYS_INLINE_ inline
#    endif
#endif

#endif // DEFS_H