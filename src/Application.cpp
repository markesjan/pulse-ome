#include "Application.hpp"

#include <algorithm>
#include <chrono>
#include <x86intrin.h>
#include <cstdio>
#include <print>
#include <thread>

#include "LobsterOrderLoader.hpp"
#include "OrderGenerator.hpp"
#include "ResultWriter.hpp"

namespace engine {

Application::Application(const Config& config) : config(config) {
}

void Application::run() {
    prepareOrders();

    trades.reserve(orders.size());
    if (config.measureLatency)
        latencies.reserve(orders.size());

    constexpr int WARMUP_RUNS = 1;
    constexpr int BENCHMARK_RUNS = 10;
    constexpr int TOTAL_RUNS = WARMUP_RUNS + BENCHMARK_RUNS;

    std::println("Running {} iterations...\n", TOTAL_RUNS);

    double totalWarmedUpDurationMs = 0.0;

    for (int i = 1; i <= TOTAL_RUNS; ++i) {
        trades.clear();

        if (config.measureLatency)
            latencies.clear();

        matchingEngine = MatchingEngine();
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
    double avgThroughput = static_cast<double>(orders.size()) / (avgDurationMs / 1000.0);

    std::vector<uint64_t> latenciesNs;
    if (config.measureLatency && !latencies.empty()) {
        double cyclesPerNs = getCyclesPerNs();
        latenciesNs.reserve(latencies.size());

        for (uint64_t rawCycles : latencies) {
            latenciesNs.push_back(
                static_cast<uint64_t>(static_cast<double>(rawCycles) / cyclesPerNs));
        }

        std::ranges::sort(latenciesNs);
    }

    ResultWriter::printSummary(avgDurationMs, avgThroughput, orders.size(), trades.size(),
                               latenciesNs);

    if (!config.tradeFile.empty())
        ResultWriter::writeTrades(trades, config.tradeFile);
}

void Application::prepareOrders() {
    if (!config.orderFile.empty()) {
        std::println("Loading orders from file: {}", config.orderFile);
        orders = LobsterOrderLoader::load(config.orderFile);
    } else {
        std::println("Generating {} orders (seed: {})", config.genCount, config.genSeed);
        orders = OrderGenerator::generate(config.genSeed,
                                          config.genCount,
                                          config.genCancelPercent,
                                          config.genBasePrice,
                                          config.genMeanOffset,
                                          config.genStdDev);
    }

    if (orders.empty()) {
        std::println(stderr, "Couldn't load or generate any data");
        exit(1);
    }
}

double Application::matchOrders() {
    auto matchingStart = std::chrono::steady_clock::now();

    if (config.measureLatency) {
        for (const auto& order : orders) {
            _mm_lfence();
            uint64_t tickStart = __rdtsc();
            _mm_lfence();

            matchingEngine.processOrder(order, trades);

            unsigned int aux;
            uint64_t tickEnd = __rdtscp(&aux);
            _mm_lfence();

            latencies.push_back(tickEnd - tickStart);
        }
    } else {
        for (const auto& order : orders)
            matchingEngine.processOrder(order, trades);
    }

    auto matchingEnd = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(matchingEnd - matchingStart).count();
}

double Application::getCyclesPerNs() {
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
