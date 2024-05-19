#ifndef MATRIX_ALGO_H
#define MATRIX_ALGO_H

#include <matrix.h>

namespace arr2d
{

    template <typename T, typename size_type, size_type m, size_type n>
    constexpr matrix<T, size_type, n, m> transpose(const matrix<T, size_type, m, n>& mt)
    {
        matrix<T, size_type, n, m> result;
        for (size_type y = 0; y < m; y++)
            for (size_type x = 0; x < n; x++)
                result(x, y) = mt(y, x);
        return result;
    }
    
} // namespace arr2d

#endif // MATRIX_ALGO_H
