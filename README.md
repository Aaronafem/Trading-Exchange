Compile and run this through a terminal environment: g++ -std=c++17 -I../include main.cpp order.cpp orderbook.cpp helpers.cpp -o main && ./main

The concept of electronic trading and the evolution of the order book, like the one simulated here, traces its origins back to the early 1980s. 
Prior to this era, stock trading was primarily done through face-to-face interaction on the trading floors of stock exchanges known as "trading pits", where traders would shout 
out bids and offers in a chaotic environment often referred to as the "open outcry" system. This system, while functional for decades, was prone to 
inefficiencies, lack of transparency, and human error.

The pivotal shift towards electronic trading began with the introduction of NASDAQ (National Association of Securities Dealers Automated Quotations) in 1971, 
but it wasn't until the 1980s that it gained significant traction. NASDAQ was the first electronic stock exchange that allowed brokers to trade stocks through 
a computerized system, eliminating the need for physical presence on a trading floor. This marked the birth of modern, fully automated electronic trading systems, 
laying the foundation for the evolution of the central limit order book.

In this early electronic trading environment, an order book was born as a key component. The order book is essentially a digital ledger where buy (bid) and sell 
(ask) orders are listed. It ensures that orders are matched automatically based on price and availability, creating a transparent and efficient market for stocks 
and other securities.

This project simulates the core functionality of an order book, which was a groundbreaking development in the financial markets. By implementing the CLOB in C++, 
a language known for its high performance and system-level capabilities, this project mirrors the high-speed, high-efficiency nature of real-world electronic trading platforms. 
The program enables the continuous matching of buy and sell orders and dynamically updates the order book with each trade.

The importance of this historical artifact cannot be overstated. The shift to electronic trading systems, as pioneered by NASDAQ, revolutionized global financial markets. 
It led to greater market efficiency, liquidity, and accessibility, making it possible for millions of trades to be processed in fractions of a second. This C++ order book implementation 
pays homage to that significant moment in computing history by recreating, on a smaller scale, the very mechanism that powers today's stock markets.
