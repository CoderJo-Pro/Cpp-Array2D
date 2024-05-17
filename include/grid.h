#ifndef GRID_H
#define GRID_H

#include "to_string.h"

#include <algorithm>
#include <exception>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <version>

// macro defs
#include <defs.h>

namespace arr2d
{

    template <typename T, typename measure_t = std::size_t>
    // #if __cpp_lib_concepts >= 202002L
    //         requires std::unsigned_integral<measure_t>
    // #endif
    class grid
    {
        // #if __cpp_lib_concepts < 202002L
        static_assert(std::is_integral_v<measure_t> && std::is_unsigned_v<measure_t>,
                      "grid<T, measure_t>: std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value != true");
        // #endif

      public:
        using value_type = T;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using data_row = value_type*;
        using data_row_ptr = data_row*;
        using size_type = measure_t;
        using difference_type = std::ptrdiff_t;

      private:
        data_row_ptr data_ = nullptr;
        measure_t size_ = 0;
        measure_t width_ = 0;
        measure_t height_ = 0;

        constexpr void reset()
        {
            data_ = nullptr;
            size_ = 0;
            width_ = 0;
            height_ = 0;
        }

        constexpr data_row_ptr alloc_data(measure_t p_width, measure_t p_height, measure_t p_size)
        {
            data_row_ptr new_data = new data_row[p_height];
            *new_data = new value_type[p_size];
            for (measure_t row = 1; row < p_height; row++)
                new_data[row] = *new_data + row * p_width;
            return new_data;
        }

        constexpr void copy_from(const grid& other) { std::copy_n(*other.data_, size_, *data_); }

      public:
        grid() = default;
        grid(measure_t p_width, measure_t p_height) : width_(p_width), height_(p_height), size_(p_width * p_height)
        {
            data_ = alloc_data(width_, height_, size_);
        }
        grid(const grid& rhs) : grid(rhs.width_, rhs.height_) { copy_from(rhs); }
        grid(grid&& rhs) noexcept : width_(rhs.width_), height_(rhs.height_), size_(rhs.size_), data_(rhs.data_) { rhs.reset(); }

        ~grid();

        NODISCARD constexpr measure_t empty() const noexcept { return size_ == 0;}
        NODISCARD constexpr measure_t max_size() const noexcept { return size_;}

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

        constexpr iterator begin() { return *data_; }
        constexpr iterator end() { return *data_ + size(); }

        NODISCARD constexpr const_iterator cbegin() const { return *data_; }
        NODISCARD constexpr const_iterator cend() const { return *data_ + size(); }

        // TODO: Implement swap container method
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
        if (data_)
        {
            delete[] *data_;
            delete[] data_;
        }
    }

    template <typename T, typename measure_t>
    constexpr typename grid<T, measure_t>::const_reference grid<T, measure_t>::at(measure_t index) const
    {
        if (!(index < size_))
            throw std::out_of_range{to_string("grid::at: index (which is ", index, ") >= size_(which is ", size(), ") ")};
        return get(index);
    }

    template <typename T, typename measure_t>
    constexpr typename grid<T, measure_t>::const_reference grid<T, measure_t>::at(measure_t x, measure_t y) const
    {
        if (!(x < width_))
            throw std::out_of_range(to_string("grid::at: x (which is ", x, ") >= width_ (which is ", width(), ")"));
        if (!(y < height_))
            throw std::out_of_range(to_string("grid::at: y (which is ", y, ") >= height_ (which is ", height(), ")"));
        return get(x, y);
    }

    template <typename T, typename measure_t>
    constexpr void grid<T, measure_t>::swap(grid& other) noexcept
    {
        grid temp{other};
        other = std::move(*this);
        *this = std::move(temp);
    }

    template <typename T, typename measure_t>
    constexpr grid<T, measure_t>& grid<T, measure_t>::operator=(const grid& rhs)
    {
        // This code only allocates the memory to avoid pointing null data when bad_alloc
        data_row_ptr new_data = alloc_data(rhs.width_, rhs.height_, rhs.size_);

        // It's good here, can delete now
        delete *data_;
        delete data_;

        copy_from(rhs);

        return *this;
    }

    template <typename T, typename measure_t>
    constexpr grid<T, measure_t>& grid<T, measure_t>::operator=(grid&& rhs) noexcept
    {
        if (this == &rhs)
            return *this;

        data_ = rhs.data_;
        size_ = rhs.size_;
        width_ = rhs.width_;
        height_ = rhs.height_;
        rhs.reset();

        return *this;
    }

} // namespace arr2d

#endif // GRID_H