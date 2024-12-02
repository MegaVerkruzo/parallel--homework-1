#include <algorithm>
#include <functional>
#include <ctime>
#include <iostream>
#include <random>

#include <parlay/parallel.h>
#include <parlay/primitives.h>
#include <parlay/sequence.h>

std::mt19937 rand_gen(4);
int batch_size = 100000;

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
    if (b - a < batch_size) {
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

auto quicksort_line(std::vector<long> &vec) {
    qsort_par_line(vec.begin(), vec.end());
}