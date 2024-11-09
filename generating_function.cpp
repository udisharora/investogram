#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <map>
#include <unordered_map>
#include "spot.cpp"
using namespace std;


// Function to generate a normally distributed random number using the Box-Muller transform
double generateNormalRandom() {
    static bool hasSpare = false;
    static double spare;
    if (hasSpare) {
        hasSpare = false;
        return spare;
    } else {
        double u = rand() / (RAND_MAX + 1.0);
        double v = rand() / (RAND_MAX + 1.0);
        double z0 = sqrt(-2.0 * log(u)) * cos(2.0 * M_PI * v);
        spare = sqrt(-2.0 * log(u)) * sin(2.0 * M_PI * v);
        hasSpare = true;
        return z0;
    }
}

unordered_map<string, double> target_prices = {
    {"AAPL", 500.0},
    {"GOOGL", 450.0},
    {"MSFT", 300.0},
    {"AMZN", 350.0},
    {"TSLA", 100.0}
};

// Function to generate stock-like orders with random walk with drift, occasional peaks and dips
void generateOrders(int numOrders, int day) {
    srand(time(0));
    
    // Initialize companies using push_back
    vector<string> companies = {"AAPL", "GOOGL", "MSFT", "AMZN", "TSLA"};
    int numCompanies = companies.size();

    // Starting stock price for each stock
    if (day == 1) {
        last_trade_prices["AAPL"] = 160.0;
        last_trade_prices["GOOGL"] = 120.0;
        last_trade_prices["MSFT"] = 100.0;
        last_trade_prices["AMZN"] = 180.0;
        last_trade_prices["TSLA"] = 80.0;
    }

    // Parameters for generating orders and random walk with drift simulation
    int maxUserId = 1000;
    double minPrice = 50.0;
    double maxPrice = 500.0;
    int minQuantity = 1;
    int maxQuantity = 50;
    double trend = 0.008;  // Small positive drift (upward trend)
    double volatility = 0.5; // 20% volatility, can be adjusted

    // Initial timestamp (e.g., trading day start)
    long long timestamp = day * 100000000;


    for (int i = 0; i < numOrders; ++i) {
        // Select a company and get its last trade price
        int companyIndex = rand() % numCompanies;
        string company = companies[companyIndex];
        double lastPrice = last_trade_prices[company];

        // Generate the random component using the Box-Muller transform
        double randomComponent = generateNormalRandom() * volatility;  // Scaled by volatility

        double companyTrend = trend + (rand() % 100 - 50) / 1000.0; // Small unique drift
        double price = lastPrice + companyTrend + randomComponent + log((companyIndex+50)/50); ;
        // Keep the price within realistic bounds
        // price = max(minPrice, min(price, maxPrice));

        // Save the price back to track the trend
        last_trade_prices[company] = price;

        // Generate realistic quantity with more orders around the median
        int medianQuantity = 25;
        int quantity = medianQuantity + static_cast<int>(rand() % (maxQuantity - minQuantity) - (maxQuantity - minQuantity) / 2);

        // Generate user ID and determine buy/sell with a slight tendency towards buys during uptrends
        int user_id = rand() % maxUserId + 1;
        bool is_buy = (rand() % 100 < (price > lastPrice ? 55 : 45));  // Bias toward buy in uptrend

        // Call spotTrade with the generated order details
        spotTrade(company, user_id, is_buy, price, timestamp, quantity);

        // Increment timestamp by 500 milliseconds for each order
        timestamp += 500;
    }
}