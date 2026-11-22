/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  SpscRingBuffer.hpp
 * @brief Definition of lock-free SPSC FIFO ring buffer.
 */

#pragma once

#include <emmintrin.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include <proto/Types.hpp>

namespace pulse::bus {

template <typename T>
concept TriviallyCopyablePOD = std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T> &&
                               std::is_default_constructible_v<T>;

template <TriviallyCopyablePOD T, std::size_t Capacity>
class SpscRingBuffer {
public:
    template <typename... Args>
        requires std::constructible_from<T, Args...>
    [[nodiscard]] bool tryEmplace(Args&&... args) noexcept {
        const auto currWriteIdx = writeIdx.load(std::memory_order::relaxed);
        const auto currReadIdx = readIdx.load(std::memory_order::acquire);

        if (currWriteIdx - currReadIdx >= Capacity)
            return false;

        const auto slot = currWriteIdx & INDEX_MASK;
        std::construct_at(&buffer[slot], std::forward<Args>(args)...);

        writeIdx.store(currWriteIdx + 1, std::memory_order::release);

        return true;
    }

    [[nodiscard]] bool tryPush(const T& item) noexcept { return tryEmplace(item); }

    [[nodiscard]] bool tryPush(T&& item) noexcept { return tryEmplace(std::move(item)); }

    template <typename... Args>
        requires std::constructible_from<T, Args...>
    void emplace(Args&&... args) noexcept {
        while (!tryEmplace(std::forward<Args>(args)...))
            _mm_pause();
    }

    void push(const T& item) noexcept {
        while (!tryPush(item))
            _mm_pause();
    }

    void push(T&& item) noexcept {
        while (!tryPush(std::move(item)))
            _mm_pause();
    }

    [[nodiscard]] bool tryPop(T& item) noexcept {
        const auto currReadIdx = readIdx.load(std::memory_order::relaxed);
        const auto currWriteIdx = writeIdx.load(std::memory_order::acquire);

        if (currReadIdx == currWriteIdx)
            return false;

        const auto slot = currReadIdx & INDEX_MASK;
        item = buffer[slot];

        readIdx.store(currReadIdx + 1, std::memory_order::release);

        return true;
    }

    void pop(T& item) noexcept {
        while (!tryPop(item))
            _mm_pause();
    }

    [[nodiscard]] const T* peek() const noexcept {
        const auto currReadIdx = readIdx.load(std::memory_order::relaxed);
        const auto currWriteIdx = writeIdx.load(std::memory_order::acquire);

        if (currReadIdx == currWriteIdx)
            return nullptr;

        const auto slot = currReadIdx & INDEX_MASK;
        return &buffer[slot];
    }

    void consume() noexcept {
        const auto currReadIdx = readIdx.load(std::memory_order::relaxed);
        assert(currReadIdx != writeIdx.load(std::memory_order::relaxed));
        readIdx.store(currReadIdx + 1, std::memory_order::release);
    }

    [[nodiscard]] std::size_t size() const noexcept {
        const auto currReadIdx = readIdx.load(std::memory_order::acquire);
        const auto currWriteIdx = writeIdx.load(std::memory_order::acquire);

        if (currWriteIdx < currReadIdx) [[unlikely]]
            return 0;

        return std::min(currWriteIdx - currReadIdx, Capacity);
    }

    [[nodiscard]] bool empty() const noexcept { return size() == 0; }

    [[nodiscard]] bool full() const noexcept { return size() >= Capacity; }

    [[nodiscard]] static constexpr std::size_t capacity() noexcept { return Capacity; }

private:
    alignas(proto::CACHE_LINE_SIZE) std::atomic<proto::RingBufferIndex> writeIdx{0};
    alignas(proto::CACHE_LINE_SIZE) std::atomic<proto::RingBufferIndex> readIdx{0};
    alignas(proto::CACHE_LINE_SIZE) std::array<T, Capacity> buffer{};

    static constexpr std::size_t INDEX_MASK = Capacity - 1;

    static_assert(std::has_single_bit(Capacity), "Capacity must be a power of two");
    static_assert(std::atomic<proto::RingBufferIndex>::is_always_lock_free,
                  "Ring buffer index type must be lock-free");
    static_assert(proto::CACHE_LINE_SIZE % sizeof(T) == 0 ||
                      sizeof(T) % proto::CACHE_LINE_SIZE == 0,
                  "T doesn't prevent cache-line straddling");
};

} // namespace pulse::bus
