#include "helpers.hpp"
#include "orderbook.hpp"
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <regex>
#include <chrono>

// Global variables
std::atomic<bool> bot_running(true);  // Control bot behavior
std::mutex book_mutex;  // Mutex for thread-safe operations

void execute_matching_trades(Orderbook& ob) {
    while (ob.get_highest_bid() >= ob.get_lowest_ask()) {
        double ask_price = ob.get_lowest_ask();
        int bid_quantity = ob.get_highest_bid_quantity();
        int ask_quantity = ob.get_lowest_ask_quantity();
        int fill_quantity = std::min(bid_quantity, ask_quantity);  // Fill as much as possible

        std::cout << "\033[33mExecuting trade: " << fill_quantity << " units at $" << ask_price << "\033[0m\n";

        // Start measuring time before executing the trade
        auto start_time = std::chrono::high_resolution_clock::now();

        // Remove filled quantities from bids and asks
        ob.remove_filled_orders(fill_quantity, ask_price, bid);  // Remove from bids
        ob.remove_filled_orders(fill_quantity, ask_price, ask);  // Remove from asks

        // Measure the time after the trade
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();

        // Print the transaction time
        std::cout << "\033[32mTransaction time: " << duration << " nanoseconds\033[0m\n";

        // After execution, print the orderbook
        ob.print();
    }
}




void user_input(Orderbook& ob) {
    while (true) {
        std::string input;
        std::cout << "Options\nPress 'i' to insert a trade, 'p' to print the order book, 'f' to freeze bot trades, 'r' to restart bot trades, 'q' to quit: ";
        std::cin >> input;

        if (input == "p") {
            std::lock_guard<std::mutex> lock(book_mutex);
            ob.print();  // Print the current orderbook
        } else if (input == "f") {
            bot_running = false;
            std::cout << "\033[33mBot trades frozen.\033[0m\n";
        } else if (input == "r") {
            bot_running = true;
            std::cout << "\033[32mBot trades restarted.\033[0m\n";
        } else if (input == "q") {
            std::cout << "\033[31mQuitting program.\033[0m\n";
            std::exit(0);  // Exit the program
        } else if (input == "i") {
            std::cout << "Enter order in format '10b 101' for 10 buy at 101, or '5s 105' for 5 sell at 105: ";
            std::cin.ignore();  
            std::getline(std::cin, input);  

            std::regex order_format(R"((\d+)([bs])\s*(\d+))");
            std::smatch matches;

            if (std::regex_match(input, matches, order_format)) {
                int quantity = std::stoi(matches[1]);
                char side = matches[2].str()[0];
                int price = std::stoi(matches[3]);

                // Check that the quantity is positive and the price is within the valid range
                if (quantity <= 0) {
                    std::cout << "\033[31mInvalid order: Quantity must be positive.\033[0m\n";
                } else if (price < 90 || price > 110) {
                    std::cout << "\033[31mInvalid order: Price must be between 90 and 110.\033[0m\n";
                } else {
                    BookSide bookSide = (side == 'b') ? bid : ask;

                    {
                        std::lock_guard<std::mutex> lock(book_mutex);
                        ob.add_order(quantity, price, bookSide);
                    }

                    std::cout << "\033[34mOrder added: " << quantity << ((side == 'b') ? " buy " : " sell ") 
                              << "at $" << price << "\033[0m\n";

                    execute_matching_trades(ob);  // Execute matching trades
                    ob.print();  // Print updated orderbook after user trade
                }
            } else {
                std::cout << "\033[31mInvalid input format. Please try again.\033[0m\n";
            }
        }
    }
}


// Bot behavior
void bot_behavior(Orderbook& ob) {
    bool is_buy = true;  // Alternates between buy and sell

    while (true) {
        if (bot_running) {
            std::this_thread::sleep_for(std::chrono::seconds(8));  // Adjust bot speed if needed

            int quantity = get_random_int(1, 2);  // Bot trades in quantities of 1 or 2
            int price;

            {
                std::lock_guard<std::mutex> lock(book_mutex);

                if (is_buy) {
                    price = get_random_int(90, 105);  // Buy within range
                    std::cout << "\n\033[34mBot is submitting a limit buy order for " << quantity 
                              << " units @ $" << price << "\033[0m\n";
                    ob.add_order(quantity, price, BookSide::bid);
                } else {
                    price = get_random_int(95, 110);  // Sell within range
                    std::cout << "\n\033[31mBot is submitting a limit sell order for " << quantity 
                              << " units @ $" << price << "\033[0m\n";
                    ob.add_order(quantity, price, BookSide::ask);
                }

                execute_matching_trades(ob);  // Execute matching trades
                ob.print();  // Print updated orderbook after bot trades
            }

            is_buy = !is_buy;  // Alternate buy/sell after every iteration
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Short pause when bot is frozen
        }
    }
}


int main() {
    Orderbook ob;

    // Initialize the order book with random bids and asks
    for (int i = 0; i < 10; ++i) {
        int buy_price = get_random_int(90, 105);
        int buy_qty = get_random_int(1, 6);
        ob.add_order(buy_qty, buy_price, BookSide::bid);

        int sell_price = get_random_int(95, 110);
        int sell_qty = get_random_int(1, 6);
        ob.add_order(sell_qty, sell_price, BookSide::ask);
    }

    std::cout << "Starting bot and live order book display...\n";

    // Start the bot in a separate thread
    std::thread bot_thread(bot_behavior, std::ref(ob));
    std::thread user_thread(user_input, std::ref(ob));

    bot_thread.join();
    user_thread.join();

    return 0;
}
