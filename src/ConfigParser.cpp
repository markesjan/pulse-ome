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
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: ./matching_engine [--input/-i <path>] [--output/-o <path>]" << std::endl;
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
