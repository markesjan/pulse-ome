#include "OrderLoader.hpp"

#include <cstring>
#include <fstream>
#include <print>
#include <unordered_map>

namespace engine {

std::vector<Order> OrderLoader::load(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::println(stderr, "Couldn't open file: {}", filePath);
        exit(1);
    }

    std::vector<Order> orders;
    orders.reserve(1'000'000);

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        const char* p = line.c_str();
        char* end;

        const char* comma = std::strchr(p, ',');
        if (!comma)
            continue;

        double timestamp = std::strtod(p, &end);
        if (p == end)
            continue;
        p = end + 1;

        auto rawEventType = static_cast<uint8_t>(std::strtol(p, &end, 10));
        p = end + 1;

        if (rawEventType >= 1 && rawEventType <= 3) {
            auto orderId = static_cast<OrderId>(std::strtoull(p, &end, 10));
            p = end + 1;
            auto quantity = static_cast<Quantity>(std::strtoul(p, &end, 10));
            p = end + 1;
            auto price = static_cast<Price>(std::strtoull(p, &end, 10));
            p = end + 1;
            auto rawSide = static_cast<int8_t>(std::strtol(p, &end, 10));

            orders.emplace_back(Order{
                timestamp,
                orderId,
                price,
                quantity,
                static_cast<EventType>(rawEventType),
                static_cast<Side>(rawSide),
            });
        }
    }

    orders.shrink_to_fit();

    std::println("Loaded {} relevant order events.", orders.size());
    return orders;
}

} // namespace engine
