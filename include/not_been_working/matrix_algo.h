#ifndef MATRIX_ALGO_H
#define MATRIX_ALGO_H

#include "matrix_base.h"

#define MATRIX_ALGO_TEMPLATE template <typename T, typename measure_t>
#define MATRIX_PARAMS measure_t m, measure_t n, get<T, measure_t> result

namespace arr2d
{

    namespace matrix_algo
    {
        template <typename T, typename measure_t>
        using get = T& (*)(measure_t, measure_t);

        template <typename T, typename measure_t>
        using const_get = T& (*)(measure_t, measure_t);

        MATRIX_ALGO_TEMPLATE
        inline void add(MATRIX_PARAMS, const_get<T, measure_t> lhs, const_get<T, measure_t> rhs)
        {
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(y, x) = lhs(y, x) + rhs(y, x);
        }

        MATRIX_ALGO_TEMPLATE
        inline void sub(MATRIX_PARAMS, const_get<T, measure_t> lhs, const_get<T, measure_t> rhs)
        {
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(y, x) = lhs(y, x) - rhs(y, x);
        }

        MATRIX_ALGO_TEMPLATE
        inline void scale(MATRIX_PARAMS, const_get<T, measure_t> mt, T& scaler)
        {
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(y, x) = mt(y, x) * scaler;
        }
    } // namespace

} // namespace arr2d

#endif // MATRIX_ALGO_H
