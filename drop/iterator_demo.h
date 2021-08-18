#include <iostream>
#include <algorithm>

template<long FROM, long TO>
class Range {
public:
    // member typedefs provided through inheriting from std::iterator
    class iterator : public std::iterator<
        std::input_iterator_tag,   // iterator_category
        long,                      // value_type (type of *it, for pure output iterator is void)
        long,                      // difference_type (type of it1-it2)
        const long*,               // pointer
        long                       // reference
    > {
        long num = FROM;
    public:
        explicit iterator(long _num = 0) : num(_num) {}
        iterator& operator++() { num = TO >= FROM ? num + 1 : num - 1; return *this; }
        iterator operator++(int) { iterator retval = *this; ++(*this); return retval; }
        bool operator==(iterator other) const { return num == other.num; }
        bool operator!=(iterator other) const { return !(*this == other); }
        reference operator*() const { return num; }
    };

    iterator begin() { return iterator(FROM); }
    iterator end() { return iterator(TO >= FROM ? TO + 1 : TO - 1); }
};

int main() {
    // std::find requires an input iterator
    auto range = Range<15, 25>();
    auto itr = std::find(range.begin(), range.end(), 18);
    std::cout << *itr << '\n'; // 18

    // Range::iterator also satisfies range-based for requirements
    for (long l : Range<3, 5>()) {
        std::cout << l << ' '; // 3 4 5
    }
    std::cout << '\n';
}

class num_iterator
{
    int i;
public:
    explicit num_iterator(int position = 0)
        : i{ position }
    {}

    int operator*() const
    {
        return i;
    }

    num_iterator& operator++()
    {
        ++i;
        return *this;
    }

    bool operator!=(const num_iterator& other) const
    {
        return i != other.i;
    }

    bool operator==(const num_iterator& other) const
    {
        return !(*this != other);
    }
};

class num_range
{
    int a;
    int b;

public:
    num_range(int from, int to)
        :a{ from }
        , b{ to }
    {}

    num_iterator begin() const
    {
        return num_iterator{ a };
    }

    num_iterator end() const
    {
        return num_iterator{ b };
    }
};

namespace std
{
    template <>
    struct iterator_traits<num_iterator>
    {
        using iterator_category = std::forward_iterator_tag;
        using value_type = int;
    };
}

int main()
{
    for (int i : num_range{ 100, 110 })
    {
        std::cout << i << ", ";
    }

    num_range range{ 100, 200 };
    auto it150 = std::find(std::begin(range), std::end(range), 150);
    std::cout << *it150 << "\n";
    std::cout << '\n';

    auto [min_it, max_it] = std::minmax_element(std::begin(range), std::end(range));
    std::cout << *min_it << " - " << *max_it << "\n";
}