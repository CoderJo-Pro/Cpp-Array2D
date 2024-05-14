#ifndef TRANSFORMABLE
#define TRANSFORMABLE

#if __cpp_constexpr >= 201907L
#    define CONSTEXPR_V constexpr
#else
#    define CONSTEXPR_V inline
#endif

namespace arr2d
{

    template <typename T, typename measure_t>
    class col_operatable
    {
      public:
        CONSTEXPR_V virtual void col_swap(measure_t a, measure_t b) = 0;
        CONSTEXPR_V virtual void col_scale(measure_t col, T scaler) = 0;
        CONSTEXPR_V virtual void col_add(measure_t from, measure_t to) = 0;
        CONSTEXPR_V virtual void col_add_scaled(measure_t from, T scaler, measure_t to) = 0;
    };

    template <typename T, typename measure_t>
    class row_operatable
    {
      public:
        CONSTEXPR_V virtual void row_swap(measure_t a, measure_t b) = 0;
        CONSTEXPR_V virtual void row_scale(measure_t row, T scaler) = 0;
        CONSTEXPR_V virtual void row_add(measure_t from, measure_t to) = 0;
        CONSTEXPR_V virtual void row_add_scaled(measure_t from, T scaler, measure_t to) = 0;
    };

    template <typename T, typename measure_t>
    class operatable : public col_operatable<T, measure_t>, public row_operatable<T, measure_t>
    {
    };

} // namespace arr2d

#endif // TRANSFORMABLE
