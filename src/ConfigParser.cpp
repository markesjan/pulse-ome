#include "ConfigParser.hpp"

#include <print>

#include "Types.hpp"

namespace engine {

Config ConfigParser::parseArguments(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "--input" || arg == "-i") && i + 1 < argc) {
            config.orderFile = argv[++i];
        } else if ((arg == "--output" || arg == "-o") && i + 1 < argc) {
            config.tradeFile = argv[++i];
        } else if (arg == "--latency" || arg == "-l") {
            config.measureLatency = true;
        } else if ((arg == "--cpu" || arg == "-c") && i + 1 < argc) {
            config.cpuCore = std::stoi(argv[++i]);
        } else if (arg == "--lock-memory" || arg == "-m") {
            config.lockMemory = true;
        } else if ((arg == "--seed" || arg == "-s") && i + 1 < argc) {
            config.genSeed = static_cast<uint32_t>(std::stoul(argv[++i]));
        } else if ((arg == "--generate" || arg == "-g") && i + 1 < argc) {
            config.genCount = std::stoull(argv[++i]);
        } else if (arg == "--cancel" && i + 1 < argc) {
            config.genCancelPercent = static_cast<uint8_t>(std::stoi(argv[++i]));
        } else if (arg == "--price" && i + 1 < argc) {
            config.genBasePrice = std::stoull(argv[++i]);
        } else if (arg == "--offset" && i + 1 < argc) {
            config.genMeanOffset = std::stod(argv[++i]);
        } else if (arg == "--deviation" && i + 1 < argc) {
            config.genStdDev = std::stod(argv[++i]);
        } else if (arg == "--help" || arg == "-h") {
            std::print("Usage: ./matching_engine [options]\n\n"
                "Options:\n"
                "  -i, --input <path>      Path to input CSV file with orders\n"
                "  -o, --output <path>     Path to output CSV file for trades\n"
                "  -l, --latency           Enable latency measurement (slows throughput)\n"
                "  -c, --cpu <core>        Pin thread to specific CPU core\n"
                "  -m, --lock-memory       Lock memory into RAM\n"
                "  -s, --seed <seed>       Seed for random order generation\n"
                "  -g, --generate <count>  Generate specified number of random orders\n"
                "      --cancel <percent>  Cancellation percentage of all generated orders\n"
                "      --price <price>     Base price of generated orders\n"
                "      --offset <float>    Mean offset from base price for order generation\n"
                "      --deviation <float> Standard deviation for price generation\n"
                "  -h, --help              Show this help message\n");
            exit(0);
        } else {
            std::println(stderr, "Invalid argument: {}", arg);
            std::println("Use --help/-h for usage");
            exit(1);
        }
    }

    return config;
}

} // namespace engine
