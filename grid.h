#ifndef GRID_H
#define GRID_H

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace arr2d
{

    template <typename T, typename measure_t = std::size_t>
#if __cpp_lib_concepts >= 202002L
        requires std::unsigned_integral<measure_t>
#endif
    class grid
    {
#if __cpp_lib_concepts < 202002L
        static_assert(std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value,
                      "grid<T, measure_t>: std::is_integral<measure_t>::value && std::is_unsigned<measure_t>::value != true");
#endif

      public:
        using value_type = T;
        // using pointer = value_type*;
        // using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using data_row = value_type*;
        using data_row_ptr = data_row*;

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
        grid() {}
        grid(measure_t p_width, measure_t p_height) : width_(p_width), height_(p_height), size_(p_width * p_height)
        {
            data_ = alloc_data(width_, height_, size_);
        }
        grid(const grid& rhs) : grid(rhs.width_, rhs.height_) { copy_from(rhs); }
        grid(grid&& rhs) : width_(rhs.width_), height_(rhs.height_), size_(rhs.size_)
        {
            data_ = rhs.data_;
            rhs.reset();
        }

        ~grid()
        {
            if (data_)
            {
                delete[] *data_;
                delete[] data_;
            }
        }

        constexpr measure_t size() const noexcept { return size_; }
        constexpr measure_t width() const noexcept { return width_; }
        constexpr measure_t height() const noexcept { return height_; }

        constexpr reference get(measure_t index) { return const_cast<reference>(static_cast<const grid>(*this).get(index)); }
        constexpr const_reference get(measure_t index) const { return (*data_)[index]; }

        constexpr reference get(measure_t x, measure_t y)
        {
            return const_cast<reference>(static_cast<const grid>(*this).get(x, y));
        }
        constexpr const_reference get(measure_t x, measure_t y) const { return data_[y][x]; }

        constexpr reference at(measure_t index) { return const_cast<reference>(static_cast<const grid>(*this).at(index)); }
        constexpr const_reference at(measure_t index) const
        {
            if (!(index < size_))
                throw std::out_of_range("grid::at: index (which is " + std::to_string(index) + ") >= size_(which is "
                                        + std::to_string(size_) + ") ");
            return get(index);
        }
        constexpr reference at(measure_t x, measure_t y)
        {
            return const_cast<reference>(static_cast<const grid>(*this).at(x, y));
        }
        constexpr const_reference at(measure_t x, measure_t y) const
        {
            if (!(x < width_))
                std::__throw_out_of_range_fmt("grid::at: x (which is %zu) >= width_ (which is %zu)", x, width_);
            if (!(y < height_))
                std::__throw_out_of_range_fmt("grid::at: y (which is %zu) >= height_ (which is %zu)", y, height_);
            return get(x, y);
        }

        template <typename U>
        constexpr void clamp(U& x, U& y) const noexcept
        {
            x = std::clamp<U>(x, 0, width_);
            y = std::clamp<U>(y, 0, height_);
        }

        constexpr grid& operator=(const grid& rhs)
        {
            // this code only allocates the memory to avoid delete data when bad_alloc
            data_row_ptr new_data = alloc_data(rhs.width_, rhs.height_, rhs.size_);

            // here is good, can delete now
            delete *data_;
            delete data_;

            copy_from(rhs);

            return *this;
        }

        constexpr grid& operator=(grid&& rhs)
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

        constexpr reference operator()(measure_t index) { return get(index); }
        constexpr const_reference operator()(measure_t index) const { return get(index); }

        constexpr reference operator()(measure_t x, measure_t y) { return get(x, y); }
        constexpr const_reference operator()(measure_t x, measure_t y) const { return get(x, y); }

        constexpr reference operator[](measure_t index) { return get(index); }
        constexpr const_reference operator[](measure_t index) const { return get(index); }

#if __cpp_multidimensional_subscript >= 202110L
        // constexpr reference operator[](measure_t args...) { return get(args); }
        // constexpr const_reference operator[](measure_t args...) const { return get(args); }

        constexpr reference operator[](measure_t x, measure_t y) { return get(x, y); }
        constexpr const_reference operator[](measure_t x, measure_t y) const { return get(x, y); }
#endif
    };

} // namespace arr2d

#endif // GRID_H