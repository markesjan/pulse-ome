/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  main.cpp
 * @brief Entry point of the matching engine application.
 */

#include "Application.hpp"
#include "ConfigParser.hpp"

int main(int argc, char* argv[]) {
    engine::Config config = engine::ConfigParser::parseArguments(argc, argv);

    engine::Application app(config);
    app.run();

    return 0;
}
