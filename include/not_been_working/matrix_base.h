#ifndef MATRIX_BASE_H
#define MATRIX_BASE_H

#include "defs.h"

namespace arr2d
{

    template <typename T, typename measure_t>
    class matrix_base
    {
      public:
        constexpr virtual measure_t width() const = 0;
        constexpr virtual measure_t height() const = 0;

        CONSTEXPR_V virtual T& get(measure_t y, measure_t x) = 0;
        CONSTEXPR_V virtual const T& get(measure_t y, measure_t x) const = 0;

      protected:
        constexpr inline void add(const matrix_base<T, measure_t>& lhs, const matrix_base<T, measure_t>& rhs,
                                  matrix_base<T, measure_t>& result)
        {
            for (measure_t x = 0; x < width(); x++)
                for (measure_t y = 0; y < height(); y++)
                    result.get(y, x) = lhs.get(y, x) + rhs.get(y, x);
        }
    };

} // namespace arr2d

#include "undefs.h"

#endif // MATRIX_BASE_H
