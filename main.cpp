#include <iostream>
#include <string>
#include <random>

#include <parlay/io.h>
#include <parlay/random.h>
#include <parlay/sequence.h>
#include <parlay/internal/get_time.h>

#include "quicksort.h"

inline bool test(std::vector<long> &result) {
    std::vector<long> real_sorted_array(result.size());
    std::copy_n(result.begin(), result.size(), real_sorted_array.begin());
    std::sort(real_sorted_array.begin(), real_sorted_array.end());

    for (int i = 0; i < result.size(); i++) {
        if (result[i] != real_sorted_array[i]) {
            return false;
        }
    }

    return true;
}

inline double make_test_sort(const std::function<void(std::vector<long> &)> &my_sort, parlay::sequence<long> &data,
                             const std::string &print) {
    parlay::internal::timer t(print);
    std::vector<long> vec(data.size());
    std::copy_n(data.begin(), data.size(), vec.begin());
    t.start();
    my_sort(vec);
    double time_execute = t.next_time();
    if (test(vec)) {
        std::cout << "test_passed, ";
    } else {
        std::cout << "test_not_passed, ";
    }
    std::cout << print << time_execute << '\n';
    return time_execute;
}

inline void launch(const std::function<void(std::vector<long> &)> &my_sort, parlay::sequence<long> &data,
                   const std::string &print, int k) {
    double mean = 0;
    for (int i = 0; i < k; i++) {
        mean += make_test_sort(my_sort, data, print);
    }
    std::cout << "mean time: " << mean / k << '\n';
}

int main(int argc, char *argv[]) {
    auto usage = "Usage: quicksort <n> <k>";
    if (argc != 3) std::cout << usage << std::endl;
    else {
        long n;
        try { n = std::stol(argv[1]); }
        catch (...) {
            std::cout << usage << '\n';
            return 1;
        }
        int k;
        try {k = std::stoi(argv[2]); }
        catch (...) {
            std::cout << usage << '\n';
            return 1;
        }

        parlay::random_generator gen;
        std::uniform_int_distribution<long> dis(0, n - 1);

        // generate random long values
        auto data = parlay::tabulate(n, [&](long i) {
            auto r = gen[i];
            return dis(r);
        });

        launch(quicksort_seq, data, "quicksort seq: ", k);
        launch(quicksort_par_line, data, "quicksort par line: ", k);
        launch(quicksort_par_log, data, "quicksort par log: ", k);
    }
}
