# PulseOME

[![CI](https://github.com/markesjan/pulse-ome/actions/workflows/ci.yml/badge.svg)](https://github.com/markesjan/pulse-ome/actions/workflows/ci.yml)
[![C++23](https://img.shields.io/badge/standard-C%2B%2B23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)

**PulseOME** is a modular, high-performance financial exchange ecosystem designed for deterministic low-latency order processing and real-time market visualization.
Developed as a bachelor's [thesis](ctufit-thesis.pdf) at **CTU FIT**, the project bridges pure data-oriented design on the critical path in matching core with object-oriented design in supporting subsystems.

---

## Architectural Decomposition

The ecosystem is partitioned into **4 decoupled components** interacting via lock-free ring buffers:

### Monorepo Structure

- [`apps/ome/`](apps/ome/): **Core Order Matching Engine (`pulse_ome`)**  
  Low-latency matching daemon. Utilizes a strict data-oriented design, hierarchical bitsets for price discovery, flat order book, and zero dynamic allocations on the critical hot path. Detailed benchmarks and theoretical background: see [`apps/ome/README.md`](apps/ome/README.md).
- TODO: [`apps/feed/`](apps/feed/): **Market Data Ingestion Service (`pulse_feed`)**  
  Feed controller managing market data streams (live exchange feeds, LOBSTER historical replay, synthetic stress generator).
- TODO: [`apps/gui/`](apps/gui/): **Real-Time GUI Dashboard (`pulse_gui`)**  
  GUI dashboard providing configuration of the ecosystem and live order book visualizations.
- TODO: [`libs/bus/`](libs/bus/): **Lock-Free Transport Library (`pulse::bus`)**  
  High-throughput SPSC ring buffer library supporting POSIX Shared Memory.
- [`tests/`](tests/): GoogleTest unit and integration suites validating system correctness.
- [`benchmarks/`](benchmarks/): Micro-benchmarks and latency profiling using Google Benchmark.

---

## System Requirements

- **Operating System:** Linux based
- **Compiler:** With C++23 support (GCC15+ / Clang18+)
- **Build System:** CMake 3.28+ and Ninja
- **Code Formatter:** `clang-format`

---

## Building and Testing

### 1. Build the Entire Monorepo
```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### 2. Run Test Suites
```bash
ctest --test-dir build --output-on-failure
```

---

## License

This project is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.
