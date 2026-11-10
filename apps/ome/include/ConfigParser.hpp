/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  ConfigParser.hpp
 * @brief Definition of the ConfigParser class for command-line arguments.
 */

#pragma once

#include "Config.hpp"

namespace engine {

class ConfigParser {
public:
    static Config parseArguments(int argc, char* argv[]);
};

} // namespace engine
