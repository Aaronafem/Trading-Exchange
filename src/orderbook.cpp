
#include "orderbook.hpp"  
#include <thread>
#include <atomic>
#include <regex>
#include "helpers.hpp"
#include <iostream>  // Required for std::cout





// Constructor
Orderbook::Orderbook() {
    // Initialize any data members if necessary
}

void Orderbook::add_order(int quantity, double price, BookSide side) {
    if (quantity <= 0) return;  // Ensure no invalid order quantities

    if (side == bid) {
        bids[price].push_back(std::make_unique<Order>(quantity, price, BookSide::bid));
    } else if (side == ask) {
        asks[price].push_back(std::make_unique<Order>(quantity, price, BookSide::ask));
    }
}


// Execute an order from the orderbook (placeholder logic)
std::tuple<int, double> Orderbook::execute_order(OrderType type, int quantity, Side side) {
    // You should add the logic to match bids and asks
    return std::make_tuple(0, 0.0);  // Replace this with actual execution logic
}

void Orderbook::print() const {
    std::cout << "========== Orderbook =========" << std::endl;

    // Print asks from highest to lowest
    std::cout << "Asks:" << std::endl;
    for (auto it = asks.rbegin(); it != asks.rend(); ++it) {
        int total_quantity = 0;
        for (const auto& order : it->second) {
            total_quantity += order->get_quantity();
        }
        std::cout << "$" << it->first << " - " << total_quantity << std::endl;
    }

    // Print bids from highest to lowest
    std::cout << "Bids:" << std::endl;
    for (auto it = bids.rbegin(); it != bids.rend(); ++it) {
        int total_quantity = 0;
        for (const auto& order : it->second) {
            total_quantity += order->get_quantity();
        }
        std::cout << "$" << it->first << " - " << total_quantity << std::endl;
    }
    std::cout << "==============================" << std::endl;
}




// Get the highest bid price
double Orderbook::get_highest_bid() const {
    if (!bids.empty()) {
        return bids.rbegin()->first;
    }
    return 0.0;  // No bids in the book
}

// Get the lowest ask price
double Orderbook::get_lowest_ask() const {
    if (!asks.empty()) {
        return asks.begin()->first;
    }
    return 0.0;  // No asks in the book
}

// Get the quantity of the highest bid
int Orderbook::get_highest_bid_quantity() {
    if (!bids.empty()) {
        return bids.rbegin()->second.front()->get_quantity();
    }
    return 0;  // No bids in the book
}

// Get the quantity of the lowest ask
int Orderbook::get_lowest_ask_quantity() {
    if (!asks.empty()) {
        return asks.begin()->second.front()->get_quantity();
    }
    return 0;  // No asks in the book
}


void Orderbook::remove_filled_orders(int filled_quantity, double price, BookSide side) {
    if (side == bid) {
        // Remove from the bids
        auto bid_it = bids.rbegin();  // Start from the highest bid
        while (filled_quantity > 0 && bid_it != bids.rend()) {
            int bid_qty = bid_it->second.front()->get_quantity();

            if (bid_qty <= filled_quantity) {
                // Fully fill the bid order
                filled_quantity -= bid_qty;
                bids[bid_it->first].erase(bids[bid_it->first].begin());
            } else {
                // Partially fill the bid order
                bids[bid_it->first].front()->set_quantity(bid_qty - filled_quantity);
                filled_quantity = 0;
            }

            // Remove empty price level
            if (bids[bid_it->first].empty()) {
                bids.erase(std::next(bid_it).base());
            }

            bid_it = bids.rbegin();  // Update the iterator
        }
    } else if (side == ask) {
        // Remove from the asks
        auto ask_it = asks.find(price);
        if (ask_it != asks.end()) {
            auto& ask_orders = ask_it->second;
            while (filled_quantity > 0 && !ask_orders.empty()) {
                int ask_qty = ask_orders.front()->get_quantity();

                if (ask_qty <= filled_quantity) {
                    // Fully fill the ask order
                    filled_quantity -= ask_qty;
                    ask_orders.erase(ask_orders.begin());
                } else {
                    // Partially fill the ask order
                    ask_orders.front()->set_quantity(ask_qty - filled_quantity);
                    filled_quantity = 0;
                }
            }

            // Remove empty price level
            if (ask_orders.empty()) {
                asks.erase(ask_it);
            }
        }
    }
}
