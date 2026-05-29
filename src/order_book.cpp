#include "order_book.h"
#include <stdexcept>

OrderBook::OrderBook() = default;

void OrderBook::addOrder(uint64_t id, uint64_t timestamp, double price,
                         uint32_t quantity, Side side) {
    Order* o = alloc_.alloc();
    o->id        = id;
    o->timestamp = timestamp;
    o->price     = price;
    o->quantity  = quantity;
    o->side      = side;
    o->prev      = nullptr;
    o->next      = nullptr;

    insertIntoLevel(o);
    index_[id] = o;
    match();
}

bool OrderBook::cancelOrder(uint64_t id) {
    auto it = index_.find(id);
    if (it == index_.end()) return false;

    Order* o = it->second;
    removeFromLevel(o);
    index_.erase(it);
    alloc_.free(o);
    return true;
}

void OrderBook::match() {
    while (!bids_.empty() && !asks_.empty()) {
        auto& [bid_price, bid_level] = *bids_.begin();
        auto& [ask_price, ask_level] = *asks_.begin();

        if (bid_price < ask_price) break;

        Order* bid = bid_level.head;
        Order* ask = ask_level.head;

        uint32_t qty = std::min(bid->quantity, ask->quantity);
        bid->quantity -= qty;
        ask->quantity -= qty;
        bid_level.total_qty -= qty;
        ask_level.total_qty -= qty;
        ++matched_count_;

        if (bid->quantity == 0) {
            index_.erase(bid->id);
            removeFromLevel(bid);
            alloc_.free(bid);
        }
        if (ask->quantity == 0) {
            index_.erase(ask->id);
            removeFromLevel(ask);
            alloc_.free(ask);
        }
    }
}

std::optional<double> OrderBook::getBestBid() const {
    if (bids_.empty()) return std::nullopt;
    return bids_.begin()->first;
}

std::optional<double> OrderBook::getBestAsk() const {
    if (asks_.empty()) return std::nullopt;
    return asks_.begin()->first;
}

void OrderBook::insertIntoLevel(Order* o) {
    if (o->side == Side::BID) {
        auto& level = bids_[o->price];
        level.price = o->price;
        level.total_qty += o->quantity;
        if (!level.tail) {
            level.head = level.tail = o;
            o->prev = o->next = nullptr;
        } else {
            o->prev = level.tail;
            o->next = nullptr;
            level.tail->next = o;
            level.tail = o;
        }
    } else {
        auto& level = asks_[o->price];
        level.price = o->price;
        level.total_qty += o->quantity;
        if (!level.tail) {
            level.head = level.tail = o;
            o->prev = o->next = nullptr;
        } else {
            o->prev = level.tail;
            o->next = nullptr;
            level.tail->next = o;
            level.tail = o;
        }
    }
}

void OrderBook::removeFromLevel(Order* o) {
    if (o->side == Side::BID) {
        auto it = bids_.find(o->price);
        if (it == bids_.end()) return;
        PriceLevel& level = it->second;
        level.total_qty -= o->quantity;
        if (o->prev) o->prev->next = o->next;
        else         level.head = o->next;
        if (o->next) o->next->prev = o->prev;
        else         level.tail = o->prev;
        if (!level.head) bids_.erase(it);
    } else {
        auto it = asks_.find(o->price);
        if (it == asks_.end()) return;
        PriceLevel& level = it->second;
        level.total_qty -= o->quantity;
        if (o->prev) o->prev->next = o->next;
        else         level.head = o->next;
        if (o->next) o->next->prev = o->prev;
        else         level.tail = o->prev;
        if (!level.head) asks_.erase(it);
    }
}
