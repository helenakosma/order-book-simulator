CXX      := g++
CXXFLAGS := -std=c++17 -O3 -Wall -march=native -I include

SRC      := src/order_book.cpp
BENCH    := bench/bench.cpp
TEST     := tests/test_book.cpp

all: bench test

bench: $(SRC) $(BENCH)
	$(CXX) $(CXXFLAGS) $(SRC) $(BENCH) -o bench_run

test: $(SRC) $(TEST)
	$(CXX) $(CXXFLAGS) $(SRC) $(TEST) -o test_run

run_bench: bench
	./bench_run

run_test: test
	./test_run

clean:
	rm -f bench_run test_run
