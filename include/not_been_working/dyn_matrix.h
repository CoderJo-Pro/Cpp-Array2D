#ifndef MATRIX_H_
#define MATRIX_H_

#include "grid.h"
#include "operatable.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <span>

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

    template <typename T, typename measure_t>
#if __cpp_lib_concepts >= 202002L
        requires std::unsigned_integral<measure_t>
#endif
    class matrix : public row_operatable<T, measure_t>
    {
#if __cpp_lib_concepts < 202002L
        static_assert(std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value,
                      "matrix<T, measure_t>: std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value != true");
#endif

      private:
        grid<T, measure_t> data_;

        constexpr T& get(measure_t y, measure_t x) { return data_.get(x, y); }
        constexpr const T& get(measure_t y, measure_t x) const { return data_.get(x, y); }

      public:
        matrix()
        {
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    get(y, x) = 0;
        }
        template <typename U, measure_t m, measure_t n>
        matrix(std::span<std::span<U, n>, m> init_data)
        {
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    get(y, x) = init_data[y][x];
        }
        
        constexpr measure_t width() const noexcept { return data_.width(); }
        constexpr measure_t height() const noexcept { return data_.height(); }

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

        matrix_by_size<n, m> transposed() const
        {
            matrix_by_size<n, m> result;
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(x, y) = operator()(y, x);
            return result;
        }

        void make_ref()
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

            for (; curr_row < m; curr_row++)
            {
                measure_t largest = find_largest();
                if (largest == T(0))
                    UNLIKELY
                    {
                        curr_col++;
                        continue;
                    }
                if (largest != curr_row)
                    LIKELY { row_swap(curr_row, largest); }

                // can modify here to multiply pivot_reciprocal with pivot every row below for scaler when width is bigger
                T pivot_reciprocal = T(1) / get(curr_row, curr_col);
                row_scale(curr_row, pivot_reciprocal);
                for (measure_t row = curr_row + 1; row < m; row++)
                    row_add_scaled(curr_row, -get(row, curr_col), row);

                curr_col++;
            }
        }
        // void make_rref();

        /// @brief gets the element at x column and y row.
        /// @param y a measure_t for row index.
        /// @param x a measure_t for row column index.
        /// @return element at x column and y row.
        constexpr T& operator()(measure_t y, measure_t x) { return get(y, x); }
        constexpr const T& operator()(measure_t y, measure_t x) const { return get(y, x); }

        /// @brief Binary operation: +matrix
        /// @return copy of origin matrix
        same_type_matrix operator+() const { return *this; }

        /// @brief Binary operation: -matrix
        /// @return negatived origin matrix
        same_type_matrix operator-() const
        {
            same_type_matrix result;
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(y, x) = -operator()(y, x);
            return result;
        }

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
        operator matrix_by_type<U>()
        {
            matrix_by_type<U> result;
            for (measure_t x = 0; x < n; x++)
                for (measure_t y = 0; y < m; y++)
                    result(y, x) = get(y, x);
            return result;
        }
    };

    template <typename T, typename measure_t>
    matrix<T, measure_t> operator+(const matrix<T, measure_t>& lhs, const matrix<T, measure_t>& rhs)
    {
        matrix<T, measure_t> result;
        for (measure_t x = 0; x < n; x++)
            for (measure_t y = 0; y < m; y++)
                result(y, x) = lhs(y, x) + rhs(y, x);
        return result;
    }

    template <typename T, typename measure_t, measure_t m, measure_t n>
    matrix<T, measure_t> operator-(const matrix<T, measure_t>& lhs, const matrix<T, measure_t>& rhs)
    {
        matrix<T, measure_t> result;
        for (measure_t x = 0; x < n; x++)
            for (measure_t y = 0; y < m; y++)
                result(y, x) = lhs(y, x) - rhs(y, x);
        return result;
    }
    template <typename T, typename measure_t, measure_t m, measure_t n>
    matrix<T, measure_t> operator*(const matrix<T, measure_t>& mt, T scaler)
    {
        matrix<T, measure_t> result;
        for (measure_t x = 0; x < n; x++)
            for (measure_t y = 0; y < m; y++)
                result(y, x) = mt(y, x) * scaler;
        return result;
    }

    template <typename T, typename measure_t, measure_t m, measure_t n>
    inline matrix<T, measure_t> operator*(T scaler, const matrix<T, measure_t>& mt)
    {
        return mt * scaler;
    }

    template <typename T, typename measure_t, measure_t m, measure_t n>
    inline std::ostream& operator<<(std::ostream& os, const matrix<T, measure_t>& mt)
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

#undef LIKELY
#undef UNLIKELY

#endif // MATRIX_H_