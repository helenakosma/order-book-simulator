#include "order_book.h"
#include <cassert>
#include <cstdio>

static int passed = 0, failed = 0;
#define CHECK(expr) do { \
    if (expr) { ++passed; } \
    else { ++failed; std::printf("FAIL: %s (line %d)\n", #expr, __LINE__); } \
} while(0)

int main() {
    // Test 1: empty book
    {
        OrderBook book;
        CHECK(!book.getBestBid().has_value());
        CHECK(!book.getBestAsk().has_value());
    }

    // Test 2: best bid/ask after inserts
    {
        OrderBook book;
        book.addOrder(1, 0, 100.00, 10, Side::BID);
        book.addOrder(2, 1, 100.50, 10, Side::BID);
        book.addOrder(3, 2, 101.00, 10, Side::ASK);
        book.addOrder(4, 3, 101.50, 10, Side::ASK);
        CHECK(book.getBestBid().value() == 100.50);
        CHECK(book.getBestAsk().value() == 101.00);
    }

    // Test 3: cancel order
    {
        OrderBook book;
        book.addOrder(1, 0, 100.00, 10, Side::BID);
        book.addOrder(2, 1, 100.50, 10, Side::BID);
        CHECK(book.getBestBid().value() == 100.50);
        CHECK(book.cancelOrder(2));
        CHECK(book.getBestBid().value() == 100.00);
        CHECK(!book.cancelOrder(99)); // non-existent
    }

    // Test 4: matching — crossing bid/ask
    {
        OrderBook book;
        book.addOrder(1, 0, 101.00, 10, Side::ASK);
        book.addOrder(2, 1, 101.00, 10, Side::BID); // crosses
        CHECK(book.matchedOrders() >= 1);
        // Both fully matched, book should be empty
        CHECK(!book.getBestBid().has_value());
        CHECK(!book.getBestAsk().has_value());
    }

    // Test 5: partial fill
    {
        OrderBook book;
        book.addOrder(1, 0, 100.00, 5,  Side::ASK);
        book.addOrder(2, 1, 100.00, 10, Side::BID);
        // ask fully consumed, bid has 5 remaining
        CHECK(!book.getBestAsk().has_value());
        CHECK(book.getBestBid().value() == 100.00);
    }

    // Test 6: FIFO queue within price level
    {
        OrderBook book;
        book.addOrder(1, 0, 100.00, 5, Side::BID);
        book.addOrder(2, 1, 100.00, 5, Side::BID);
        // cancel first one
        CHECK(book.cancelOrder(1));
        // now add matching ask — should match with order 2
        book.addOrder(3, 2, 100.00, 5, Side::ASK);
        CHECK(!book.getBestBid().has_value());
        CHECK(!book.getBestAsk().has_value());
    }

    std::printf("\n%d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
