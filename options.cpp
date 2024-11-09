#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <chrono>
#include "spot.cpp"  // Make sure this file defines 'getspotprice'

// Constants for the model
const int NUM_USERS = 100;
const int NUM_COMPANIES = 10;
const int DAYS_IN_MONTH = 30;
const double RISK_FREE_RATE = 0.05;
const double VOLATILITY = 0.3;
const double MATURITY_TIME = 30.0 / 365;
const double ORDER_PROBABILITY = 0.3;
const double MIN_CONTRACT_PRICE = 1.0;
const double SPOT_PRICE_START = 1000.0;
const int DAY_WEEK = 7;

class OptionSimulator {
public:
    OptionSimulator(double spotPriceStart, int numUsers, int numCompanies, int daysInMonth)
        : spotPrice(spotPriceStart), numUsers(numUsers), numCompanies(numCompanies), daysInMonth(daysInMonth) {}

    void generateOrderBook(const std::unordered_map<long long, std::vector<double>>& spotPrices);

private:
    double spotPrice;
    int numUsers;
    int numCompanies;
    int daysInMonth;

    double randomDouble(double min, double max);
    int randomInt(int min, int max);
    double N(double x);
    double blackScholesCall(double spotPrice, double strikePrice);
    double blackScholesPut(double spotPrice, double strikePrice);
    double simulateSpotPrice(double currentPrice);
    std::string calculateExpirationDate(int month);
    double chooseStrikePrice(double spotPrice);
    long long getClosestTimestamp(const std::unordered_map<long long, std::vector<double>>& prices, long long target);
    std::string generateRandomTimestamp(int day);
};

double OptionSimulator::randomDouble(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

int OptionSimulator::randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

double OptionSimulator::N(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

double OptionSimulator::blackScholesCall(double spotPrice, double strikePrice) {
    double d1 = (log(spotPrice / strikePrice) + (RISK_FREE_RATE + 0.5 * VOLATILITY * VOLATILITY) * MATURITY_TIME) / (VOLATILITY * sqrt(MATURITY_TIME));
    double d2 = d1 - VOLATILITY * sqrt(MATURITY_TIME);
    double callPrice = spotPrice * N(d1) - strikePrice * exp(-RISK_FREE_RATE * MATURITY_TIME) * N(d2);
    return std::max(callPrice, MIN_CONTRACT_PRICE);
}

double OptionSimulator::blackScholesPut(double spotPrice, double strikePrice) {
    double d1 = (log(spotPrice / strikePrice) + (RISK_FREE_RATE + 0.5 * VOLATILITY * VOLATILITY) * MATURITY_TIME) / (VOLATILITY * sqrt(MATURITY_TIME));
    double d2 = d1 - VOLATILITY * sqrt(MATURITY_TIME);
    double putPrice = strikePrice * exp(-RISK_FREE_RATE * MATURITY_TIME) * N(-d2) - spotPrice * N(-d1);
    return std::max(putPrice, MIN_CONTRACT_PRICE);
}

double OptionSimulator::simulateSpotPrice(double currentPrice) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::normal_distribution<> dis(0, 1);
    double drift = (RISK_FREE_RATE - 0.5 * VOLATILITY * VOLATILITY) * (1.0 / DAYS_IN_MONTH);
    double diffusion = VOLATILITY * sqrt(1.0 / DAYS_IN_MONTH) * dis(gen);
    return currentPrice * exp(drift + diffusion);
}

std::string OptionSimulator::calculateExpirationDate(int month) {
    int expirationDay_Future = month + (rand() % 7) + 1;
    if (expirationDay_Future > DAY_WEEK) {
        expirationDay_Future %= DAY_WEEK;
    }
    return std::to_string(expirationDay_Future);
}

double OptionSimulator::chooseStrikePrice(double spotPrice) {
    return round(spotPrice);
}

long long OptionSimulator::getClosestTimestamp(const std::unordered_map<long long, std::vector<double>>& prices, long long target) {
    long long closestTimestamp = -1;
    long long minDifference = __LONG_LONG_MAX__;
    for (const auto& entry : prices) {
        long long timestamp = entry.first;
        long long difference = std::abs(timestamp - target);
        if (difference < minDifference) {
            minDifference = difference;
            closestTimestamp = timestamp;
        }
    }
    return closestTimestamp;
}

std::string OptionSimulator::generateRandomTimestamp(int day) {
    int hour_Future = rand() % 6 + 9;
    int minute_Future = rand() % 60;
    int second_Future = rand() % 60;
    long long msSinceStart_Future = ((hour_Future - 9) * 3600 + minute_Future * 60 + second_Future) * 1000;
    char formattedTimestamp_Future[20];
    snprintf(formattedTimestamp_Future, sizeof(formattedTimestamp_Future), "%d%08lld", day, msSinceStart_Future);
    return std::string(formattedTimestamp_Future);
}

void OptionSimulator::generateOrderBook(const std::unordered_map<long long, std::vector<double>>& spotPrices) {
    std::ofstream file("data/options_trading_orderbook.csv");
    file << "OrderID,UserID,CompanyID,Date,Timestamp,ExpirationDate,OptionType,SpotPrice,StrikePrice,OptionPrice\n";
    int orderID = 1;

    for (int day = 1; day <= DAY_WEEK; ++day) {
        for (int userID = 1; userID <= numUsers; ++userID) {
            if (randomDouble(0, 1) <= ORDER_PROBABILITY) {
                int companyID = randomInt(1, numCompanies);
                std::string timestamp = generateRandomTimestamp(day);
                long long time_future = std::stoll(timestamp);
                long long closestTimestamp = getClosestTimestamp(spotPrices, time_future);
                double spotPrice_Future = spotPrices.at(closestTimestamp)[companyID];
                double strikePrice = chooseStrikePrice(spotPrice_Future);
                std::string optionType = randomDouble(0, 1) < 0.5 ? "Call" : "Put";
                double optionPrice = (optionType == "Call") ?
                    blackScholesCall(spotPrice_Future, strikePrice) :
                    blackScholesPut(spotPrice_Future, strikePrice);
                std::string expirationDate = calculateExpirationDate(day);

                file << orderID++ << ","
                     << userID << ","
                     << companyID << ","
                     << "2024-11-" << std::setw(2) << std::setfill('0') << day << ","
                     << timestamp << ","
                     << expirationDate << ","
                     << optionType << ","
                     << std::fixed << std::setprecision(2) << spotPrice_Future << ","
                     << strikePrice << ","
                     << std::fixed << std::setprecision(2) << optionPrice << "\n";
            }
        }
    }

    file.close();
    std::cout << "Option trading orderbook generated successfully!" << std::endl;
}
