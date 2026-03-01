#include "ResultWriter.hpp"

#include <fstream>
#include <iostream>

namespace engine {

void ResultWriter::writeTrades(const std::vector<Trade>& trades, const std::string& filePath) {
    std::ofstream outFile(filePath);

    if (!outFile.is_open()) {
        std::cerr << "Couldn't open output file: " << filePath << std::endl;
        return;
    }

    outFile << "Symbol,BuyOrderID,SellOrderID,Quantity,Price\n";

    for (const auto& trade : trades) {
        outFile << trade.symbol << ","
            << trade.buyOrderId << ","
            << trade.sellOrderId << ","
            << trade.quantity << ","
            << trade.price << "\n";
    }
}

} // namespace engine
