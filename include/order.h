#pragma once
#include <cstdint>

enum class Side : uint8_t { BID, ASK };

struct Order {
    uint64_t id;
    uint64_t timestamp;
    double   price;
    uint32_t quantity;
    Side     side;
    uint8_t  _pad[3];
    Order*   prev;
    Order*   next;
};

static_assert(sizeof(Order) <= 64, "Order exceeds 64 bytes");

struct PriceLevel {
    double   price;
    uint32_t total_qty;
    Order*   head;
    Order*   tail;
};
