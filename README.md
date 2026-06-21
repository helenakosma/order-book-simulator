# Order Book Simulator

A high-performance limit order book written in C++17, designed for low-latency simulation and benchmarking.

## Features

- **Price-time priority matching** — orders within a price level are matched FIFO
- **O(log P) add/cancel** where P is the number of distinct price levels (backed by `std::map`)
- **O(1) cancel by ID** via a hash-map index
- **Slab allocator** — all `Order` objects come from a pre-allocated pool of 1 million slots, eliminating heap allocation on the hot path
- Each `Order` is padded to fit within a single 64-byte cache line

## Project Layout

```
include/
  order.h          # Order and PriceLevel structs
  order_book.h     # OrderBook class interface
  slab_alloc.h     # Fixed-size slab allocator
src/
  order_book.cpp   # OrderBook implementation
bench/
  bench.cpp        # Throughput / latency benchmark (1M orders)
tests/
  test_book.cpp    # Unit tests
Makefile
```

## Build

Requires g++ with C++17 support.

```bash
make          # builds both bench_run and test_run
make bench    # benchmark binary only
make test     # test binary only
```

## Run

```bash
./bench_run   # prints throughput, median latency, p99 latency
./test_run    # runs unit tests
```

## API

```cpp
OrderBook book;

// Add a limit order (triggers matching immediately)
book.addOrder(id, timestamp, price, quantity, Side::BID);
book.addOrder(id, timestamp, price, quantity, Side::ASK);

// Cancel by ID — returns false if not found
book.cancelOrder(id);

// Query top of book
std::optional<double> bid = book.getBestBid();
std::optional<double> ask = book.getBestAsk();

// Total match events so far
uint64_t n = book.matchedOrders();
```

## Design Notes

The slab allocator (`SlabAlloc<Order, 1'000'000>`) holds a pool of 1M pre-constructed `Order` objects and a free-stack for O(1) alloc/free. This keeps order insertion off the general heap. The pool is large enough for the default benchmark workload; increase `SLAB_SIZE` in `order_book.h` for larger simulations.

Matching runs immediately after each `addOrder` call, consuming as many crossing bid/ask pairs as possible before returning.
