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
#include <vector>
#include <version>

// macro defs
#include <arr2d/defs.h>

#define _GRID_TYPEDEFS(T, measure_t, Alloc)                                                                                                          \
  public:                                                                                                                                            \
    using value_type = T;                                                                                                                            \
    using pointer = value_type*;                                                                                                                     \
    using const_pointer = const value_type*;                                                                                                         \
    using reference = value_type&;                                                                                                                   \
    using const_reference = const value_type&;                                                                                                       \
                                                                                                                                                     \
    using iterator = value_type*;                                                                                                                    \
    using const_iterator = const value_type*;                                                                                                        \
    using size_type = measure_t;                                                                                                                     \
    using difference_type = std::ptrdiff_t;                                                                                                          \
                                                                                                                                                     \
    using reverse_iterator = std::reverse_iterator<iterator>;                                                                                        \
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;                                                                            \
                                                                                                                                                     \
    using data_row = pointer;                                                                                                                        \
    using data_row_ptr = data_row*;                                                                                                                  \
                                                                                                                                                     \
    using allocator_type = Alloc;                                                                                                                    \
                                                                                                                                                     \
    using value_type_alloc_type = typename allocator_type::template rebind<value_type>::other;                                                       \
    using data_row_alloc_type = typename allocator_type::template rebind<data_row>::other;                                                           \
                                                                                                                                                     \
  private:

namespace arr2d
{
    template <typename T, typename measure_t = std::size_t, typename Alloc = std::allocator<T>>
    class _grid_base
    {
        _GRID_TYPEDEFS(T, measure_t, Alloc)

        struct grid_impl_data
        {
            data_row_ptr data_{nullptr};
            size_type size_{0};
            size_type width_{0};
            size_type height_{0};

            constexpr void reset()
            {
                data_ = nullptr;
                size_ = 0;
                width_ = 0;
                height_ = 0;
            }

            constexpr void copy_from(const grid_impl_data& from, data_row_ptr data) { std::copy(from.cbegin(), from.cend(), *data); }
            constexpr void copy_from(const grid_impl_data& from) { copy_from(from, data_); }

            constexpr void move_info_from(const grid_impl_data& from)
            {
                size_ = from.size_;
                width_ = from.width_;
                height_ = from.height_;
            }

            constexpr void move_from(const grid_impl_data& from)
            {
                data_ = from.data_;
                move_info_from(from);
            }

            constexpr void swap(const grid_impl_data& other) noexcept
            {
                data_row_ptr temp_data = data_;
                size_type temp_size = size_;
                size_type temp_width = width_;
                size_type temp_height = height_;
                move_from(other);
                other.data_ = temp_data;
                other.size_ = temp_size;
                other.width_ = temp_width;
                other.height_ = temp_height;
            }
        };

        struct grid_impl : public value_type_alloc_type, public grid_impl_data
        {
            constexpr grid_impl() = default;
            constexpr grid_impl(const value_type_alloc_type& a) : value_type_alloc_type{a} {}
            constexpr grid_impl(grid_impl&& impl) noexcept : value_type_alloc_type(std::move(impl)), grid_impl_data(std::move(impl)) {}
            constexpr grid_impl(value_type_alloc_type&& a) noexcept : value_type_alloc_type(std::move(a)) {}
            constexpr grid_impl(value_type_alloc_type&& a, grid_impl&& impl) noexcept :
                value_type_alloc_type(std::move(a)),
                grid_impl_data(std::move(impl))
            {
            }
        };

        grid_impl impl_;

        constexpr data_row_ptr alloc_data(const size_type p_width, const size_type p_height, const size_type p_size)
        {
            data_row_ptr new_data{impl_.allocate(p_height)};
            *new_data = impl_.allocate(p_size);
            for (size_type row = 1; row < p_height; row++)
                new_data[row] = new_data[row - 1] + p_width;
            return new_data;
        }

      public:
        constexpr _grid_base() = default;
        constexpr _grid_base(const allocator_type& a) : impl_(a) {}
        constexpr _grid_base(const size_type p_width, const size_type p_height) : width_(p_width), height_(p_height), size_(p_width * p_height)
        {
            data_ = alloc_data(width_, height_, size_);
        }
        constexpr _grid_base(const size_type p_width, const size_type p_height, const allocator_type& a) : _grid_base{p_width, p_height}, impl_{a} {}
        constexpr _grid_base(_grid_base&&) = default;
        constexpr _grid_base(value_type_alloc_type&& a) : impl_{std::move(a)} {}
        constexpr _grid_base(_grid_base&& grid_base, const allocator_type& a) : impl_{a} {}
        constexpr _grid_base(const allocator_type& a, _grid_base&& grid_base) : impl_{value_type_alloc_type{a}, std::move(grid_base.impl_)} {}

        ~_grid_base()
        {
            using traits = std::allocator_traits<value_type_alloc_type>;
            traits::deallocate(impl_, *(impl_.data_), impl_.size_);

            using data_row_traits = std::allocator_traits<data_row_alloc_type>;
            data_row_traits::deallocate(data_row_alloc_type{}, impl_.data_, impl_.height_);
        }
    };

    template <typename T, typename measure_t = std::size_t, typename Alloc = std::allocator<T>>
    class grid : public _grid_base<T, measure_t, Alloc>
    {
        static_assert(std::is_integral_v<measure_t> && std::is_unsigned_v<measure_t>,
                      "grid<T, measure_t>: std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value != true");
        _GRID_TYPEDEFS(T, measure_t, Alloc)

        using _grid_base<T, measure_t, Alloc>::impl_;

      public:
        NODISCARD constexpr size_type empty() const noexcept { return impl_.size_ == 0; }
        NODISCARD constexpr size_type max_size() const noexcept { return impl_.size_; }

        NODISCARD constexpr size_type size() const noexcept { return impl_.size_; }
        NODISCARD constexpr size_type width() const noexcept { return impl_.width_; }
        NODISCARD constexpr size_type height() const noexcept { return impl_.height_; }

        constexpr reference get(size_type index) { return (*impl_.data_)[index]; }
        NODISCARD constexpr const_reference get(size_type index) const { return (*impl_.data_)[index]; }

        constexpr reference get(size_type x, size_type y) { return impl_.data_[y][x]; }
        NODISCARD constexpr const_reference get(size_type x, size_type y) const { return impl_.data_[y][x]; }

        constexpr reference at(size_type index) { return const_cast<reference>(static_cast<const grid>(*this).at(index)); }
        NODISCARD constexpr const_reference at(size_type index) const;

        constexpr reference at(size_type x, size_type y) { return const_cast<reference>(static_cast<const grid>(*this).at(x, y)); }
        NODISCARD constexpr const_reference at(size_type x, size_type y) const;

        constexpr iterator begin() { return iterator{*impl_.data_}; }
        constexpr iterator end() { return iterator{*impl_.data_ + size()}; }

        NODISCARD constexpr const_iterator begin() const { return const_iterator{*impl_.data_}; }
        NODISCARD constexpr const_iterator end() const { return const_iterator{*impl_.data_ + size()}; }

        NODISCARD constexpr const_iterator cbegin() const { return const_iterator{begin()}; }
        NODISCARD constexpr const_iterator cend() const { return const_iterator{end()}; }

        constexpr reverse_iterator rbegin() { return reverse_iterator{end()}; }
        constexpr reverse_iterator rend() { return reverse_iterator{begin()}; }

        NODISCARD constexpr const_reverse_iterator rbegin() const { return const_reverse_iterator{end()}; }
        NODISCARD constexpr const_reverse_iterator rend() const { return const_reverse_iterator{begin()}; }

        NODISCARD constexpr const_reverse_iterator crbegin() const { return const_reverse_iterator{rbegin()}; }
        NODISCARD constexpr const_reverse_iterator crend() const { return const_reverse_iterator{rend()}; }

        constexpr void swap(grid& other) noexcept;
        constexpr allocator_type get_allocator() { return impl_; }

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
    constexpr typename grid<T, measure_t, Alloc>::const_reference grid<T, measure_t, Alloc>::at(size_type index) const
    {
        if (index >= impl_.size_)
            throw std::out_of_range{to_string("grid::at: index (which is ", index, ") >= size_ (which is ", size(), ") ")};
        return get(index);
    }

    template <typename T, typename measure_t, typename Alloc>
    constexpr typename grid<T, measure_t, Alloc>::const_reference grid<T, measure_t, Alloc>::at(size_type x, size_type y) const
    {
        if (x >= impl_.width_)
            throw std::out_of_range(to_string("grid::at: x (which is ", x, ") >= width_ (which is ", width(), ")"));
        if (y >= impl_.height_)
            throw std::out_of_range(to_string("grid::at: y (which is ", y, ") >= height_ (which is ", height(), ")"));
        return get(x, y);
    }

    template <typename T, typename measure_t, typename Alloc>
    constexpr void grid<T, measure_t, Alloc>::swap(grid& other) noexcept
    {
        data_row_ptr temp_data = impl_.data_;
        measure_t temp_size = impl_.size_;
        measure_t temp_width = impl_.width_;
        measure_t temp_height = impl_.height_;
        move_from(other);
        other.impl_.data_ = temp_data;
        other.impl_.size_ = temp_size;
        other.impl_.width_ = temp_width;
        other.impl_.height_ = temp_height;
    }

    template <typename T, typename measure_t, typename Alloc>
    constexpr grid<T, measure_t, Alloc>& grid<T, measure_t, Alloc>::operator=(const grid& rhs)
    {
        // This code only allocates the memory to avoid pointing null data when bad_alloc
        data_row_ptr new_data = alloc_data(rhs.width_, rhs.height_, rhs.size_);

        // Allocate successfully, copy it to new_data
        copy_from(rhs, new_data);

        // It's good here, can delete now
        ~grid();

        // Assign to data_
        impl_.data_ = new_data;
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