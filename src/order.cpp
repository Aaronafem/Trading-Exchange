#include "order.hpp"



// Constructor definition
Order::Order(int quantity_, double price_, BookSide side_)
    : quantity(quantity_), price(price_), side(side_) {
    timestamp = std::time(nullptr);  // Set the timestamp to the current time
}

// Set the quantity of the order
void Order::set_quantity(int new_qty) {
    quantity = new_qty;
}

// Get the quantity of the order
int Order::get_quantity() const {
    return quantity;
}

// Get the price of the order
double Order::get_price() const {
    return price;
}

// Get the timestamp of the order
time_t Order::get_timestamp() const {
    return timestamp;
}
