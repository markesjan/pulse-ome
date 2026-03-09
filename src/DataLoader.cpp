#include "DataLoader.hpp"

#include <fstream>
#include <iostream>

namespace engine {

std::vector<Order> DataLoader::loadOrders(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Couldn't open file: " << filePath << std::endl;
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

        double timestamp = std::strtod(p, &end);
        if (p == end)
            continue;
        p = end + 1;

        auto eventType = static_cast<uint8_t>(std::strtol(p, &end, 10));
        p = end + 1;

        if (eventType >= 1 && eventType <= 3) {
            auto orderId = static_cast<uint64_t>(std::strtoull(p, &end, 10));
            p = end + 1;
            auto quantity = static_cast<uint32_t>(std::strtoul(p, &end, 10));
            p = end + 1;
            auto price = static_cast<uint64_t>(std::strtoull(p, &end, 10));
            p = end + 1;
            auto side = static_cast<int8_t>(std::strtol(p, &end, 10));

            orders.emplace_back(Order{
                timestamp,
                orderId,
                price,
                quantity,
                eventType,
                side
            });
        }
    }

    orders.shrink_to_fit();

    std::cout << "Loaded " << orders.size() << " relevant order events." << std::endl;
    return orders;
}

} // namespace engine
