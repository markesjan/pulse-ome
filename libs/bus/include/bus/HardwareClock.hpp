/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  HardwareClock.hpp
 * @brief Definition of hardware clock singleton for precise measuring.
 */

#pragma once

#include <cstdint>

#include <proto/Types.hpp>

namespace pulse::bus {

class HardwareClock {
public:
    static HardwareClock& instance() noexcept;

    HardwareClock(const HardwareClock&) = delete;
    HardwareClock& operator=(const HardwareClock&) = delete;
    HardwareClock(HardwareClock&&) = delete;
    HardwareClock& operator=(HardwareClock&&) = delete;

    [[nodiscard]] static proto::CpuCycles readCycles() noexcept;
    [[nodiscard]] static proto::CpuCycles startMeasure() noexcept;
    [[nodiscard]] static proto::CpuCycles stopMeasure() noexcept;

    [[nodiscard]] std::uint64_t cyclesToNs(proto::CpuCycles cycles) const noexcept;
    [[nodiscard]] double nsPerCycle() const noexcept;

private:
    HardwareClock() noexcept;
    ~HardwareClock() = default;

    double calibratedNsPerCycle{0.0};
};

} // namespace pulse::bus
