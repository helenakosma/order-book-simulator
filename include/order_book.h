#pragma once
#include "order.h"
#include "slab_alloc.h"
#include <map>
#include <unordered_map>
#include <functional>
#include <optional>

static constexpr std::size_t SLAB_SIZE = 1'000'000;

class OrderBook {
public:
    OrderBook();

    void addOrder(uint64_t id, uint64_t timestamp, double price,
                  uint32_t quantity, Side side);
    bool cancelOrder(uint64_t id);
    void match();

    std::optional<double> getBestBid() const;
    std::optional<double> getBestAsk() const;

    uint64_t matchedOrders() const { return matched_count_; }

private:
    void insertIntoLevel(Order* o);
    void removeFromLevel(Order* o);
    PriceLevel& getOrCreateLevel(double price, Side side);

    using BidMap = std::map<double, PriceLevel, std::greater<double>>;
    using AskMap = std::map<double, PriceLevel>;

    BidMap bids_;
    AskMap asks_;
    std::unordered_map<uint64_t, Order*> index_;

    SlabAlloc<Order, SLAB_SIZE> alloc_;
    uint64_t matched_count_ = 0;
};
