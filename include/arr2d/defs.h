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

#endif // DEFS_H
