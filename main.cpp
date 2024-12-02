#include <iostream>
#include <string>
#include <random>

#include <parlay/io.h>
#include <parlay/primitives.h>
#include <parlay/random.h>
#include <parlay/sequence.h>
#include <parlay/internal/get_time.h>

//#include "parlay/internal/quicksort.h"
#include "quicksort.h"

// **************************************************************
// Driver
// **************************************************************
int main(int argc, char* argv[]) {
    int k = 3;
    auto usage = "Usage: quicksort <n>";
    if (argc != 2) std::cout << usage << std::endl;
    else {
        long n;
        try { n = std::stol(argv[1]); }
        catch (...) { std::cout << usage << std::endl; return 1; }
        parlay::random_generator gen;
        std::uniform_int_distribution<long> dis(0, n-1);

        // generate random long values
        auto data = parlay::tabulate(n, [&] (long i) {
            auto r = gen[i];
            return dis(r);
        });

        parlay::internal::timer t("Time");
//        parlay::sequence<long> result;
//        for (int i=0; i < k; i++) {
//            result = quicksort_par(data);
//            t.next("quicksort parallel");
//        }

        parlay::sequence<long> seqResult;
        for (int i = 0; i < k; i++) {
            std::vector<long> vec(data.size());
            std::copy_n(data.begin(), data.size(), vec.begin());
            t.next("copy_n");
            quicksort_seq(vec);
            t.next("quicksort seq");
        }

        for (int i = 0; i < k; i++) {
            parlay::sequence<long> hui(data.size());
            std::copy_n(data.begin(), data.size(), hui.begin());
            t.next("copy_n");
            parlay::internal::p_quicksort_inplace(hui.cut(0, hui.size()), std::less<>());
            t.next("quicksort internal inplace");
        }

        for (int i = 0; i < k; i++) {
            std::vector<long> vec(data.size());
            std::copy_n(data.begin(), data.size(), vec.begin());
            t.next("copy_n");
            quicksort_line(vec);
            t.next("quicksort par line");
        }
    }
}