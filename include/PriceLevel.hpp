#pragma once

#include "Types.hpp"

namespace engine {

struct PriceLevel {
    OrderIndex headIdx = INVALID_INDEX;
    OrderIndex tailIdx = INVALID_INDEX;
};

} // namespace engine
