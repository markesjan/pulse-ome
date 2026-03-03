#pragma once

#include <string>

namespace engine {

struct Config {
    std::string inFile = "../data/AAPL_orders_lvl1.csv";
    std::string outFile = "../output/trades.csv";
};

class ConfigParser {
public:
    static Config parseArguments(int argc, char* argv[]);
};

} // namespace engine
