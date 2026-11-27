/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 */

#include <gtest/gtest.h>

#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <thread>
#include <type_traits>

#include <bus/HardwareClock.hpp>
#include <proto/Types.hpp>

namespace pulse::bus::test {

static_assert(!std::is_default_constructible_v<HardwareClock>);
static_assert(!std::is_copy_constructible_v<HardwareClock>);
static_assert(!std::is_copy_assignable_v<HardwareClock>);
static_assert(!std::is_move_constructible_v<HardwareClock>);
static_assert(!std::is_move_assignable_v<HardwareClock>);

TEST(HardwareClockTest, SingletonReturnsSameInstance) {
    const auto& first = HardwareClock::instance();
    const auto& second = HardwareClock::instance();

    EXPECT_EQ(&first, &second);
}

TEST(HardwareClockTest, SingletonIsThreadSafe) {
    constexpr std::size_t THREAD_COUNT = 4;
    std::array<const HardwareClock*, THREAD_COUNT> instances{};

    {
        std::array<std::jthread, THREAD_COUNT> threads;
        for (std::size_t i = 0; i < THREAD_COUNT; ++i) {
            threads[i] =
                std::jthread([&instances, i] { instances[i] = &HardwareClock::instance(); });
        }
    }

    const auto* expected = &HardwareClock::instance();
    for (const auto* ptr : instances)
        EXPECT_EQ(ptr, expected);
}

TEST(HardwareClockTest, CalibratedNsPerCycleInReasonableBounds) {
    const double nsPerCycle = HardwareClock::instance().nsPerCycle();

    EXPECT_TRUE(std::isfinite(nsPerCycle));
    EXPECT_GT(nsPerCycle, 0.05);
    EXPECT_LT(nsPerCycle, 10.0);
}

TEST(HardwareClockTest, ReadCyclesAdvancesOverTime) {
    const auto t1 = HardwareClock::readCycles();
    const auto t2 = HardwareClock::readCycles();
    EXPECT_GE(t2, t1);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    const auto t3 = HardwareClock::readCycles();
    EXPECT_GT(t3, t1);
}

TEST(HardwareClockTest, StartAndStopMeasureBracketing) {
    const auto immediateStart = HardwareClock::startMeasure();
    const auto immediateStop = HardwareClock::stopMeasure();
    EXPECT_GE(immediateStop, immediateStart);

    const auto start = HardwareClock::startMeasure();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    const auto end = HardwareClock::stopMeasure();
    EXPECT_GT(end, start);
}

TEST(HardwareClockTest, CyclesToNsConversion) {
    const auto& clock = HardwareClock::instance();

    EXPECT_EQ(clock.cyclesToNs(0), 0ULL);

    constexpr proto::CpuCycles SAMPLE_CYCLES = 1'000'000ULL;
    const auto expectedNs = static_cast<std::uint64_t>(
        std::round(static_cast<double>(SAMPLE_CYCLES) * clock.nsPerCycle()));
    EXPECT_EQ(clock.cyclesToNs(SAMPLE_CYCLES), expectedNs);

    EXPECT_LE(clock.cyclesToNs(10'000ULL), clock.cyclesToNs(20'000ULL));
}

TEST(HardwareClockTest, MatchesSteadyClockDurationWithinTolerance) {
    const auto startCycles = HardwareClock::startMeasure();
    const auto startTime = std::chrono::steady_clock::now();

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    const auto endTime = std::chrono::steady_clock::now();
    const auto endCycles = HardwareClock::stopMeasure();

    ASSERT_GT(endCycles, startCycles);

    const auto wallNs =
        std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
    ASSERT_GT(wallNs, 0);

    const auto tscNs = HardwareClock::instance().cyclesToNs(endCycles - startCycles);
    const auto wallNsDouble = static_cast<double>(wallNs);

    EXPECT_NEAR(static_cast<double>(tscNs), wallNsDouble, wallNsDouble * 0.20);
}

} // namespace pulse::bus::test
