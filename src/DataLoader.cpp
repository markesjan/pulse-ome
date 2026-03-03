#include "DataLoader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace engine {

std::vector<Order> DataLoader::loadOrders(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Couldn't open file: " << filePath << std::endl;
        exit(1);
    }

    std::vector<Order> orders;
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> cols;

        while (std::getline(ss, token, ',')) {
            cols.push_back(token);
        }

        if (cols.size() == 6) {
            uint8_t eventType = static_cast<uint8_t>(std::stoi(cols[1]));

            if (eventType >= 1 && eventType <= 3) {
                Order order;

                order.timestamp = std::stod(cols[0]);
                order.eventType = eventType;
                order.orderId = static_cast<uint64_t>(std::stoi(cols[2]));
                order.quantity = static_cast<uint32_t>(std::stoi(cols[3]));
                order.price = static_cast<uint64_t>(std::stoi(cols[4]));
                order.side = static_cast<int8_t>(std::stoi(cols[5]));
                order.symbol = "AAPL";

                orders.push_back(order);
            }
        }
    }

    std::cout << "Loaded " << orders.size() << " relevant order events." << std::endl;
    return orders;
}

} // namespace engine
