#ifndef FUTURE_CPP
#define FUTURE_CPP
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <unordered_map>
#include "spot.cpp" 
using namespace std;

const int NUM_USERS_FUTURE = 100;
const int NUM_COMPANIES_FUTURE = 3;
const int DAYS_IN_WEEK_FUTURE = 7; 
const double RISK_FREE_RATE_FUTURE = 0.05; 
const double DIVIDEND_YIELD_FUTURE = 0.02; 
const double VOLATILITY_ADJUSTMENT_FUTURE = 0.01; 

struct OrderFuture {
    int orderID_Future;
    int userID_Future;
    int companyID_Future;
    string date_Future;
    string timestamp_Future;
    string expirationDate_Future;
    double spotPrice_Future;
    double futurePrice_Future;
    string orderType_Future; 
    int lotSize_Future; // Random lot size
    string canceledOrderType_Future; // Additional field to capture canceled order type
};

double calculateFuturePriceFuture(double spotPrice_Future, double timeToExpiration_Future, double rate_Future, double dividendYield_Future, double volatilityAdjustment_Future) {
    return spotPrice_Future * exp((rate_Future - dividendYield_Future + volatilityAdjustment_Future) * timeToExpiration_Future);
}

string calculateExpirationDateFuture(int orderDay_Future) {
    int expirationDay_Future = orderDay_Future + (rand() % 7) + 1; // Adjusts within the week
    if (expirationDay_Future > DAYS_IN_WEEK_FUTURE) {
        expirationDay_Future %= DAYS_IN_WEEK_FUTURE;
    }
    return "Day " + to_string(expirationDay_Future);
}

string generateTimestampWithDayPrefixFuture(int day_Future) {
    int hour_Future = rand() % 6 + 9;
    int minute_Future = rand() % 60;
    int second_Future = rand() % 60;

    long long msSinceStart_Future = ((hour_Future - 9) * 3600 + minute_Future * 60 + second_Future) * 1000;
    char formattedTimestamp_Future[20];
    snprintf(formattedTimestamp_Future, sizeof(formattedTimestamp_Future), "%d%08lld", day_Future, msSinceStart_Future);

    return string(formattedTimestamp_Future);
}

long long getClosestTimestamp(const unordered_map<long long, vector<double>>& prices, long long target) {
    long long closestTimestamp = -1;
    long long minDifference = __LONG_LONG_MAX__;

    for (const auto& entry : prices) {
        long long timestamp = entry.first;
        long long difference = abs(timestamp - target);

        if (difference < minDifference) {
            minDifference = difference;
            closestTimestamp = timestamp;
        }
    }
    return closestTimestamp;
}

void generateFuturesTradingOrderBookFuture(const string& filename_Future, unordered_map<long long, vector<double>>& getspotprice) {
    ofstream file_Future(filename_Future);
    file_Future << "OrderID_Future,UserID_Future,CompanyID_Future,Date_Future,Timestamp_Future,ExpirationDate_Future,SpotPrice_Future,FuturePrice_Future,OrderType_Future,LotSize_Future,CanceledOrderType_Future\n";

    int orderID_Future = 1;
    vector<OrderFuture> orders_Future;

    for (int day_Future = 1; day_Future <= DAYS_IN_WEEK_FUTURE; ++day_Future) {
        vector<OrderFuture> dailyOrders_Future;

        for (int userID_Future = 1; userID_Future <= NUM_USERS_FUTURE; ++userID_Future) {
            if (rand() % 2 == 0) continue;

            string expirationDate_Future = calculateExpirationDateFuture(day_Future);
            string timestamp_Future = generateTimestampWithDayPrefixFuture(day_Future);
            long long time_future = stoll(timestamp_Future);

            int companyID_Future = rand() % NUM_COMPANIES_FUTURE;
            string date_Future = "Day " + to_string(day_Future);

            // Retrieve the closest available spot price if an exact timestamp match isn't found
            long long closestTimestamp = getClosestTimestamp(getspotprice, time_future);
            double spotPrice_Future = getspotprice[closestTimestamp][companyID_Future];
            
            // Calculate future price with weekly or monthly expiration
            bool isWeekly_Future = (rand() % 2 == 0);
            double timeToExpiration_Future = isWeekly_Future ? 1.0 / 52 : 1.0 / 12;
            double futurePrice_Future = calculateFuturePriceFuture(spotPrice_Future, timeToExpiration_Future, RISK_FREE_RATE_FUTURE, DIVIDEND_YIELD_FUTURE, VOLATILITY_ADJUSTMENT_FUTURE);

            // Randomly decide if the order is a "Buy", "Sell" or "Cancel"
            string orderType_Future;
            string canceledOrderType_Future = ""; // By default, no cancellation

            int orderChoice = rand() % 3;
            if (orderChoice == 0) {
                orderType_Future = "Buy";
            } else if (orderChoice == 1) {
                orderType_Future = "Sell";
            } else {
                orderType_Future = "Cancel";
                canceledOrderType_Future = (rand() % 2 == 0) ? "Buy" : "Sell"; // Record the canceled type
            }

            // Generate a random lot size between 1 and 10
            int lotSize_Future = rand() % 10 + 1;

            dailyOrders_Future.push_back({orderID_Future++, userID_Future, companyID_Future + 1, date_Future, timestamp_Future, expirationDate_Future, spotPrice_Future, futurePrice_Future, orderType_Future, lotSize_Future, canceledOrderType_Future});
        }

        sort(dailyOrders_Future.begin(), dailyOrders_Future.end(), [](const OrderFuture &a, const OrderFuture &b) {
            return a.timestamp_Future < b.timestamp_Future;
        });

        orders_Future.insert(orders_Future.end(), dailyOrders_Future.begin(), dailyOrders_Future.end());
    }

    for (const auto &order_Future : orders_Future) {
        file_Future << order_Future.orderID_Future << ","
                    << order_Future.userID_Future << ","
                    << order_Future.companyID_Future << ","
                    << order_Future.date_Future << ","
                    << order_Future.timestamp_Future << ","
                    << order_Future.expirationDate_Future << ","
                    << fixed << setprecision(2) << order_Future.spotPrice_Future << ","
                    << fixed << setprecision(2) << order_Future.futurePrice_Future << ","
                    << order_Future.orderType_Future << ","
                    << order_Future.lotSize_Future << ","
                    << order_Future.canceledOrderType_Future << "\n";
    }

    file_Future.close();
    cout << "Futures trading order book generated successfully." << endl;
}

#endif