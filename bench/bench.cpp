#include "order_book.h"
#include <chrono>
#include <cstdio>
#include <random>
#include <vector>
#include <algorithm>

int main() {
    constexpr int N = 1'000'000;
    constexpr double TICK = 0.01;
    constexpr int    TICKS = 200; // [99.00, 101.00]

    std::mt19937 rng(42);
    std::uniform_int_distribution<int> tick_dist(0, TICKS);

    // Pre-generate orders
    struct OParam { uint64_t id; double price; uint32_t qty; Side side; };
    std::vector<OParam> orders;
    orders.reserve(N);
    for (int i = 0; i < N; ++i) {
        double price = 99.00 + tick_dist(rng) * TICK;
        orders.push_back({(uint64_t)(i + 1), price, 100u,
                          (i % 2 == 0) ? Side::BID : Side::ASK});
    }

    OrderBook book;
    std::vector<int64_t> latencies;
    latencies.reserve(N);

    auto wall_start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        auto& o = orders[i];
        auto t0 = std::chrono::high_resolution_clock::now();
        book.addOrder(o.id, (uint64_t)i, o.price, o.qty, o.side);
        auto t1 = std::chrono::high_resolution_clock::now();
        latencies.push_back(
            std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count());
    }

    auto wall_end = std::chrono::high_resolution_clock::now();
    double elapsed_s = std::chrono::duration<double>(wall_end - wall_start).count();

    std::sort(latencies.begin(), latencies.end());
    int64_t median_ns = latencies[N / 2];

    std::printf("Orders inserted : %d\n", N);
    std::printf("Matched orders  : %llu\n", (unsigned long long)book.matchedOrders());
    std::printf("Elapsed         : %.3f s\n", elapsed_s);
    std::printf("Throughput      : %.0f orders/sec\n", N / elapsed_s);
    std::printf("Median latency  : %lld ns\n", (long long)median_ns);
    std::printf("p99 latency     : %lld ns\n", (long long)latencies[(int)(N * 0.99)]);
}
