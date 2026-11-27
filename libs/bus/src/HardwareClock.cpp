/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 */

#include <bus/HardwareClock.hpp>

#include <x86intrin.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <thread>

namespace pulse::bus {

namespace {

constexpr std::size_t CALIBRATION_SAMPLES = 5;
constexpr auto CALIBRATION_WINDOW = std::chrono::milliseconds(5);
constexpr double FALLBACK_NS_PER_CYCLE = 1.0;

static_assert(CALIBRATION_SAMPLES % 2 == 1);

[[nodiscard]] double calibrateNsPerCycle() noexcept {
    std::array<double, CALIBRATION_SAMPLES> samples{};

    for (double& sample : samples) {
        const auto startCycles = HardwareClock::startMeasure();
        const auto startTime = std::chrono::steady_clock::now();

        std::this_thread::sleep_for(CALIBRATION_WINDOW);

        const auto endTime = std::chrono::steady_clock::now();
        const auto endCycles = HardwareClock::stopMeasure();

        const auto elapsedNs =
            std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();

        sample = (endCycles > startCycles && elapsedNs > 0)
                     ? static_cast<double>(elapsedNs) / static_cast<double>(endCycles - startCycles)
                     : FALLBACK_NS_PER_CYCLE;
    }

    // Use the median sample to filter out OS scheduling jitter during startup
    std::ranges::sort(samples);
    return samples[CALIBRATION_SAMPLES / 2];
}

} // namespace

HardwareClock& HardwareClock::instance() noexcept {
    static HardwareClock clockInstance;
    return clockInstance;
}

proto::CpuCycles HardwareClock::readCycles() noexcept {
    return static_cast<proto::CpuCycles>(__rdtsc());
}

proto::CpuCycles HardwareClock::startMeasure() noexcept {
    _mm_lfence();
    const auto tickStart = static_cast<proto::CpuCycles>(__rdtsc());
    _mm_lfence();

    return tickStart;
}

proto::CpuCycles HardwareClock::stopMeasure() noexcept {
    unsigned int aux = 0;
    const auto tickEnd = static_cast<proto::CpuCycles>(__rdtscp(&aux));
    _mm_lfence();

    return tickEnd;
}

std::uint64_t HardwareClock::cyclesToNs(proto::CpuCycles cycles) const noexcept {
    const double ns = static_cast<double>(cycles) * calibratedNsPerCycle;
    return static_cast<std::uint64_t>(std::round(ns));
}

double HardwareClock::nsPerCycle() const noexcept {
    return calibratedNsPerCycle;
}

HardwareClock::HardwareClock() noexcept : calibratedNsPerCycle(calibrateNsPerCycle()) {}

} // namespace pulse::bus
