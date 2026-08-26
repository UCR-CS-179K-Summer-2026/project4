#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

struct OrderSummary {
    std::string customerName;
    std::vector<int> itemPrices;
};

// Returns the total value of one order.
int calculateTotal(const OrderSummary& order) {
    return std::accumulate(order.itemPrices.begin(), order.itemPrices.end(), 0);
}

// Returns a sorted copy so callers retain their original item order.
std::vector<int> sortedPrices(const OrderSummary& order) {
    std::vector<int> prices = order.itemPrices;
    std::sort(prices.begin(), prices.end());
    return prices;
}

// Formats one summary for the clean-control report.
std::string formatSummary(const OrderSummary& order, int total) {
    return order.customerName + " order total: " + std::to_string(total);
}

// Accepts only orders with a name and at least one non-negative price.
bool isValidOrder(const OrderSummary& order) {
    if (order.customerName.empty()) {
        return false;
    }
    return std::all_of(order.itemPrices.begin(), order.itemPrices.end(),
        [](int price) { return price >= 0; });
}

// Runs the clean-control scenario with a small realistic order workflow.
int main() {
    OrderSummary order{"Avery", {12, 8, 15, 6}};
    if (isValidOrder(order)) {
        const std::vector<int> prices = sortedPrices(order);
        const int total = calculateTotal(order);
        std::cout << formatSummary(order, total) << std::endl;
        std::cout << "Lowest item: " << prices.front() << std::endl;
    }
    return 0;
}
