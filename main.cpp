#ifndef MAIN_CPP
#define MAIN_CPP
#include "spot.cpp"
#include "future.cpp"
#include "options.cpp"
#include "generating_function.cpp"
#include "fno.hpp"
#include <bits/stdc++.h>
using namespace std;

// Function to check if a string is a valid number (can include a decimal point)
bool isValidDouble(const string &str) {
    // Check if the string is not empty and contains valid characters for a floating-point number
    bool decimalPoint = false;
    for (char c : str) {
        if (!isdigit(c) && c != '.') {
            return false;
        }
        if (c == '.') {
            if (decimalPoint) {
                return false; // Multiple decimal points
            }
            decimalPoint = true;
        }
    }
    return !str.empty();
}

double safeStod(const string &str) {
    // Check if the string is valid
    if (isValidDouble(str)) {
        try {
            return stod(str); // Convert to double
        } catch (const invalid_argument &e) {
            cerr << "Invalid argument: " << str << endl;
            return -1.0; // Handle error, return a default or invalid value
        } catch (const out_of_range &e) {
            cerr << "Out of range: " << str << endl;
            return -1.0;
        }
    } else {
        cerr << "Non-numeric value: " << str << endl;
        return -1.0; // Return default value when input is invalid
    }
}

int main() {
    //create fresh stock prices file
    ofstream outputFile("data/stock_prices.dat");
    outputFile.clear();
    //FILE HEADER
    outputFile << "Timestamp" << " " << "Company" << " " << "Price" << " " << "Buyer ID" << " " << "Seller ID" << "\n";
    outputFile.close();
    //create fresh cancelled orders file
    ofstream file1("cancelled_orders.dat");
    file1.clear();
    file1.close();
    
    // Example: Generate 90000 orders for the simulation
    initializeTrie();
    int numDays = 7;
    int stampsDay = 43200;
    int number_user=1000;
    for(int i = 0; i < numDays; i++){
        int day = i+1;
        generateOrders(stampsDay, day);
        writeCancelledOrdersToFile(day);
    }
    // Generate the futures trading order book and save it to a CSV file
    generateFuturesTradingOrderBookFuture("data/fututres_trading_orderbook_future.csv",getspotprice);
    OptionSimulator simulator(150, NUM_USERS_FUTURE,4, numDays);
    simulator.generateOrderBook(getspotprice);  



    // create futures market
    FutureMarket futureMarket;

    // add contract to future market
    vector<string> companies = {"AAPL", "GOOGL", "MSFT", "AMZN", "TSLA"};
    vector<int> lots = {200, 400, 100, 150, 550};
    vector<int> expiries = {3, 4, 2, 5, 6};

    for (int i = 0; i < companies.size(); i++) {
        futureMarket.addContract(i, companies[i], lots[i], expiries[i]);
    }

    ifstream inputFile("data/fututres_trading_orderbook_future.csv");  // Open the CSV file
    string line;

    // Check if file is open
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file!" << std::endl;
        return 1;
    }

    // Reading each line from the CSV file
    while (std::getline(inputFile, line)) {
        vector<string> row;  // Store a single row of data
        stringstream lineStream(line);  // Turn the line into a stream
        string cell;

        // Split the line by comma
        while (getline(lineStream, cell, ',')) {
            row.push_back(cell);  // Add each cell to the row
        }

        // Check if the row is not empty
        if (!row.empty()) {
            // Check if the row is a header
            if (row[0] == "OrderID_Future") {
                continue;  // Skip the header
            }

            // Get the timestamp, company, price, buyer ID, and seller ID
            string companyID, userID, strike, futurePrice, amt, orderType, timestamp, cancelType;
            userID = row[1];
            companyID = row[2];
            timestamp = row[4];
            strike = row[6];
            futurePrice = row[7];
            amt = row[9];
            orderType = row[8];
            cancelType = row[10];
            

              if (orderType == "Cancel"){
                // cancel bid
                if (cancelType == "Buy") {
                    cancelType = "0";
                } else {
                    cancelType = "1";
                }
                futureMarket.cancelBid(stoi(userID), stoi(companyID), stoi(cancelType), stoi(amt), stoi(timestamp));
                continue;
            } else {
                // add bid
                if (orderType == "Buy") {
                    orderType = "0";
                } else {
                    orderType = "1";
                }
                futureMarket.addBid(stoi(userID), stoi(companyID), stod(strike), stod(futurePrice), stoi(amt), stoi(orderType));
            }
        }
    }

    inputFile.close();  // Close the file after reading

    map<int, map<string, int>> finalMargin = futureMarket.closeMarket();

    // add data from finalMargin through dofnotransaction function 
    for (auto it = finalMargin.begin(); it != finalMargin.end(); ++it) {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
            doFnOTransaction(it->first, it2->second, it2->first);
        }
    }
   

    //print the last trade prices for each stock
    for (auto it = last_trade_prices.begin(); it != last_trade_prices.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    char s;
    cout<<"terminate Y/N ";
    cin>>s;
    while(s!='Y'){
        getUserInfo(last_trade_prices);
        cout<<"terminate Y/N ";
        cin>>s;
    }
}
#endif