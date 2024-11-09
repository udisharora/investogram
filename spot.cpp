#ifndef SPOT_CPP
#define SPOT_CPP
#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include "manage_user.cpp"
using namespace std;

//create a function to write the spot prices of each stock at each timestamp in a file
unordered_map<long long,vector<double> > getspotprice;

// write all the spot prices to a file
void writeSpotPricesToFile(){
    // Open a file in append mode
    std::ofstream file("spot_prices.dat", std::ios::app);
    
    // Write the day number to the file
    file << "Spot Prices" << "\n";
    
    // Write all spot prices to the file
    for(auto it : getspotprice){
        file<<it.first<<" ";
        for(auto it1 : it.second){
            file<<it1<<" ";
        }
        file<<"\n";
    }
    
    // Close the file
    file.close();
}

// Function to log the price of a stock to a file
void logPriceToFile(const std::string &company, long long timestamp, double price, int buyer_id, int seller_id) {
    // Open a file in append mode
    std::ofstream file("data/stock_prices.dat", std::ios::app);
    
    // Write the timestamp, company name, and price to the file
    file << timestamp << " " << company << " " << price << " " << buyer_id << " " << seller_id << "\n";
    
    // Close the file
    file.close();
}

//hash map for last trade price for each stock
unordered_map<string, double> last_trade_prices;


struct Order {
    std::string company_name;
    int user_id;
    bool is_buy; // true for buy, false for sell
    double price;
    long long timestamp;
    double quantity;

    Order(std::string comp, int id, bool buy, double p, long long ts, double q = 1.0)
        : company_name(comp), user_id(id), is_buy(buy), price(p), timestamp(ts), quantity(q) {}
};



// Comparator for max-heap (buy orders)
struct BuyComparator {
    bool operator()(const Order &a, const Order &b) {
        return a.price < b.price || (a.price == b.price && a.timestamp > b.timestamp);
    }
};

// Comparator for min-heap (sell orders)
struct SellComparator {
    bool operator()(const Order &a, const Order &b) {
        return a.price > b.price || (a.price == b.price && a.timestamp > b.timestamp);
    }
};

// Priority queues for buy and sell orders
std::priority_queue<Order, std::vector<Order>, BuyComparator> buyOrders;
std::priority_queue<Order, std::vector<Order>, SellComparator> sellOrders;

// Trading duration in seconds (9:15 AM to 3:30 PM = 6 hours 15 minutes = 22,500 seconds = 22,500,000 milliseconds)
const long long TRADING_DAY_DURATION = 22500000;

// Function to handle a trade between buyer and seller
void executeTrade(int buyer_id, int seller_id, double price, double quantity,string company) {
    // Transaction processing logic
    doTransaction(buyer_id,seller_id,price,quantity,company);
}

// Function to write all cacelled orders to a file
void writeCancelledOrdersToFile(int day) {
    // Open a file in append mode
    std::ofstream file("data/cancelled_orders.dat", std::ios::app);
    
    // Write the day number to the file
    file << "Day " << day << "\n";
    
    // Write all cancelled orders to the file
    while (!buyOrders.empty()) {
        Order order = buyOrders.top();
        buyOrders.pop();
        file << order.timestamp << " " << order.company_name << " " << order.price << " " << order.user_id << " " << order.quantity << " " << "Buy" << "\n";
    }
    
    while (!sellOrders.empty()) {
        Order order = sellOrders.top();
        sellOrders.pop();
        file << order.timestamp << " " << order.company_name << " " << order.price << " " << order.user_id << " " << order.quantity << " " << "Sell" << "\n";
    }
    
    // Close the file
    file.close();
}


//global variable for last trade time
long long last_trade_price = 0;

void spotTrade(const std::string &company, int user_id, bool is_buy, double price, long long timestamp, double quantity) {
    // Create a new order
    Order order(company, user_id, is_buy, price, timestamp, quantity);

    // Match buy and sell orders
    bool tradeExecuted = false;
    if (is_buy) {
        // Check if there’s a sell order that can match
        while (!sellOrders.empty() && sellOrders.top().price <= price) {
            Order sellOrder = sellOrders.top();
            sellOrders.pop();
            if (sellOrder.company_name != company) continue;

            double tradeQuantity = std::min(order.quantity, sellOrder.quantity);
            executeTrade(user_id, sellOrder.user_id, sellOrder.price, tradeQuantity, company);
            last_trade_prices[company] = sellOrder.price;
            logPriceToFile(company, timestamp, sellOrder.price, user_id, sellOrder.user_id);

            order.quantity -= tradeQuantity;
            sellOrder.quantity -= tradeQuantity;

            if (sellOrder.quantity > 0) sellOrders.push(sellOrder);
            if (order.quantity == 0) {
                tradeExecuted = true;
                break;
            }
        }
        if (order.quantity > 0) buyOrders.push(order);

    } else {
        // Check if there’s a buy order that can match
        while (!buyOrders.empty() && buyOrders.top().price >= price) {
            Order buyOrder = buyOrders.top();
            buyOrders.pop();
            if (buyOrder.company_name != company) continue;

            double tradeQuantity = std::min(order.quantity, buyOrder.quantity);
            executeTrade(buyOrder.user_id, user_id, buyOrder.price, tradeQuantity, company);
            last_trade_prices[company] = buyOrder.price;
            logPriceToFile(company, timestamp, buyOrder.price, buyOrder.user_id, user_id);

            order.quantity -= tradeQuantity;
            buyOrder.quantity -= tradeQuantity;

            if (buyOrder.quantity > 0) buyOrders.push(buyOrder);
            if (order.quantity == 0) {
                tradeExecuted = true;
                break;
            }
        }
        if (order.quantity > 0) sellOrders.push(order);
    }

    // Add all spot prices to getspotprice for the current timestamp
    if (tradeExecuted) {
        getspotprice[timestamp].clear();
        for (auto it : last_trade_prices) {
            getspotprice[timestamp].push_back(it.second);
        }
    } else {
        // If no trade was executed, you could still add the last known prices for each company
        if (!getspotprice.count(timestamp)) {
            for (auto it : last_trade_prices) {
                getspotprice[timestamp].push_back(it.second);
            }
        }
    }
}
#endif