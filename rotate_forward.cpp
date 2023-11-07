// clang++ -std=c++20 -O3 rotate_forward.cpp
#include <algorithm>
#include <iterator>
#include <concepts>

template <std::forward_iterator I>
inline
void rotate_forward_step(I& f, I& m, I l) {
    // precondition: mutable_bounded_range(f, l) ∧ f ≺ m ≺ l
    I c = m;
    do {
        std::iter_swap(f++, c++);
        if (f == m) m = c;
    } while (c != l);
}

template <std::forward_iterator I>
inline
I rotate_forward_nontrivial(I f, I m, I l) {
    // Precondition: mutable bounded range(f, l) ∧ f ≺ m ≺ l
    rotate_forward_step(f, m, l);
    I m_prime = f;
    while (m != l) {
        rotate_forward_step(f, m, l);
    }
    return m_prime;
}

#include <vector>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <functional>
#include <numeric>


template <std::forward_iterator I>
void print_vector(I f, I l) {
    while (f != l) {
        std::cout << *f << ' ';
        ++f;
    }
    std::cout << '\n';
}

template <typename F, typename C, std::forward_iterator I>
auto measure_time(F func, C& data, I m) {
    auto const start = std::chrono::high_resolution_clock::now();
    I nm = func(data.begin(), m, data.end());
    auto const end = std::chrono::high_resolution_clock::now();
    return std::pair{nm, std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()};
}

int main() {
    using vec_t = std::vector<int>;
    using it_t = vec_t::iterator;
    vec_t sizes = {100, 1'000, 10'000, 100'000};
    for (auto size : sizes) {
        vec_t orig(size);
        std::iota(orig.begin(), orig.end(), 0);
        // print_vector(orig.begin(), orig.end());

        std::cout << "Size: " << size << '\n';

        for (int i = 1; i <= 10; ++i) {
            auto const offset = (size / 10) * i - 2;
            {
                auto data = orig;
                auto const m = data.begin() + offset;
                auto const [nm, time] = measure_time(std::rotate<it_t>, data, m);
                std::cout << "  std::rotate -               "
                        << "new m: " << *nm << ", "
                        << "first: " << data.front() << ", "
                        << "time for m at "
                        << offset
                        << ": " << time
                        << '\n';
            }

            {
                auto data = orig;
                auto const m = data.begin() + offset;

                auto const [nm, time] = measure_time(rotate_forward_nontrivial<it_t>, data, m);
                std::cout << "  rotate_forward_nontrivial - "
                        << "new m: " << *nm << ", "
                        << "first: " << data.front() << ", "
                        << "time for m at "
                        << offset
                        << ": " << time
                        << '\n';
            }
        }
        // break;
    }
}


