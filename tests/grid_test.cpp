#include "sequence_generator.h"

#include <algorithm>
#include <arr2d/grid.h>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#define GRID_WIDTH 5
#define GRID_HEIGHT 4
#define GRID_SIZE ((GRID_WIDTH) * (GRID_HEIGHT))

TEMPLATE_TEST_CASE("Grid test", "[grid]", int, std::string)
{
    using grid = arr2d::grid<TestType>;

    constexpr std::size_t w = GRID_WIDTH;
    constexpr std::size_t h = GRID_HEIGHT;
    constexpr std::size_t size = GRID_SIZE;
    grid g{w, h};

    REQUIRE(g.width() == w);
    REQUIRE(g.height() == h);
    REQUIRE(g.size() == w * h);

    SECTION("Manipulate grid instances with element accessing")
    {
        // Generated elements storage vector for g
        const std::vector<TestType> vec = GENERATE(take(5, random_seq<TestType>(GRID_SIZE)));
        REQUIRE(vec.size() == g.size());

        std::copy(vec.cbegin(), vec.cend(), g.begin());
        REQUIRE(std::equal(vec.cbegin(), vec.cend(), g.cbegin()));

        for (size_t y = 0; y < g.height(); y++)
            for (size_t x = 0; x < g.width(); x++)
                REQUIRE(g.get(x, y) == vec.at(y * g.width() + x));

        SECTION("Copy")
        {
            TestType temp;
            random(temp);
            const TestType magic_value = temp;
            const std::size_t magic_index = 0;

            SECTION("Copy constructor")
            {
                grid g2{g};

                REQUIRE(std::equal(g.cbegin(), g.cend(), g2.cbegin()));
                REQUIRE(std::equal(g.cbegin(), g.cend(), vec.cbegin()));

                g2[magic_index] = magic_value;
                REQUIRE(g2[magic_index] == magic_value);
                REQUIRE(g[magic_index] == vec[magic_index]);
            }

            SECTION("Copy assignment")
            {
                grid g2;
                g2 = g;

                REQUIRE(std::equal(g.cbegin(), g.cend(), g2.cbegin()));
                REQUIRE(std::equal(g.cbegin(), g.cend(), vec.cbegin()));

                g2[magic_index] = magic_value;
                REQUIRE(g2[magic_index] == magic_value);
                REQUIRE(g[magic_index] == vec[magic_index]);
            }

            REQUIRE(std::equal(g.cbegin(), g.cend(), vec.cbegin()));
        }

        SECTION("Move")
        {
            SECTION("Move constructor")
            {
                grid g2{std::move(g)};
                REQUIRE(std::equal(g2.cbegin(), g2.cend(), vec.cbegin()));
            }
            SECTION("Move assignment")
            {
                grid g2;
                g2 = std::move(g);
                REQUIRE(std::equal(g2.cbegin(), g2.cend(), vec.cbegin()));
            }

            REQUIRE(g.width() == 0);
            REQUIRE(g.height() == 0);
            REQUIRE(g.size() == 0);
        }
    }

    SECTION("Accesses that are out of bounds will throw exceptions") { REQUIRE_THROWS_AS(g.at(g.size()), std::out_of_range); }

    SECTION("Swap should swap the data")
    {
        
    }
}
