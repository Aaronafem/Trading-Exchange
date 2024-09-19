#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <map>
#include <vector>
#include <memory>
#include "order.hpp"
#include <iostream>



class Orderbook {
    std::map<double, std::vector<std::unique_ptr<Order>>> bids;  // Bid orders
    std::map<double, std::vector<std::unique_ptr<Order>>> asks;  // Ask orders

public:
    Orderbook();  // Constructor

    void add_order(int quantity, double price, BookSide side);  // Add order to the book
    std::tuple<int, double> execute_order(OrderType type, int quantity, Side side);  // Execute order
    void print() const;  // Print the orderbook

    int get_highest_bid_quantity();  // Get quantity of highest bid
    int get_lowest_ask_quantity();  // Get quantity of lowest ask
    void remove_filled_orders(int filled_quantity, double price, BookSide side);


    // Get the highest bid and lowest ask prices
    double get_highest_bid() const;
    double get_lowest_ask() const;
};

#endif // ORDERBOOK_HPP
