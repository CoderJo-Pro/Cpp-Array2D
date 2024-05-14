#ifndef ARR2D_H
#define ARR2D_H

#include <algorithm>
#include <array>

namespace arr2d
{

    template <typename T, typename measure_t = std::size_t>
    class dyn_arr2d
    {
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
        measure_t width_ = 0;
        measure_t height_ = 0;

        constexpr void reset()
        {
            data_ = nullptr;
            width_ = 0;
            height_ = 0;
        }

        constexpr data_row_ptr alloc_data(measure_t p_width, measure_t p_height)
        {
            data_row_ptr new_data = new data_row[p_height];
            *new_data = new value_type[p_width * p_height];
            for (measure_t row = 1; row < p_height; row++)
                new_data[row] = *new_data + row * p_width;
            return new_data;
        }

        constexpr void copy_from(const dyn_arr2d& other) { std::copy_n(*other.data_, width_ * height_, *data_); }

      public:
        dyn_arr2d() {}
        dyn_arr2d(measure_t p_width, measure_t p_height) : width_(p_width), height_(p_height)
        {
            data_ = alloc_data(width_, height_);
        }
        dyn_arr2d(const dyn_arr2d& rhs) : dyn_arr2d(rhs.width_, rhs.height_) { copy_from(rhs); }
        dyn_arr2d(dyn_arr2d&& rhs) : width_(rhs.width_), height_(rhs.height_)
        {
            data_ = rhs.data_;
            rhs.reset();
        }

        ~dyn_arr2d()
        {
            if (data_)
            {
                delete[] *data_;
                delete[] data_;
            }
        }

        constexpr measure_t width() const noexcept { return width_; }
        constexpr measure_t height() const noexcept { return height_; }

        constexpr reference get(measure_t index) { return const_cast<reference>(static_cast<const dyn_arr2d>(*this).get(index)); }
        constexpr const_reference get(measure_t index) const { return (*data_)[index]; }

        constexpr reference get(measure_t x, measure_t y)
        {
            return const_cast<reference>(static_cast<const dyn_arr2d>(*this).get(x, y));
        }
        constexpr const_reference get(measure_t x, measure_t y) const { return data_[y][x]; }

        constexpr reference at(measure_t x, measure_t y)
        {
            return const_cast<reference>(static_cast<const dyn_arr2d>(*this).at(x, y));
        }
        constexpr const_reference at(measure_t x, measure_t y) const
        {
            if (!(x < width_))
                std::__throw_out_of_range_fmt("dyn_arr2d::at: x (which is %zu) >= width_ (which is %zu)", x, width_);
            if (!(y < height_))
                std::__throw_out_of_range_fmt("dyn_arr2d::at: y (which is %zu) >= height_ (which is %zu)", y, height_);
            return get(x, y);
        }

        template <typename U>
        constexpr void clamp(U& x, U& y) const noexcept
        {
            x = std::clamp<U>(x, 0, width_);
            y = std::clamp<U>(y, 0, height_);
        }

        constexpr dyn_arr2d& operator=(const dyn_arr2d& rhs)
        {
            // this code only allocates the memory to avoid delete data when bad_alloc
            data_row_ptr new_data = alloc_data(rhs.width_, rhs.height_, rhs.width_ * rhs.height_);

            // here is good, can delete now
            delete *data_;
            delete data_;

            copy_from(rhs);

            return *this;
        }

        constexpr dyn_arr2d& operator=(dyn_arr2d&& rhs)
        {
            if (this == &rhs)
                return *this;

            data_ = rhs.data_;
            width_ = rhs.width_;
            height_ = rhs.height_;
            rhs.reset();

            return *this;
        }
    };

} // namespace arr2d

#endif // ARR2D_H
