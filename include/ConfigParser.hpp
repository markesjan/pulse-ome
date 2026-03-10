#pragma once

#include "Config.hpp"

namespace engine {

class ConfigParser {
public:
    static Config parseArguments(int argc, char* argv[]);
};

} // namespace engine
