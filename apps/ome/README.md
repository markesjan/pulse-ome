# PulseOME: Order Matching Engine

A high-performance C++23 order matching engine designed for low-latency and deterministic execution. This component serves as the central matching core (`apps/ome/`) within the modular **PulseOME** ecosystem.

This engine was originally developed as a bachelor's thesis at FIT CTU. The thesis text (in Czech) is available at [`../../ctufit-thesis.pdf`](../../ctufit-thesis.pdf) and provides in-depth theoretical analysis, architectural design, and performance profiling.

The engine evolved through multiple optimization iterations, transforming from a standard object-oriented implementation into an optimized system capable of processing over 50 million ops/s on real-world market data (on the referenced test environment).

---

## Key Features

- **High Throughput:** Reaches over 51 million ops/s on LOBSTER datasets and up to 55 million ops/s in synthetic scenarios.
- **Predictable Latency:** Minimized jitter through OS-level optimizations, including memory locking and thread affinity.
- **Data-Oriented Architecture:** Optimized for CPU cache with contiguous memory layouts and minimal pointer chasing.
- **Hierarchical Price Discovery:** Uses a 4-level bitset for constant-time discovery of the best bid and ask prices.
- **Zero Runtime Allocation:** Pre-allocates memory pools for orders and price levels to eliminate heap fragmentation and allocation overhead.
- **Profile-Guided Optimization (PGO):** Built-in support for PGO to enhance performance based on execution profiles.

---

## Hardware Environment

The performance metrics and benchmarks presented below were achieved on the following hardware configuration, operating in a stabilized environment at base clock frequencies:

- **CPU**: AMD Ryzen 5 5625U (6 C / 12 T, Base: 2.27 GHz, Max: 4.30 GHz)
- **L1 Cache**: 32 KiB L1d / 32 KiB L1i (per core)
- **L2 Cache**: 512 KiB (per core)
- **L3 Cache**: 16 MiB (shared)
- **RAM**: 16 GB LPDDR4, 4266 MT/s
- **OS Tuning**: Memory locking, thread affinity, Core isolation

---

## Architecture and Design

The engine is designed to minimize the critical path of order processing. Key architectural components include:

- **Order Pool:** A pre-allocated pool of order objects. Orders are indexed directly, allowing $O(1)$ access for cancellations and modifications.
- **Multi-level Price Bitset:** A hierarchical bitmask structure that tracks active price levels. It allows the engine to skip empty price levels efficiently, ensuring $O(1)$ discovery of the next best price regardless of the spread.
- **Flat Order Book:** Instead of pointer-based trees or lists, the order book utilizes contiguous vectors to store price levels and order chains, significantly reducing cache misses.

---

## Performance Evolution

The engine's development followed an iterative process. Each phase introduced significant performance gains by addressing specific hardware bottlenecks.

### Iteration Summary (LOBSTER MSFT lvl10 Dataset)

| Iteration             | Description                                                | Throughput (M ops/s) | Speedup |
|:----------------------|:-----------------------------------------------------------|:--------------------:|:-------:|
| 1. Baseline           | Standard OOP using `std::map` and `std::list`.             |         2.51         |  1.0x   |
| 2. Indexing           | Introduction of direct order indexing for $O(1)$ lookups.  |         7.91         |  3.1x   |
| 3. Pool Allocation    | Elimination of runtime heap allocations via memory pools.  |         9.38         |  3.7x   |
| 4. Data-Oriented      | Transformation to flat structures and cache-aware layouts. |        33.32         |  13.2x  |
| 5. Final Optimization | Micro-optimizations, PGO, and OS-level isolation.          |        51.06         |  20.3x  |

### Synthetic Benchmark Results

The engine demonstrates high stability even under extreme workloads:

- **100% Order Insertion:** 55.11 M ops/s
- **50% Order Cancellation:** 52.52 M ops/s
- **High Matching Intensity (69% Match Rate):** 24.88 M ops/s

---

## Building and Execution

The engine target `pulse_ome` is configured from the monorepo root:

```bash
# Configure and build from repository root
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target pulse_ome
```

### Running the Binary

```bash
# Process historical LOBSTER dataset
./build/apps/ome/pulse_ome -i data/LOBSTER/MSFT/MSFT_2012-06-21_message_lvl10.csv -o trades.csv -l

# Run synthetic stress test scenario
./build/apps/ome/pulse_ome --generate 1000000 --cancel 10 --seed 42
```

### Profile-Guided Optimization (PGO)

1. **Instrumented Build:**
   ```bash
   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_PGO_GENERATE=ON
   cmake --build build --target pulse_ome
   ./build/apps/ome/pulse_ome -i data/LOBSTER/MSFT/MSFT_2012-06-21_message_lvl10.csv
   ```
2. **Optimized Build:**
   ```bash
   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DENABLE_PGO_GENERATE=OFF -DENABLE_PGO_USE=ON
   cmake --build build --target pulse_ome
   ```

### Benchmarks

Run micro-benchmarks configured in `benchmarks/`:
```bash
./build/benchmarks/engine_benchmarks
```
