/**
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 Jan Markes
 *
 * @file  PriceLevel.hpp
 * @brief Definition of the PriceLevel structure in the order book.
 */

#pragma once

#include "Types.hpp"

namespace engine {

struct PriceLevel {
    OrderIndex headIdx = INVALID_INDEX;
    OrderIndex tailIdx = INVALID_INDEX;
};

} // namespace engine
