/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  SpscRingBuffer.hpp
 * @brief Definition of lock-free SPSC FIFO ring buffer.
 */

#pragma once

#include <array>
#include <atomic>
#include <bit>
#include <cstddef>
#include <type_traits>

#include <proto/Types.hpp>

namespace pulse::bus {

template <typename T, std::size_t Capacity>
    requires std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T> &&
             std::is_default_constructible_v<T>
class SpscRingBuffer {
public:
    bool try_push(const T& item) noexcept;
    bool try_pop(T& item) noexcept;

    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] bool full() const noexcept;

    [[nodiscard]] static constexpr std::size_t capacity() noexcept { return Capacity; }

private:
    alignas(proto::CACHE_LINE_SIZE) std::atomic<proto::RingBufferIndex> writeIdx{0};
    alignas(proto::CACHE_LINE_SIZE) std::atomic<proto::RingBufferIndex> readIdx{0};
    alignas(proto::CACHE_LINE_SIZE) std::array<T, Capacity> buffer{};

    static_assert(std::has_single_bit(Capacity), "Capacity must be a power of two");
    static_assert(std::atomic<proto::RingBufferIndex>::is_always_lock_free,
                  "Ring buffer index type must be lock-free");
    static_assert(proto::CACHE_LINE_SIZE % sizeof(T) == 0 ||
                      sizeof(T) % proto::CACHE_LINE_SIZE == 0,
                  "T doesn't prevent cache-line straddling");
};

} // namespace pulse::bus
