/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Application.cpp
 * @brief Implementation of the Application class.
 */

#include "Application.hpp"

#include <pthread.h>
#include <sys/mman.h>
#include <x86intrin.h>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <print>
#include <thread>

#include "LobsterOrderLoader.hpp"
#include "OrderGenerator.hpp"
#include "ResultWriter.hpp"

namespace engine {

constexpr int WARMUP_RUNS = 1;
constexpr int BENCHMARK_RUNS = 10;
constexpr int TOTAL_RUNS = WARMUP_RUNS + BENCHMARK_RUNS;
constexpr std::size_t MAX_ORDERS = 10'000'000;
constexpr Price MAX_PRICE = 100'000'000;

Application::Application(const Config& cfg) : config(cfg) {
    idMapper.emplace(MAX_ORDERS);
}

void Application::run() {
    applyIsolation();
    prepareOrders();

    trades.reserve(orderMsgs.size());
    if (config.measureLatency)
        latencies.reserve(orderMsgs.size());

    std::println("Running {} iterations...\n", TOTAL_RUNS);

    double totalWarmedUpDurationMs = 0.0;

    for (int i = 1; i <= TOTAL_RUNS; ++i) {
        trades.clear();

        if (config.measureLatency)
            latencies.clear();

        matchingEngine.emplace(MAX_ORDERS, MAX_PRICE);
        double durationS = matchOrders();
        double durationMs = durationS * 1000.0;

        if (i <= WARMUP_RUNS) {
            std::println("Warmup:        {:.2f} ms", durationMs);
        } else {
            int benchRun = i - WARMUP_RUNS;
            std::println("[{:>2}/{}] Bench: {:.2f} ms", benchRun, BENCHMARK_RUNS, durationMs);
            totalWarmedUpDurationMs += durationMs;
        }
    }

    double avgDurationMs = totalWarmedUpDurationMs / BENCHMARK_RUNS;
    double avgThroughput = static_cast<double>(orderMsgs.size()) / (avgDurationMs / 1000.0);

    std::vector<uint64_t> latenciesNs;
    if (config.measureLatency && !latencies.empty()) {
        double cyclesPerNs = countCyclesPerNs();
        latenciesNs.reserve(latencies.size());

        for (uint64_t rawCycles : latencies) {
            latenciesNs.push_back(
                static_cast<uint64_t>(static_cast<double>(rawCycles) / cyclesPerNs));
        }

        std::ranges::sort(latenciesNs);
    }

    ResultWriter::printSummary(avgDurationMs, avgThroughput, orderMsgs.size(), trades.size(),
                               latenciesNs);

    if (!config.tradeFile.empty()) {
        auto* idMapperPtr = config.orderFile.empty() ? nullptr : &(*idMapper);
        ResultWriter::writeTrades(trades, config.tradeFile, idMapperPtr);
    }
}

void Application::prepareOrders() {
    if (!config.orderFile.empty()) {
        std::println("Loading orders from file: {}", config.orderFile);
        orderMsgs = LobsterOrderLoader::load(config.orderFile, *idMapper);
    } else {
        std::println("Generating {} orders (seed: {})", config.genCount, config.genSeed);
        orderMsgs =
            OrderGenerator::generate(config.genSeed, config.genCount, config.genCancelPercent,
                                     config.genBasePrice, config.genMeanOffset, config.genStdDev);
    }

    if (orderMsgs.empty()) {
        std::println(stderr, "Couldn't load or generate any data");
        exit(1);
    }
}

void Application::applyIsolation() const {
    if (config.lockMemory) {
        if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
            std::println(stderr, "Warning: Failed to lock memory");
        else
            std::println("Memory locked successfully");
    }

    if (config.cpuCore >= 0) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(static_cast<std::size_t>(config.cpuCore), &cpuset);

        if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
            std::println(stderr, "Warning: Failed to pin thread to core {}", config.cpuCore);
        else
            std::println("Thread successfully pinned thread to core {}", config.cpuCore);
    }
}

double Application::matchOrders() {
    auto matchingStart = std::chrono::steady_clock::now();

    if (config.measureLatency) {
        for (const auto& order : orderMsgs) {
            _mm_lfence();
            uint64_t tickStart = __rdtsc();
            _mm_lfence();

            matchingEngine->processOrder(order, trades);

            unsigned int aux;
            uint64_t tickEnd = __rdtscp(&aux);
            _mm_lfence();

            latencies.push_back(tickEnd - tickStart);
        }
    } else {
        for (const auto& order : orderMsgs)
            matchingEngine->processOrder(order, trades);
    }

    auto matchingEnd = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(matchingEnd - matchingStart).count();
}

double Application::countCyclesPerNs() {
    _mm_lfence();
    uint64_t startCycles = __rdtsc();
    _mm_lfence();

    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    auto end = std::chrono::high_resolution_clock::now();

    _mm_lfence();
    unsigned int aux;
    uint64_t endCycles = __rdtscp(&aux);
    _mm_lfence();

    uint64_t cycles = endCycles - startCycles;
    int64_t elapsedNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    return static_cast<double>(cycles) / static_cast<double>(elapsedNs);
}

} // namespace engine
