#ifndef MATRIX_H_
#define MATRIX_H_

#include "matrix_algo.h"
#include "matrix_base.h"
#include "operatable.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <type_traits>

// macro defines
#include "defs.h"

namespace arr2d
{
    template <typename T, typename measure_t, measure_t m, measure_t n>
    class matrix_data_impl
    {
      public:
        using data_column = std::array<T, m>;
        using data_block = std::array<data_column, n>;

      private:
        data_block data_;

      public:
        constexpr T& get(measure_t y, measure_t x) { return data_[x][y]; }
        constexpr const T& get(measure_t y, measure_t x) const { return data_[x][y]; }
    };

    template <typename T, typename measure_t, measure_t m, measure_t n>
#if __cpp_lib_concepts >= 202002L
        requires std::unsigned_integral<measure_t>
#endif
    class matrix : public row_operatable<T, measure_t>
    {
#if __cpp_lib_concepts < 202002L
        static_assert(std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value,
                      "matrix<T, measure_t>: std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value != true");
#endif

      public:
        template <measure_t h, measure_t w>
        using matrix_by_size = matrix<T, measure_t, h, w>;

        template <typename U>
        using matrix_by_type = matrix<U, measure_t, m, n>;

      private:
        matrix_data_impl<T, measure_t, m, n> data_;

        constexpr T& get(measure_t y, measure_t x) { return data_.get(y, x); }
        constexpr const T& get(measure_t y, measure_t x) const { return data_.get(y, x); }

      public:
        matrix()
        {
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    get(y, x) = 0;
        }
        template <typename U>
        matrix(U init_data[m][n])
        {
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    get(y, x) = init_data[y][x];
        }

        constexpr measure_t width() const { return n; }
        constexpr measure_t height() const { return m; }

        constexpr void row_swap(measure_t a, measure_t b) override
        {
            for (measure_t col = 0; col < n; col++)
                std::swap(get(a, col), get(b, col));
        }
        constexpr void row_scale(measure_t row, T scaler) override
        {
            for (measure_t col = 0; col < n; col++)
                get(row, col) *= scaler;
        }
        constexpr void row_add(measure_t from, measure_t to) override
        {
            for (measure_t col = 0; col < n; col++)
                get(to, col) += get(from, col);
        }
        constexpr void row_add_scaled(measure_t from, T scaler, measure_t to) override
        {
            for (measure_t col = 0; col < n; col++)
                get(to, col) += get(from, col) * scaler;
        }

        constexpr measure_t row_pivot(measure_t row)
        {
            for (measure_t col = 0; col < n; col++)
                if (get(row, col) != 0)
                    LIKELY { return col; }
            return n;
        }

        constexpr void transpose()
        {
            static_assert(m == n, "matrix<T, measure_t, m, n>: m != n");
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(x, y) = get(y, x);
        }

        constexpr matrix_by_size<n, m> transposed() const
        {
            matrix_by_size<n, m> result;
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(x, y) = operator()(y, x);
            return result;
        }

        constexpr void make_ref()
        {
            measure_t curr_col = 0;
            measure_t curr_row = 0;

            auto find_largest = [this, &curr_col, &curr_row]() -> measure_t
            {
                measure_t largest = curr_row;
                for (measure_t row = largest + 1; row < m; row++)
                    if (std::abs(get(largest, curr_col)) < std::abs(get(row, curr_col)))
                        largest = row;
                return largest;
            };

            for (; curr_row < m; curr_col++, curr_row++)
            {
                measure_t largest = find_largest();
                if (get(largest, curr_col) == static_cast<T>(0))
                    UNLIKELY { continue; }
                if (largest != curr_row)
                    LIKELY { row_swap(curr_row, largest); }

                // can modify here to multiply pivot_reciprocal with pivot every row below for scaler when width is bigger
                T pivot_reciprocal = static_cast<T>(1) / get(curr_row, curr_col);
                row_scale(curr_row, pivot_reciprocal);
                for (measure_t row = curr_row + 1; row < m; row++)
                    row_add_scaled(curr_row, -get(row, curr_col), row);
            }
        }
        constexpr void make_rref()
        {
            for (measure_t curr_row = m - 1; curr_row > 0; curr_row--)
            {
                measure_t pivot = row_pivot(curr_row);
                if (pivot != n)
                    for (measure_t row = 0; row < curr_row; row++)
                        row_add_scaled(curr_row, -get(row, pivot), row);
            }
        }

        constexpr void solve()
        {
            make_ref();
            make_rref();
        }

        /// @brief gets the element at x column and y row.
        /// @param y a measure_t for row index.
        /// @param x a measure_t for row column index.
        /// @return element at x column and y row.
        constexpr T& operator()(measure_t y, measure_t x) { return get(y, x); }
        constexpr const T& operator()(measure_t y, measure_t x) const { return get(y, x); }

        /// @brief Binary operation: +matrix
        /// @return copy of origin matrix
        matrix operator+() const { return *this; }

        /// @brief Binary operation: -matrix
        /// @return negatived origin matrix
        matrix operator-() const
        {
            matrix result;
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(y, x) = -operator()(y, x);
            return result;
        }

        // friend matrix operator+(const matrix& lhs, const matrix& rhs);
        // friend matrix operator*(const matrix& mt, T scaler);
        // friend matrix operator*(T scaler, const matrix& mt);

        template <measure_t p>
        matrix_by_size<m, p> operator*(const matrix_by_size<n, p>& rhs) const
        {
            matrix_by_size<m, p> result;

            for (measure_t x = 0; x < p; x++)
                for (measure_t y = 0; y < m; y++)
                {
                    T sum = 0;
                    for (measure_t i = 0; i < n; i++)
                        sum += operator()(y, i) * rhs(i, x);
                    result(y, x) = sum;
                }
            return result;
        }

        template <typename U>
        operator matrix_by_type<U>() const
        {
            matrix_by_type<U> result;
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(y, x) = get(y, x);
            return result;
        }
    };

    template <typename T, typename measure_t, measure_t m, measure_t n>
    matrix<T, measure_t, m, n> operator+(const matrix<T, measure_t, m, n>& lhs, const matrix<T, measure_t, m, n>& rhs)
    {
        matrix<T, measure_t, m, n> result;
        matrix_algo::add<T, measure_t>(m, n, result.get, lhs.get, rhs.get);
        return result;
    }

    template <typename T, typename measure_t, measure_t m, measure_t n>
    matrix<T, measure_t, m, n> operator-(const matrix<T, measure_t, m, n>& lhs, const matrix<T, measure_t, m, n>& rhs)
    {
        matrix<T, measure_t, m, n> result;
        matrix_algo::sub<T, measure_t>(m, n, result.get, lhs.get, rhs.get);
        return result;
    }
    template <typename T, typename measure_t, measure_t m, measure_t n>
    matrix<T, measure_t, m, n> operator*(const matrix<T, measure_t, m, n>& mt, T scaler)
    {
        matrix<T, measure_t, m, n> result;
        matrix_algo::scale<T, measure_t>(m, n, result.get, mt.get, scaler);
        return result;
    }

    template <typename T, typename measure_t, measure_t m, measure_t n>
    inline matrix<T, measure_t, m, n> operator*(T scaler, const matrix<T, measure_t, m, n>& mt)
    {
        return mt * scaler;
    }

    template <typename T, typename measure_t, measure_t m, measure_t n>
    inline std::ostream& operator<<(std::ostream& os, const matrix<T, measure_t, m, n>& mt)
    {
        std::ios_base::fmtflags flag = os.setf(std::ios_base::dec, std::ios_base::floatfield);

        for (measure_t y = 0; y < m; y++)
        {
            for (measure_t x = 0; x < n; x++)
            {
                // std::cout.setf(std::ios_base::fixed);
                // std::cout.precision(3);
                os << mt(y, x) << ' ';
            }
            os << std::endl;
        }

        os.setf(flag, std::ios_base::floatfield);
        return os;
    }

    template <typename T, std::size_t m, std::size_t n>
    using matrix_size_t = matrix<T, std::size_t, m, n>;

} // namespace arr2d

#include "undefs.h"

#endif // MATRIX_H_