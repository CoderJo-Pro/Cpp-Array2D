#ifndef GRID_H
#define GRID_H

#include "to_string.h"

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <version>

// macro defs
#include "defs.h"

namespace arr2d
{

    template <typename T, typename measure_t = std::size_t, typename Alloc = std::allocator<T>>
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

        using allocator_type = Alloc;

      private:
        data_row data_{nullptr};
        size_type size_{0};
        size_type width_{0};
        size_type height_{0};

        static allocator_type allocator;

        constexpr void reset()
        {
            data_ = nullptr;
            size_ = 0;
            width_ = 0;
            height_ = 0;
        }

        constexpr data_row alloc_data(size_type p_width, size_type p_height, size_type p_size)
        {
            data_row new_data = allocator.allocate(p_size);

            const pointer end = new_data + p_size;
            for (pointer ptr = new_data; ptr != end; ++ptr)
                ::new ((void*)ptr) value_type();

            return new_data;
        }

        constexpr void copy_from(const grid& from, data_row data) { std::copy(from.cbegin(), from.cend(), data); }
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
        grid(size_type p_width, size_type p_height) : width_(p_width), height_(p_height), size_(p_width * p_height)
        {
            data_ = alloc_data(width_, height_, size_);
        }
        grid(const grid& rhs) : grid(rhs.width_, rhs.height_) { copy_from(rhs); }
        grid(grid&& rhs) noexcept : width_(rhs.width_), height_(rhs.height_), size_(rhs.size_), data_(rhs.data_) { rhs.reset(); }

        ~grid();

        NODISCARD constexpr size_type empty() const noexcept { return size_ == 0; }
        NODISCARD constexpr size_type max_size() const noexcept { return size_; }

        NODISCARD constexpr size_type size() const noexcept { return size_; }
        NODISCARD constexpr size_type width() const noexcept { return width_; }
        NODISCARD constexpr size_type height() const noexcept { return height_; }

        constexpr reference get(size_type index) { return data_[index]; }
        NODISCARD constexpr const_reference get(size_type index) const { return data_[index]; }

        constexpr reference get(size_type x, size_type y) { return data_[y * width() + x]; }
        NODISCARD constexpr const_reference get(size_type x, size_type y) const { return data_[y * width() + x]; }

        constexpr reference at(size_type index) { return const_cast<reference>(static_cast<const grid>(*this).at(index)); }
        NODISCARD constexpr const_reference at(size_type index) const;

        constexpr reference at(size_type x, size_type y) { return const_cast<reference>(static_cast<const grid>(*this).at(x, y)); }
        NODISCARD constexpr const_reference at(size_type x, size_type y) const;

        template <typename U>
        constexpr void clamp(U& x, U& y) const noexcept
        {
            x = std::clamp<U>(x, 0, width_);
            y = std::clamp<U>(y, 0, height_);
        }

        constexpr iterator begin() { return iterator{data_}; }
        constexpr iterator end() { return iterator{data_ + size()}; }

        NODISCARD constexpr const_iterator begin() const { return const_iterator{data_}; }
        NODISCARD constexpr const_iterator end() const { return const_iterator{data_ + size()}; }

        NODISCARD constexpr const_iterator cbegin() const { return const_iterator{begin()}; }
        NODISCARD constexpr const_iterator cend() const { return const_iterator{end()}; }

        constexpr reverse_iterator rbegin() { return reverse_iterator{end()}; }
        constexpr reverse_iterator rend() { return reverse_iterator{begin()}; }

        NODISCARD constexpr const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
        NODISCARD constexpr const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }

        NODISCARD constexpr const_reverse_iterator crbegin() const { return const_reverse_iterator{rbegin()}; }
        NODISCARD constexpr const_reverse_iterator crend() const { return const_reverse_iterator{rend()}; }

        constexpr void swap(grid& other) noexcept;
        constexpr allocator_type get_allocator() { return allocator; }

        constexpr grid& operator=(const grid& rhs);
        constexpr grid& operator=(grid&& rhs) noexcept;

        constexpr reference operator()(size_type index) { return get(index); }
        constexpr const_reference operator()(size_type index) const { return get(index); }

        constexpr reference operator()(size_type x, size_type y) { return get(x, y); }
        constexpr const_reference operator()(size_type x, size_type y) const { return get(x, y); }

        constexpr reference operator[](size_type index) { return get(index); }
        constexpr const_reference operator[](size_type index) const { return get(index); }

#if __cpp_multidimensional_subscript >= 202110L
        constexpr reference operator[](size_type x, size_type y) { return get(x, y); }
        constexpr const_reference operator[](size_type x, size_type y) const { return get(x, y); }
#endif

        friend constexpr bool operator==(const grid& lhs, const grid& rhs) { return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend()); }
        friend constexpr bool operator!=(const grid& rhs, const grid& lhs) { return !(lhs == rhs); }
    };

    template <typename T, typename measure_t, typename Alloc>
    typename grid<T, measure_t, Alloc>::allocator_type grid<T, measure_t, Alloc>::allocator{};

    template <typename T, typename measure_t, typename Alloc>
    grid<T, measure_t, Alloc>::~grid()
    {
        if (data_)
            LIKELY
            {
                std::for_each(begin(), end(), [](reference item) { item.~T(); });
                allocator.deallocate(data_, size_);
            }
    }

    template <typename T, typename measure_t, typename Alloc>
    constexpr typename grid<T, measure_t, Alloc>::const_reference grid<T, measure_t, Alloc>::at(size_type index) const
    {
        if (index >= size_)
            throw std::out_of_range{to_string("grid::at: index (which is ", index, ") >= size_ (which is ", size(), ") ")};
        return get(index);
    }

    template <typename T, typename measure_t, typename Alloc>
    constexpr typename grid<T, measure_t, Alloc>::const_reference grid<T, measure_t, Alloc>::at(size_type x, size_type y) const
    {
        if (x >= width_)
            throw std::out_of_range(to_string("grid::at: x (which is ", x, ") >= width_ (which is ", width(), ")"));
        if (y >= height_)
            throw std::out_of_range(to_string("grid::at: y (which is ", y, ") >= height_ (which is ", height(), ")"));
        return get(x, y);
    }

    template <typename T, typename measure_t, typename Alloc>
    constexpr void grid<T, measure_t, Alloc>::swap(grid& other) noexcept
    {
        data_row temp_data = data_;
        measure_t temp_size = size_;
        measure_t temp_width = width_;
        measure_t temp_height = height_;
        move_from(other);
        other.data_ = temp_data;
        other.size_ = temp_size;
        other.width_ = temp_width;
        other.height_ = temp_height;
    }

    template <typename T, typename measure_t, typename Alloc>
    constexpr grid<T, measure_t, Alloc>& grid<T, measure_t, Alloc>::operator=(const grid& rhs)
    {
        // This code only allocates the memory to avoid pointing null data when bad_alloc
        data_row new_data = alloc_data(rhs.width_, rhs.height_, rhs.size_);

        // Allocate successfully, copy it to new_data
        copy_from(rhs, new_data);

        // It's good here, can delete now
        this->~grid();

        // Assign to data_
        data_ = new_data;
        move_info_from(rhs);

        return *this;
    }

    template <typename T, typename measure_t, typename Alloc>
    constexpr grid<T, measure_t, Alloc>& grid<T, measure_t, Alloc>::operator=(grid&& rhs) noexcept
    {
        if (this == &rhs)
            UNLIKELY return *this;

        move_from(rhs);
        rhs.reset();

        return *this;
    }

    template <typename T, typename measure_t, typename Alloc>
    constexpr void swap(grid<T, measure_t, Alloc>& a, grid<T, measure_t, Alloc>& b) noexcept
    {
        return a.swap(b);
    }

} // namespace arr2d

#endif // GRID_H
