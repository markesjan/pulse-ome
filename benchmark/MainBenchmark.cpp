#include <benchmark/benchmark.h>
#include <pthread.h>
#include <sys/mman.h>
#include <iostream>
#include <string>
#include <vector>
#include <print>

int main(int argc, char** argv) {
    benchmark::MaybeReenterWithoutASLR(argc, argv);

    bool lockMemory = false;
    int cpuCore = -1;
    std::vector<char*> newArgv;
    newArgv.push_back(argv[0]);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-m" || arg == "--lock-memory") {
            lockMemory = true;
        } else if ((arg == "-c" || arg == "--cpu") && i + 1 < argc) {
            try {
                cpuCore = std::stoi(argv[++i]);
            } catch (...) {
                std::cerr << "Invalid CPU core: " << argv[i] << std::endl;
            }
        } else {
            newArgv.push_back(argv[i]);
        }
    }

    if (lockMemory) {
        if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
            std::println(stderr, "Warning: Failed to lock memory");
        else
            std::println("Memory locked successfully");
    }

    if (cpuCore >= 0) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(cpuCore, &cpuset);

        if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0)
            std::println(stderr, "Warning: Failed to set thread affinity to core {}", cpuCore);
        else
            std::println("Thread successfully pinned to core {}", cpuCore);
    }

    int new_argc = static_cast<int>(newArgv.size());
    char** new_argv_ptr = newArgv.data();

    ::benchmark::Initialize(&new_argc, new_argv_ptr);
    if (::benchmark::ReportUnrecognizedArguments(new_argc, new_argv_ptr))
        return 1;
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}
