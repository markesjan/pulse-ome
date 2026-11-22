/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  SpscRingBufferTest.cpp
 * @brief Unit tests for lock-free SPSC FIFO ring buffer.
 */

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <thread>
#include <utility>

#include <bus/SpscRingBuffer.hpp>
#include <proto/Messages.hpp>
#include <proto/Types.hpp>

namespace pulse::bus::test {

TEST(SpscRingBufferTest, InitialStateAndCapacity) {
    constexpr std::size_t CAPACITY = 8;
    SpscRingBuffer<int, CAPACITY> ring;

    EXPECT_TRUE(ring.empty());
    EXPECT_FALSE(ring.full());
    EXPECT_EQ(ring.size(), 0U);
    EXPECT_EQ(ring.capacity(), CAPACITY);
    static_assert(SpscRingBuffer<int, CAPACITY>::capacity() == CAPACITY);
}

TEST(SpscRingBufferTest, UnderflowOnEmpty) {
    SpscRingBuffer<int, 4> ring;
    int value = 42;

    EXPECT_FALSE(ring.tryPop(value));
    EXPECT_EQ(value, 42);
    EXPECT_EQ(ring.peek(), nullptr);
    EXPECT_TRUE(ring.empty());
    EXPECT_EQ(ring.size(), 0U);
}

TEST(SpscRingBufferTest, FifoOrderingAndFullBoundary) {
    constexpr std::size_t CAPACITY = 4;
    SpscRingBuffer<int, CAPACITY> ring;

    EXPECT_TRUE(ring.tryPush(10));
    EXPECT_TRUE(ring.tryPush(20));
    EXPECT_TRUE(ring.tryPush(30));
    EXPECT_TRUE(ring.tryPush(40));

    EXPECT_TRUE(ring.full());
    EXPECT_FALSE(ring.empty());
    EXPECT_EQ(ring.size(), CAPACITY);

    EXPECT_FALSE(ring.tryPush(50));
    EXPECT_EQ(ring.size(), CAPACITY);

    int outValue = 0;
    EXPECT_TRUE(ring.tryPop(outValue));
    EXPECT_EQ(outValue, 10);

    EXPECT_TRUE(ring.tryPop(outValue));
    EXPECT_EQ(outValue, 20);

    EXPECT_TRUE(ring.tryPop(outValue));
    EXPECT_EQ(outValue, 30);

    EXPECT_TRUE(ring.tryPop(outValue));
    EXPECT_EQ(outValue, 40);

    EXPECT_TRUE(ring.empty());
    EXPECT_FALSE(ring.full());
    EXPECT_EQ(ring.size(), 0U);
    EXPECT_FALSE(ring.tryPop(outValue));
}

TEST(SpscRingBufferTest, CircularWrapAround) {
    constexpr std::size_t CAPACITY = 4;
    SpscRingBuffer<std::uint32_t, CAPACITY> ring;
    constexpr std::uint32_t ITERATIONS = 1000;

    for (std::uint32_t i = 0; i < ITERATIONS; ++i) {
        const auto item1 = i * 2U;
        const auto item2 = i * 2U + 1U;

        EXPECT_TRUE(ring.tryPush(item1));
        EXPECT_TRUE(ring.tryPush(item2));

        std::uint32_t popped1 = 0;
        std::uint32_t popped2 = 0;

        EXPECT_TRUE(ring.tryPop(popped1));
        EXPECT_EQ(popped1, item1);

        EXPECT_TRUE(ring.tryPop(popped2));
        EXPECT_EQ(popped2, item2);

        EXPECT_TRUE(ring.empty());
    }
}

TEST(SpscRingBufferTest, PeekDoesNotConsume) {
    SpscRingBuffer<int, 4> ring;

    EXPECT_TRUE(ring.tryPush(42));
    EXPECT_EQ(ring.size(), 1U);

    const int* firstPeek = ring.peek();
    ASSERT_NE(firstPeek, nullptr);
    EXPECT_EQ(*firstPeek, 42);
    EXPECT_EQ(ring.size(), 1U);

    const int* secondPeek = ring.peek();
    ASSERT_NE(secondPeek, nullptr);
    EXPECT_EQ(firstPeek, secondPeek);
    EXPECT_EQ(*secondPeek, 42);
    EXPECT_EQ(ring.size(), 1U);

    ring.consume();
    EXPECT_TRUE(ring.empty());
    EXPECT_EQ(ring.size(), 0U);
    EXPECT_EQ(ring.peek(), nullptr);
}

TEST(SpscRingBufferTest, PeekAndConsumeSequence) {
    SpscRingBuffer<int, 4> ring;

    EXPECT_TRUE(ring.tryPush(100));
    EXPECT_TRUE(ring.tryPush(200));
    EXPECT_TRUE(ring.tryPush(300));

    const int* p1 = ring.peek();
    ASSERT_NE(p1, nullptr);
    EXPECT_EQ(*p1, 100);
    ring.consume();

    const int* p2 = ring.peek();
    ASSERT_NE(p2, nullptr);
    EXPECT_EQ(*p2, 200);
    ring.consume();

    const int* p3 = ring.peek();
    ASSERT_NE(p3, nullptr);
    EXPECT_EQ(*p3, 300);
    ring.consume();

    EXPECT_TRUE(ring.empty());
    EXPECT_EQ(ring.peek(), nullptr);
}

TEST(SpscRingBufferTest, EmplaceOrderMessage) {
    SpscRingBuffer<proto::OrderMessage, 8> ring;

    constexpr proto::Timestamp TS = 123456789ULL;
    constexpr proto::OrderId ORDER_ID = 42ULL;
    constexpr proto::Price PRICE = 15025ULL;
    constexpr proto::Quantity QTY = 100U;
    constexpr proto::OrderAction ACTION = proto::OrderAction::New;
    constexpr proto::Side SIDE = proto::Side::Buy;

    EXPECT_TRUE(ring.tryEmplace(TS, ORDER_ID, PRICE, QTY, ACTION, SIDE));
    EXPECT_EQ(ring.size(), 1U);

    proto::OrderMessage out{};
    EXPECT_TRUE(ring.tryPop(out));

    EXPECT_EQ(out.timestamp, TS);
    EXPECT_EQ(out.orderId, ORDER_ID);
    EXPECT_EQ(out.price, PRICE);
    EXPECT_EQ(out.quantity, QTY);
    EXPECT_EQ(out.action, ACTION);
    EXPECT_EQ(out.side, SIDE);
}

TEST(SpscRingBufferTest, RValueMoveSemantics) {
    SpscRingBuffer<proto::OrderMessage, 4> ring;

    proto::OrderMessage msg{
        .timestamp = 987654321ULL,
        .orderId = 88ULL,
        .price = 25000ULL,
        .quantity = 50U,
        .action = proto::OrderAction::Execution,
        .side = proto::Side::Sell,
    };

    EXPECT_TRUE(ring.tryPush(std::move(msg)));

    proto::OrderMessage out{};
    EXPECT_TRUE(ring.tryPop(out));
    EXPECT_EQ(out.orderId, 88ULL);
    EXPECT_EQ(out.action, proto::OrderAction::Execution);
}

TEST(SpscRingBufferTest, BlockingPushPopAndEmplace) {
    SpscRingBuffer<int, 4> ring;

    ring.push(1);
    ring.push(2);
    ring.emplace(3);

    EXPECT_EQ(ring.size(), 3U);

    int val = 0;
    ring.pop(val);
    EXPECT_EQ(val, 1);

    ring.pop(val);
    EXPECT_EQ(val, 2);

    ring.pop(val);
    EXPECT_EQ(val, 3);

    EXPECT_TRUE(ring.empty());
}

TEST(SpscRingBufferTest, MultiThreadedSpscStress) {
    constexpr std::size_t CAPACITY = 1024;
    constexpr std::uint64_t TOTAL_MESSAGES = 1'000'000ULL;

    SpscRingBuffer<std::uint64_t, CAPACITY> ring;

    std::jthread producer([&ring]() {
        for (std::uint64_t i = 0; i < TOTAL_MESSAGES; ++i) {
            ring.push(i);
        }
    });

    std::uint64_t expected = 0;
    std::jthread consumer([&ring, &expected]() {
        for (std::uint64_t i = 0; i < TOTAL_MESSAGES; ++i) {
            std::uint64_t value = 0;
            ring.pop(value);
            EXPECT_EQ(value, expected);
            ++expected;
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(expected, TOTAL_MESSAGES);
    EXPECT_TRUE(ring.empty());
    EXPECT_EQ(ring.size(), 0U);
}

} // namespace pulse::bus::test
