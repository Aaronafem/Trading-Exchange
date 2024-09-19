#ifndef ORDER_HPP
#define ORDER_HPP

#include <ctime>
#include "enums.hpp"  // Include enums.hpp where BookSide is defined

class Order {
    int quantity;
    double price;
    time_t timestamp;
    BookSide side;

public:
    // Constructor
    Order(int quantity_, double price_, BookSide side_);

    // Setters and getters
    void set_quantity(int new_qty);
    int get_quantity() const;

    double get_price() const;
    time_t get_timestamp() const;
};

#endif // ORDER_HPP
