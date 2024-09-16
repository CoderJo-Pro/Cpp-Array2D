#include <algorithm>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <random>
#include <string>
#include <utility>
#include <vector>

template <typename T>
void random(T& item)
{}

template <>
void random<int>(int& item)
{
    static std::minstd_rand gen;
    static std::uniform_int_distribution<int> dist{-100, 100};
    item = dist(gen);
}

template <>
void random<std::string>(std::string& item)
{
    static const std::vector<std::string> strs = {"Hello",
                                                  "World!",
                                                  "or not...",
                                                  "I LIKE PROGRAMMING!!!",
                                                  "testing",
                                                  "0110010111001010",
                                                  "This is a long sentence which is of a lot of memory.",
                                                  "short",
                                                  "This is a face: (-_-)",
                                                  "Another face: (^_^)"};

    static std::minstd_rand gen;
    static std::uniform_int_distribution<std::size_t> dist{0, strs.size() - 1};
    item = strs.at(dist(gen));
}

template <typename Elem>
void randomize(std::vector<Elem>& vec)
{
    for (Elem& item : vec)
        random<Elem>(item);
}

template <typename Elem>
class SequenceGenerator : public Catch::Generators::IGenerator<std::vector<Elem>>
{
    using Seq = std::vector<Elem>;

  private:
    std::size_t seq_size = 0;
    Seq current;

  public:
    SequenceGenerator(std::size_t size = 0) : seq_size(size), current(size, Elem()) { randomize<Elem>(current); }

    bool next() override
    {
        randomize<Elem>(current);
        return true;
    }

    [[nodiscard]] const Seq& get() const override { return current; }
};

template <typename Elem>
Catch::Generators::GeneratorWrapper<std::vector<Elem>> random_seq(std::size_t size)
{
    return Catch::Generators::GeneratorWrapper<std::vector<Elem>>(new SequenceGenerator<Elem>(size));
}