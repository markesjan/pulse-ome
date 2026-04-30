#pragma once

#include "Types.hpp"

namespace engine {

struct OrderLocation {
    Side side;
    Price price;
    std::size_t vecIdx;
};

} // namespace engine
