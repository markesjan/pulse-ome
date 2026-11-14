/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  Commands.hpp
 * @brief Definitions of feed control (gui -> feed) command structures.
 */

#pragma once

#include <type_traits>

#include "Types.hpp"

namespace pulse::proto {

struct alignas(COMMAND_SIZE) FeedCommand {
    FeedAction action;
    FeedSource source{FeedSource::Invalid};
    double speedMultiplier{0.0};
};

static_assert(sizeof(FeedCommand) == COMMAND_SIZE);
static_assert(std::is_trivially_copyable_v<FeedCommand>);
static_assert(std::is_standard_layout_v<FeedCommand>);

} // namespace pulse::proto
