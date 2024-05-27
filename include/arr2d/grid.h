#ifndef GRID_H
#define GRID_H

#include "to_string.h"

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <version>

// macro defs
#include <arr2d/defs.h>

namespace arr2d
{

    template <typename T, typename measure_t = std::size_t>
    class grid
    {
        static_assert(std::is_integral_v<measure_t> && std::is_unsigned_v<measure_t>,
                      "grid<T, measure_t>: std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value != true");

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

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        using data_row = pointer;
        using data_row_ptr = data_row*;

      private:
        data_row_ptr data_{nullptr};
        measure_t size_{0};
        measure_t width_{0};
        measure_t height_{0};

        constexpr void reset()
        {
            data_ = nullptr;
            size_ = 0;
            width_ = 0;
            height_ = 0;
        }

        constexpr data_row_ptr alloc_data(measure_t p_width, measure_t p_height, measure_t p_size)
        {
            data_row_ptr new_data{new data_row[p_height]};
            *new_data = new value_type[p_size];
            for (measure_t row = 1; row < p_height; row++)
                new_data[row] = new_data[row - 1] + p_width;
            return new_data;
        }

        constexpr void copy_from(const grid& from, data_row_ptr data) { std::copy(from.cbegin(), from.cend(), *data); }
        constexpr void copy_from(const grid& from) { copy_from(from, data_); }

        constexpr void move_info_from(const grid& from)
        {
            size_ = from.size_;
            width_ = from.width_;
            height_ = from.height_;
        }

        constexpr void move_from(const grid& from)
        {
            data_ = from.data_;
            move_info_from(from);
        }

      public:
        grid() = default;
        grid(measure_t p_width, measure_t p_height) : width_(p_width), height_(p_height), size_(p_width * p_height)
        {
            data_ = alloc_data(width_, height_, size_);
        }
        grid(const grid& rhs) : grid(rhs.width_, rhs.height_) { copy_from(rhs); }
        grid(grid&& rhs) noexcept : width_(rhs.width_), height_(rhs.height_), size_(rhs.size_), data_(rhs.data_) { rhs.reset(); }

        ~grid();

        NODISCARD constexpr measure_t empty() const noexcept { return size_ == 0; }
        NODISCARD constexpr measure_t max_size() const noexcept { return size_; }

        NODISCARD constexpr measure_t size() const noexcept { return size_; }
        NODISCARD constexpr measure_t width() const noexcept { return width_; }
        NODISCARD constexpr measure_t height() const noexcept { return height_; }

        constexpr reference get(measure_t index) { return (*data_)[index]; }
        NODISCARD constexpr const_reference get(measure_t index) const { return (*data_)[index]; }

        constexpr reference get(measure_t x, measure_t y) { return data_[y][x]; }
        NODISCARD constexpr const_reference get(measure_t x, measure_t y) const { return data_[y][x]; }

        constexpr reference at(measure_t index) { return const_cast<reference>(static_cast<const grid>(*this).at(index)); }
        constexpr const_reference at(measure_t index) const;

        constexpr reference at(measure_t x, measure_t y)
        {
            return const_cast<reference>(static_cast<const grid>(*this).at(x, y));
        }
        NODISCARD constexpr const_reference at(measure_t x, measure_t y) const;

        template <typename U>
        constexpr void clamp(U& x, U& y) const noexcept
        {
            x = std::clamp<U>(x, 0, width_);
            y = std::clamp<U>(y, 0, height_);
        }

        constexpr iterator begin() { return iterator{*data_}; }
        constexpr iterator end() { return iterator{*data_ + size()}; }

        NODISCARD constexpr const_iterator begin() const { return const_iterator{*data_}; }
        NODISCARD constexpr const_iterator end() const { return const_iterator{*data_ + size()}; }

        NODISCARD constexpr const_iterator cbegin() const { return const_iterator{begin()}; }
        NODISCARD constexpr const_iterator cend() const { return const_iterator{end()}; }

        constexpr reverse_iterator rbegin() { return reverse_iterator{end()}; }
        constexpr reverse_iterator rend() { return reverse_iterator{begin()}; }

        NODISCARD constexpr const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
        NODISCARD constexpr const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }

        NODISCARD constexpr const_reverse_iterator crbegin() const { return const_reverse_iterator{rbegin()}; }
        NODISCARD constexpr const_reverse_iterator crend() const { return const_reverse_iterator{rend()}; }

        constexpr void swap(grid& other) noexcept;

        constexpr grid& operator=(const grid& rhs);
        constexpr grid& operator=(grid&& rhs) noexcept;

        constexpr reference operator()(measure_t index) { return get(index); }
        constexpr const_reference operator()(measure_t index) const { return get(index); }

        constexpr reference operator()(measure_t x, measure_t y) { return get(x, y); }
        constexpr const_reference operator()(measure_t x, measure_t y) const { return get(x, y); }

        constexpr reference operator[](measure_t index) { return get(index); }
        constexpr const_reference operator[](measure_t index) const { return get(index); }

#if __cpp_multidimensional_subscript >= 202110L
        constexpr reference operator[](measure_t x, measure_t y) { return get(x, y); }
        constexpr const_reference operator[](measure_t x, measure_t y) const { return get(x, y); }
#endif

        friend constexpr bool operator==(const grid& lhs, const grid& rhs)
        {
            return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
        }
        friend constexpr bool operator!=(const grid& rhs, const grid& lhs) { return !(lhs == rhs); }
    };

    template <typename T, typename measure_t>
    grid<T, measure_t>::~grid()
    {
        if (data_) LIKELY
        {
            delete[] *data_;
            delete[] data_;
        }
    }

    template <typename T, typename measure_t>
    constexpr typename grid<T, measure_t>::const_reference grid<T, measure_t>::at(measure_t index) const
    {
        if (index >= size_)
            throw std::out_of_range{to_string("grid::at: index (which is ", index, ") >= size_ (which is ", size(), ") ")};
        return get(index);
    }

    template <typename T, typename measure_t>
    constexpr typename grid<T, measure_t>::const_reference grid<T, measure_t>::at(measure_t x, measure_t y) const
    {
        if (x >= width_)
            throw std::out_of_range(to_string("grid::at: x (which is ", x, ") >= width_ (which is ", width(), ")"));
        if (y >= height_)
            throw std::out_of_range(to_string("grid::at: y (which is ", y, ") >= height_ (which is ", height(), ")"));
        return get(x, y);
    }

    template <typename T, typename measure_t>
    constexpr void grid<T, measure_t>::swap(grid& other) noexcept
    {
        data_row_ptr temp_data = data_;
        measure_t temp_size = size_;
        measure_t temp_width = width_;
        measure_t temp_height = height_;
        move_from(other);
        other.data_ = temp_data;
        other.size_ = temp_size;
        other.width_ = temp_width;
        other.height_ = temp_height;
    }

    template <typename T, typename measure_t>
    constexpr grid<T, measure_t>& grid<T, measure_t>::operator=(const grid& rhs)
    {
        // This code only allocates the memory to avoid pointing null data when bad_alloc
        data_row_ptr new_data = alloc_data(rhs.width_, rhs.height_, rhs.size_);

        // Allocate successfully, copy it to new_data
        copy_from(rhs, new_data);

        // It's good here, can delete now
        ~grid();

        // Assign to data_
        data_ = new_data;
        move_info_from(rhs);

        return *this;
    }

    template <typename T, typename measure_t>
    constexpr grid<T, measure_t>& grid<T, measure_t>::operator=(grid&& rhs) noexcept
    {
        if (this == &rhs)
            UNLIKELY { return *this; }

        move_from(rhs);
        rhs.reset();

        return *this;
    }

    template <typename T, typename measure_t>
    constexpr void swap(grid<T, measure_t>& a, grid<T, measure_t>& b) noexcept
    {
        return a.swap(b);
    }

} // namespace arr2d

#include <arr2d/undefs.h>

#endif // GRID_H