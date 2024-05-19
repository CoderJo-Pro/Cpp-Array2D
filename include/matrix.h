#ifndef MATRIX_H_
#define MATRIX_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <type_traits>
#include <version>

// macro defines
#include "defs.h"

namespace arr2d
{

    template <typename T, typename measure_t, measure_t m, measure_t n>
    class matrix
    {
        static_assert(std::is_integral_v<measure_t> && std::is_unsigned_v<measure_t>,
                      "matrix<T, measure_t>: std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value != true");

      public:
        using value_type = T;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using size_type = measure_t;
        using difference_type = std::ptrdiff_t;

        template <size_type h, size_type w>
        using matrix_by_size = matrix<T, size_type, h, w>;

        template <typename U>
        using matrix_by_type = matrix<U, size_type, m, n>;
        
      private:
        using data_row = std::array<T, n>;
        using data_block = std::array<data_row, m>;

        data_block data_;

        constexpr T& get(size_type y, size_type x) { return data_[y][x]; }
        constexpr const T& get(size_type y, size_type x) const { return data_[y][x]; }

      public:
        matrix() = default;

        template <typename U>
        matrix(const U& init_val)
        {
            std::for_each_n(begin(), m * n, [&init_val](T& item) { item = init_val; });
        }
        template <typename U>
        matrix(U init_data[m][n])
        {
            for (size_type x = 0; x < n; x++)
                for (size_type y = 0; y < m; y++)
                    get(y, x) = init_data[y][x];
        }

        constexpr size_type width() const { return n; }
        constexpr size_type height() const { return m; }

        constexpr size_type size() const { return m * n; }
        constexpr size_type max_size() const { return size(); }

        constexpr iterator begin() { return data_.begin()->begin(); }
        constexpr iterator end() { return data_.end()->begin(); }

        constexpr const_iterator cbegin() const { return data_.begin()->begin(); }
        constexpr const_iterator cend() const { return data_.end()->begin(); }

        constexpr void row_swap(size_type a, size_type b, size_type start_col = 0)
        {
            for (size_type col = start_col; col < n; col++)
                std::swap(get(a, col), get(b, col));
        }
        constexpr void row_scale(size_type row, T scaler, size_type start_col = 0)
        {
            for (size_type col = start_col; col < n; col++)
                get(row, col) *= scaler;
        }
        constexpr void row_add(size_type from, size_type to, size_type start_col = 0)
        {
            for (size_type col = start_col; col < n; col++)
                get(to, col) += get(from, col);
        }
        constexpr void row_add_scaled(size_type from, T scaler, size_type to, size_type start_col = 0)
        {
            for (size_type col = start_col; col < n; col++)
                get(to, col) += get(from, col) * scaler;
        }

        constexpr size_type row_pivot(size_type row)
        {
            for (size_type col = 0; col < n; col++)
                if (get(row, col) != 0)
                    LIKELY { return col; }
            return n;
        }

        constexpr void make_ref()
        {
            size_type curr_col = 0;
            size_type curr_row = 0;

            while (curr_row < m)
            {
                size_type largest = curr_row;
                for (size_type row = largest + 1; row < m; row++)
                    if (std::abs(get(largest, curr_col)) < std::abs(get(row, curr_col)))
                        largest = row;

                if (get(largest, curr_col) == static_cast<T>(0))
                    UNLIKELY
                    {
                        curr_col++;
                        continue;
                    }
                if (largest != curr_row)
                    LIKELY { row_swap(curr_row, largest, curr_col); }

                // can modify here to multiply pivot_reciprocal with pivot every row below for scaler when width is bigger
                T pivot_reciprocal = static_cast<T>(1) / get(curr_row, curr_col);
                row_scale(curr_row, pivot_reciprocal, curr_col);
                for (size_type row = curr_row + 1; row < m; row++)
                    row_add_scaled(curr_row, -get(row, curr_col), row, curr_col);

                curr_col++;
                curr_row++;
            }
        }
        constexpr void make_rref()
        {
            for (size_type curr_row = m - 1; curr_row > 0; curr_row--)
            {
                size_type pivot = row_pivot(curr_row);
                if (pivot != n)
                    for (size_type row = 0; row < curr_row; row++)
                        row_add_scaled(curr_row, -get(row, pivot), row);
            }
        }

        constexpr void solve()
        {
            make_ref();
            make_rref();
        }

        /// @brief Gets the element at x column and y row.
        /// @param y a size_type for row index.
        /// @param x a size_type for row column index.
        /// @return Element at x column and y row.
        constexpr T& operator()(size_type y, size_type x) { return get(y, x); }
        constexpr const T& operator()(size_type y, size_type x) const { return get(y, x); }

        /// @brief Binary operation: +matrix
        /// @return copy of origin matrix
        matrix operator+() const { return *this; }

        /// @brief Binary operation: -matrix
        /// @return negatived origin matrix
        matrix operator-() const
        {
            matrix result;
            for (size_type x = 0; x < n; x++)
                for (size_type y = 0; y < m; y++)
                    result(y, x) = -operator()(y, x);
            return result;
        }

        // friend matrix operator+(const matrix& lhs, const matrix& rhs);
        // friend matrix operator*(const matrix& mt, T scaler);
        // friend matrix operator*(T scaler, const matrix& mt);

        template <size_type p>
        matrix_by_size<m, p> operator*(const matrix_by_size<n, p>& rhs) const
        {
            matrix_by_size<m, p> result;

            for (size_type x = 0; x < p; x++)
                for (size_type y = 0; y < m; y++)
                {
                    T sum = 0;
                    for (size_type i = 0; i < n; i++)
                        sum += operator()(y, i) * rhs(i, x);
                    result(y, x) = sum;
                }
            return result;
        }

        template <typename U>
        operator matrix_by_type<U>() const
        {
            matrix_by_type<U> result;
            for (size_type x = 0; x < n; x++)
                for (size_type y = 0; y < m; y++)
                    result(y, x) = get(y, x);
            return result;
        }
    };

    template <typename T, typename size_type, size_type m, size_type n>
    matrix<T, size_type, m, n> operator+(const matrix<T, size_type, m, n>& lhs, const matrix<T, size_type, m, n>& rhs)
    {
        matrix<T, size_type, m, n> result;
        for (size_type x = 0; x < n; x++)
            for (size_type y = 0; y < m; y++)
                result(y, x) = lhs(y, x) + rhs(y, x);
        return result;
    }

    template <typename T, typename size_type, size_type m, size_type n>
    matrix<T, size_type, m, n> operator-(const matrix<T, size_type, m, n>& lhs, const matrix<T, size_type, m, n>& rhs)
    {
        matrix<T, size_type, m, n> result;
        for (size_type x = 0; x < n; x++)
            for (size_type y = 0; y < m; y++)
                result(y, x) = lhs(y, x) - rhs(y, x);
        return result;
    }
    template <typename T, typename size_type, size_type m, size_type n>
    matrix<T, size_type, m, n> operator*(const matrix<T, size_type, m, n>& mt, T scaler)
    {
        matrix<T, size_type, m, n> result;
        for (size_type x = 0; x < n; x++)
            for (size_type y = 0; y < m; y++)
                result(y, x) = mt(y, x) * scaler;
        return result;
    }

    template <typename T, typename size_type, size_type m, size_type n>
    inline matrix<T, size_type, m, n> operator*(T scaler, const matrix<T, size_type, m, n>& mt)
    {
        return mt * scaler;
    }

    template <typename T, typename size_type, size_type m, size_type n>
    inline std::ostream& operator<<(std::ostream& os, const matrix<T, size_type, m, n>& mt)
    {
        std::ios_base::fmtflags flag = os.setf(std::ios_base::dec, std::ios_base::floatfield);

        for (size_type y = 0; y < m; y++)
        {
            for (size_type x = 0; x < n; x++)
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