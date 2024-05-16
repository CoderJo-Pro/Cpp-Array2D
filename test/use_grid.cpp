#include "log.h"

#include <algorithm>
#include <cstddef>
#include <grid.h>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

template <typename T, typename measure_t = std::size_t>
void show_grid(const arr2d::grid<T, measure_t>& g)
{
    static const char hsep = '|';
    static const char vsep = '-';
    int max_w = 0;

    auto print_vsep = [&g, &max_w]()
    {
        for (std::size_t i = 0; i < (max_w + 1) * g.width() + 1; i++)
            std::cout.put(vsep);
        std::cout << std::endl;
    };

    for (measure_t y = 0; y < g.height(); y++)
        for (measure_t x = 0; x < g.width(); x++)
            max_w = std::max(max_w, static_cast<int>(std::to_string(g(x, y)).size()));

    std::ios_base::fmtflags origin = std::cout.setf(std::ios_base::right);
    for (measure_t y = 0; y < g.height(); y++)
    {
        print_vsep();
        for (measure_t x = 0; x < g.width(); x++)
            std::cout << hsep << std::setw(max_w) << g(x, y);
        std::cout << hsep << std::endl;
    }
    print_vsep();
    std::cout.setf(origin);
}

int main()
{
    using namespace arr2d;
    using std::cout;
    using std::endl;

    grid<int> g(3, 2);

    log(g.width());
    log(g.height());
    log(g.size());
    show_grid(g);

    {
        grid<int> g2 = g;
        show_grid(g2);
        show_grid(g);
    }
    show_grid(g);

    grid<int> g3 = std::move(grid<int>(3, 2));
    show_grid(g3);

    g = std::move(g3);
    show_grid(g);

    log("Should error here:");
    try
    {
        g.at(100) = 0;
    }
    catch (const std::out_of_range& err)
    {
        log(err.what());
    }

    grid<int> g2 = g;
    log("g == g2:");
    cout.setf(std::ios_base::boolalpha);
    log(g == g2);
    g2[0] = 100;
    cout.setf(std::ios_base::boolalpha);
    log(g != g2);

    log("Done.");
}