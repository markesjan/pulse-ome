#include "ConfigParser.hpp"

#include <iostream>

namespace engine {

Config ConfigParser::parseArguments(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if ((arg == "--input" || arg == "-i") && i + 1 < argc) {
            config.inFile = argv[++i];
        } else if ((arg == "--output" || arg == "-o") && i + 1 < argc) {
            config.outFile = argv[++i];
        } else if ((arg == "--generate" || arg == "-g") && i + 1 < argc) {
            config.generateCnt = std::stoull(argv[++i]);
        } else if ((arg == "--seed" || arg == "-s") && i + 1 < argc) {
            config.seed = static_cast<uint32_t>(std::stoul(argv[++i]));
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: ./matching_engine [options]\n\n"
                << "Options:\n"
                << "  -i, --input <path>      Path to input CSV file with orders\n"
                << "  -o, --output <path>     Path to output CSV file for trades\n"
                << "  -g, --generate <count>  Generate specified number of random orders\n"
                << "  -s, --seed <seed>       Seed for random order generation\n"
                << "  -h, --help              Show this help message\n"
                << std::endl;
            exit(0);
        } else {
            std::cerr << "Invalid argument: " << arg << std::endl;
            std::cout << "Use --help/-h for usage" << std::endl;
            exit(1);
        }
    }

    return config;
}

} // namespace engine
