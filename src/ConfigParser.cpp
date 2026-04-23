#include "ConfigParser.hpp"

#include <print>

namespace engine {

Config ConfigParser::parseArguments(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "--input" || arg == "-i") && i + 1 < argc) {
            config.orderFile = argv[++i];
        } else if ((arg == "--output" || arg == "-o") && i + 1 < argc) {
            config.tradeFile = argv[++i];
        } else if ((arg == "--generate" || arg == "-g") && i + 1 < argc) {
            config.generateCnt = std::stoull(argv[++i]);
        } else if ((arg == "--seed" || arg == "-s") && i + 1 < argc) {
            config.seed = static_cast<uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--latency" || arg == "-l") {
            config.measureLatency = true;
        } else if (arg == "--help" || arg == "-h") {
            std::print("Usage: ./matching_engine [options]\n\n"
                "Options:\n"
                "  -i, --input <path>      Path to input CSV file with orders\n"
                "  -o, --output <path>     Path to output CSV file for trades\n"
                "  -g, --generate <count>  Generate specified number of random orders\n"
                "  -s, --seed <seed>       Seed for random order generation\n"
                "  -l, --latency           Enable latency measurement (slows throughput)\n"
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
