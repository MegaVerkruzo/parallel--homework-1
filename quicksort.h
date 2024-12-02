#include <algorithm>
#include <functional>
#include <ctime>
#include <iostream>
#include <random>
#include <ranges>
#include <map>

#include <parlay/parallel.h>
#include <parlay/primitives.h>
#include <parlay/sequence.h>

std::mt19937 rand_gen(4);
int batch_size_line = 100000;
int batch_size_log = 100000005;

template<typename It>
void qsort_seq(It a, It b) {
    if (b - a <= 1) {
        return;
    }

    auto main_element = *(a + rand_gen() % (b - a));
    auto left = a;
    auto right = b - 1;

    while (left <= right) {
        while (*left < main_element) ++left;
        while (*right > main_element) --right;
        if (left <= right) {
            std::iter_swap(left++, right--);
        }
    }

    qsort_seq(a, right + 1);
    qsort_seq(left, b);
}

void quicksort_seq(std::vector<long> &vec) {
    qsort_seq(vec.begin(), vec.end());
}

template<typename It>
void qsort_par_line(It a, It b) {
    if (b - a <= batch_size_line) {
        qsort_seq(a, b);
        return;
    }

    auto main_element = *(a + rand_gen() % (b - a));
    auto left = a;
    auto right = b - 1;

    while (left <= right) {
        while (*left < main_element) ++left;
        while (*right > main_element) --right;
        if (left <= right) {
            std::iter_swap(left++, right--);
        }
    }

    parlay::par_do(
            [&] { qsort_par_line(a, right + 1); },
            [&] { qsort_par_line(left, b); }
    );
}

auto quicksort_par_line(std::vector<long> &vec) {
    qsort_par_line(vec.begin(), vec.end());
}

template<typename It>
void qsort_par_log(It a, It b) {
    int n = b - a;
    if (n <= batch_size_log) {
        qsort_par_line(a, b);
        return;
    }

    auto main_element = *(a + rand_gen() % (n));
    auto [left_seq, mid_seq, right_seq] = std::make_tuple(
            parlay::filter(std::ranges::subrange(a, b), [&](const auto &elem) { return elem < main_element; }),
            parlay::filter(std::ranges::subrange(a, b), [&](const auto &elem) { return elem < main_element; }),
            parlay::filter(std::ranges::subrange(a, b), [&](const auto &elem) { return elem < main_element; })
    );
    auto [left_size, mid_size] = std::make_tuple(
            left_seq.size(),
            mid_seq.size()
    );

    parlay::parallel_for(0, n, [&](size_t i) {
        if (i < left_size) {
            *(a + i) = left_seq[i];
        } else if (i < left_size + mid_size) {
            *(a + i) = mid_seq[i - left_size];
        } else {
            *(a + i) = right_seq[i - left_size - mid_size];
        }
    });

    parlay::parallel_do(
            [&] { qsort_par_log(a, a + left_size); },
            [&] { qsort_par_log(a + left_size + mid_size, b); }
    );
}

auto quicksort_par_log(std::vector<long> &vec) {
    qsort_par_log(vec.begin(), vec.end());
}
