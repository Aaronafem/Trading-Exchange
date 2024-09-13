#include <iostream>
#include <map>
#include <vector>
#include <memory>
#include <random>
#include <ctime>
#include <ncurses.h>

enum class OrderType {
    BUY,
    SELL
};

// Custom make_unique for C++11 compatibility
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

struct Order {
    OrderType type;
    double price;
    int quantity;

    Order(OrderType type_, double price_, int quantity_)
        : type(type_), price(price_), quantity(quantity_) {}
};

class OrderBook {
public:
    std::map<double, std::vector<std::unique_ptr<Order>>> bids;  // Buy Orders
    std::map<double, std::vector<std::unique_ptr<Order>>> asks;  // Sell Orders

    // Add an order to the order book
    void add_order(OrderType type, double price, int quantity) {
        auto order = make_unique<Order>(type, price, quantity);

        if (type == OrderType::BUY) {
            bids[price].push_back(std::move(order));
        } else {
            asks[price].push_back(std::move(order));
        }
    }

    // Match buy and sell orders and display the fill price (always the ask price)
    void match_orders() {
        while (!bids.empty() && !asks.empty()) {
            auto highest_bid = --bids.end();  // Get the highest bid
            auto lowest_ask = asks.begin();   // Get the lowest ask

            if (highest_bid->first >= lowest_ask->first) {
                int quantity = std::min(highest_bid->second[0]->quantity, lowest_ask->second[0]->quantity);
                double fill_price = lowest_ask->first;  // Fill price is always the ask price

                printw("Matched Order: Buy at Price %.2f with Sell at Price %.2f for quantity %d\n",
                       highest_bid->first, lowest_ask->first, quantity);
                printw("Fill Price: %.2f\n", fill_price);  // Display the fill price (ask price)

                // Adjust the quantities
                highest_bid->second[0]->quantity -= quantity;
                lowest_ask->second[0]->quantity -= quantity;

                // Remove the order if the quantity is zero
                if (highest_bid->second[0]->quantity == 0) {
                    highest_bid->second.erase(highest_bid->second.begin());
                    if (highest_bid->second.empty()) {
                        bids.erase(highest_bid);
                    }
                }

                if (lowest_ask->second[0]->quantity == 0) {
                    lowest_ask->second.erase(lowest_ask->second.begin());
                    if (lowest_ask->second.empty()) {
                        asks.erase(lowest_ask);
                    }
                }
            } else {
                break;  // No match is possible
            }
        }
    }

    // Display the order book using ncurses with buy orders on the left and sell orders on the right
    void display_order_book() {
        clear();
        //int max_bar_length = 30;  // Max length of the bar for a large quantity (adjusted for side-by-side display)
        
        printw("Order Book:\n");
        printw("%-25s %-10s %10s %10s\n", "Buy Orders", "Size", "Sell Orders", "Size");

        // Get the maximum number of rows to display based on the larger of the two sides
        int max_rows = std::max(bids.size(), asks.size());
        auto bid_it = bids.begin();
        auto ask_it = asks.begin();

        // Iterate through both buy and sell orders and display them side by side
        for (int i = 0; i < max_rows; ++i) {
            // Left column for Buy Orders
            if (bid_it != bids.end()) {
                double price = bid_it->first;
                auto& orders = bid_it->second;
                int total_quantity = 0;
                for (auto& order : orders) {
                    total_quantity += order->quantity;
                }
                printw("%-25.2f %-10d ", price, total_quantity);  // Print the buy price and total quantity
                ++bid_it;
            } else {
                printw("%-25s %-10s ", "", "");  // Empty space for Buy Orders if there are no more orders
            }

            // Right column for Sell Orders
            if (ask_it != asks.end()) {
                double price = ask_it->first;
                auto& orders = ask_it->second;
                int total_quantity = 0;
                for (auto& order : orders) {
                    total_quantity += order->quantity;
                }
                printw("%10.2f %10d\n", price, total_quantity);  // Print the sell price and total quantity
                ++ask_it;
            } else {
                printw("%10s %10s\n", "", "");  // Empty space for Sell Orders if there are no more orders
            }
        }
        refresh();  // Refresh to show the updated order book
    }
};

// Function to generate random orders for testing, now constrained to prices between 30 and 70
void generate_random_orders(OrderBook &orderBook, int num_orders) {
    std::default_random_engine generator(time(0));  // Random seed
    std::uniform_real_distribution<double> price_distribution(30.0, 70.0);  // Price between 30 and 70
    std::uniform_int_distribution<int> quantity_distribution(1, 10);  // Quantity between 1 and 10
    std::uniform_int_distribution<int> type_distribution(0, 1);  // 0 for Buy, 1 for Sell

    for (int i = 0; i < num_orders; ++i) {
        OrderType type = (type_distribution(generator) == 0) ? OrderType::BUY : OrderType::SELL;
        double price = price_distribution(generator);
        int quantity = quantity_distribution(generator);
        
        // Add the generated order to the order book
        orderBook.add_order(type, price, quantity);
    }
}

int main() {
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    
    OrderBook orderBook;
    
    // Generate random orders for testing, with prices between 30 and 70
    generate_random_orders(orderBook, 10);  // Generate 10 random orders

    int option;

    while (true) {
        clear();
        printw("1. Add Order\n2. Match Orders\n3. Display Order Book\n4. Exit\n");
        refresh();
        option = getch() - '0';  // Convert character input to number

        if (option == 1) {
            char type;
            double price = 0;
            int quantity = 0;

            // Input validation loop for order type
            do {
                printw("Enter order type (B for Buy, S for Sell, or E to exit): ");
                refresh();
                type = getch();
                type = toupper(type);  // Ensure type is uppercase
                if (type == 'E') {
                    break;  // Exit input process
                }
                if (type != 'B' && type != 'S') {
                    printw("\nInvalid order type. Please enter B, S, or E.\n");
                }
            } while (type != 'B' && type != 'S');  // Loop until valid input is received

            if (type == 'E') continue;  // Return to the main menu if user chooses to exit

            // Enable echo mode to allow user to see their input
            echo();

            // Input validation loop for price
            do {
                printw("\nEnter price (must be greater than 0, or type E to exit): ");
                char price_input[100];
                scanw(const_cast<char*>("%s"), price_input);  // Corrected with const_cast<char*>
                if (price_input[0] == 'e' || price_input[0] == 'E') {
                    break;  // Exit input process
                }
                price = atof(price_input);  // Convert string to double
                if (price <= 0) {
                    printw("Invalid price. Please enter a price greater than 0.\n");
                }
            } while (price <= 0);  // Loop until valid price is entered

            if (price <= 0) continue;  // Return to main menu if exited

            // Input validation loop for quantity
            do {
                printw("Enter quantity (must be at least 1, or type E to exit): ");
                char quantity_input[100];
                scanw(const_cast<char*>("%s"), quantity_input);  // Corrected with const_cast<char*>
                if (quantity_input[0] == 'e' || quantity_input[0] == 'E') {
                    break;  // Exit input process
                }
                quantity = atoi(quantity_input);  // Convert string to integer
                if (quantity < 1) {
                    printw("Invalid quantity. Please enter a quantity of at least 1.\n");
                }
            } while (quantity < 1);  // Loop until valid quantity is entered

            if (quantity < 1) continue;  // Return to main menu if exited

                       // Disable echo mode after input
            noecho();

            // Determine the order type based on the user's input
            OrderType orderType = (type == 'B') ? OrderType::BUY : OrderType::SELL;
            orderBook.add_order(orderType, price, quantity);  // Add the order to the order book

            printw("\nOrder added successfully!\n");
            refresh();
            getch();  // Wait for user input before returning to the menu
        } else if (option == 2) {
            orderBook.match_orders();  // Match the buy and sell orders
            printw("\nOrders matched successfully!\n");
            refresh();
            getch();  // Wait for user input before returning to the menu
        } else if (option == 3) {
            orderBook.display_order_book();  // Display the current order book with prices and quantities
            getch();  // Wait for user input before returning to the menu
        } else if (option == 4) {
            break;  // Exit the loop and end the program
        } else {
            printw("Invalid option. Please try again.\n");
            refresh();
            getch();  // Wait for user input before returning to the menu
        }
    }

    // End ncurses
    endwin();
    return 0;
}

