/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  OrderBook.cpp
 * @brief Implementation of the OrderBook class.
 */

#include "OrderBook.hpp"

#include <algorithm>
#include <stdexcept>

namespace engine {

OrderBook::OrderBook(std::size_t maxOrders, Price maxPrice)
    : bidBits(maxPrice), askBits(maxPrice) {
    ordersPool.reserve(maxOrders);
    locations.resize(maxOrders, INVALID_INDEX);

    bids.resize(maxPrice + 1);
    asks.resize(maxPrice + 1);

    currBestBid = 0;
    currBestAsk = INVALID_PRICE;
    firstFreeIdx = INVALID_INDEX;
}

void OrderBook::insert(const Order& order, Side side) {
    OrderIndex idx = allocateOrder(order);
    locations[order.orderId] = idx;

    if (side == Side::Buy) {
        PriceLevel& level = bids[order.price];

        if (level.headIdx == INVALID_INDEX) {
            level.headIdx = idx;
            level.tailIdx = idx;
            bidBits.setPrice(order.price);
        } else {
            ordersPool[level.tailIdx].nextOrderIdx = idx;
            level.tailIdx = idx;
        }

        if (currBestBid == INVALID_PRICE || order.price > currBestBid)
            currBestBid = order.price;
    } else {
        PriceLevel& level = asks[order.price];

        if (level.headIdx == INVALID_INDEX) {
            level.headIdx = idx;
            level.tailIdx = idx;
            askBits.setPrice(order.price);
        } else {
            ordersPool[level.tailIdx].nextOrderIdx = idx;
            level.tailIdx = idx;
        }

        if (currBestAsk == INVALID_PRICE || order.price < currBestAsk)
            currBestAsk = order.price;
    }
}

void OrderBook::cancelPartial(OrderId orderId, Quantity cancelQty) {
    OrderIndex idx = locations[orderId];
    if (idx == INVALID_INDEX)
        return;

    Order& poolOrder = ordersPool[idx];

    if (poolOrder.quantity > cancelQty) {
        poolOrder.quantity -= cancelQty;
    } else {
        poolOrder.quantity = 0;
        locations[orderId] = INVALID_INDEX;
    }
}

void OrderBook::cancelFull(OrderId orderId) {
    OrderIndex idx = locations[orderId];
    if (idx == INVALID_INDEX)
        return;

    ordersPool[idx].quantity = 0;
    locations[orderId] = INVALID_INDEX;
}

void OrderBook::eraseLocation(OrderId orderId) {
    if (orderId < locations.size())
        locations[orderId] = INVALID_INDEX;
}

Order* OrderBook::bestBid() {
    while (currBestBid != INVALID_PRICE && currBestBid > 0) {
        PriceLevel& level = bids[currBestBid];

        while (level.headIdx != INVALID_INDEX) {
            Order& headOrder = ordersPool[level.headIdx];

            if (headOrder.quantity == 0) {
                OrderIndex deadIdx = level.headIdx;
                level.headIdx = headOrder.nextOrderIdx;

                if (level.headIdx == INVALID_INDEX)
                    level.tailIdx = INVALID_INDEX;

                freeOrder(deadIdx);
            } else {
                return &headOrder;
            }
        }

        bidBits.clearPrice(currBestBid);

        if (currBestBid == 0)
            break;

        currBestBid = bidBits.findNextBid(currBestBid - 1);
    }

    return nullptr;
}

Order* OrderBook::bestAsk() {
    while (currBestAsk != INVALID_PRICE) {
        PriceLevel& level = asks[currBestAsk];

        while (level.headIdx != INVALID_INDEX) {
            Order& headOrder = ordersPool[level.headIdx];

            if (headOrder.quantity == 0) {
                OrderIndex deadIdx = level.headIdx;
                level.headIdx = headOrder.nextOrderIdx;

                if (level.headIdx == INVALID_INDEX)
                    level.tailIdx = INVALID_INDEX;

                freeOrder(deadIdx);
            } else {
                return &headOrder;
            }
        }

        askBits.clearPrice(currBestAsk);

        currBestAsk = askBits.findNextAsk(currBestAsk + 1);
    }

    return nullptr;
}

std::vector<OrderBookLevel> OrderBook::bookState(std::size_t depth) const {
    if (depth == 0)
        return {};

    std::vector<OrderBookLevel> state;
    state.reserve(depth);

    Price scanBid = currBestBid;
    Price scanAsk = currBestAsk;

    for (std::size_t i = 0; i < depth; ++i) {
        OrderBookLevel currLevel{};
        bool foundBid = false;
        bool foundAsk = false;

        while (scanBid != INVALID_PRICE && scanBid > 0 && !foundBid) {
            const PriceLevel& level = bids[scanBid];
            Quantity totalQty = 0;

            OrderIndex currIdx = level.headIdx;
            while (currIdx != INVALID_INDEX) {
                totalQty += ordersPool[currIdx].quantity;
                currIdx = ordersPool[currIdx].nextOrderIdx;
            }

            if (totalQty > 0) {
                currLevel.bidPrice = scanBid;
                currLevel.bidQty = totalQty;
                foundBid = true;
            }

            if (scanBid == 0)
                break;

            scanBid = bidBits.findNextBid(scanBid - 1);
        }

        while (scanAsk != INVALID_PRICE && !foundAsk) {
            const PriceLevel& level = asks[scanAsk];
            Quantity totalQty = 0;

            OrderIndex currIdx = level.headIdx;
            while (currIdx != INVALID_INDEX) {
                totalQty += ordersPool[currIdx].quantity;
                currIdx = ordersPool[currIdx].nextOrderIdx;
            }

            if (totalQty > 0) {
                currLevel.askPrice = scanAsk;
                currLevel.askQty = totalQty;
                foundAsk = true;
            }

            scanAsk = askBits.findNextAsk(scanAsk + 1);
        }

        if (foundBid || foundAsk) {
            state.push_back(currLevel);
        } else {
            state.resize(depth);
            break;
        }
    }

    return state;
}

OrderIndex OrderBook::allocateOrder(const Order& order) {
    if (firstFreeIdx != INVALID_INDEX) {
        OrderIndex idx = firstFreeIdx;
        firstFreeIdx = ordersPool[idx].nextOrderIdx;

        ordersPool[idx] = order;
        ordersPool[idx].nextOrderIdx = INVALID_INDEX;

        return idx;
    }

    if (ordersPool.size() >= ordersPool.capacity())
        throw std::runtime_error("OrderBook memory pool exhausted!");

    ordersPool.push_back(order);
    ordersPool.back().nextOrderIdx = INVALID_INDEX;
    return static_cast<OrderIndex>(ordersPool.size() - 1);
}

void OrderBook::freeOrder(OrderIndex index) {
    ordersPool[index].nextOrderIdx = firstFreeIdx;
    firstFreeIdx = index;
}

} // namespace engine
