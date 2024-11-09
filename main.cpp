#ifndef MAIN_CPP
#define MAIN_CPP
#include "spot.cpp"
#include "future.cpp"
#include "options.cpp"
#include "generating_function.cpp"
#include "fno.hpp"
#include <bits/stdc++.h>
using namespace std;

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


    // create options market
    OptionsMarket optionsMarket;

    for (int i = 0; i < companies.size(); i++) {
        optionsMarket.addContract(i, companies[i], lots[i], expiries[i]);
    }

    ifstream inputFile1("data/options_trading_orderbook.csv");  // Open the CSV file
    string line1;

    // Check if file is open
    if (!inputFile1.is_open()) {
        std::cerr << "Failed to open the file!" << std::endl;
        return 1;
    }
    int i=1;

    // Reading each line from the CSV file
    while (std::getline(inputFile1, line1)) {
        vector<string> row;  // Store a single row of data
        stringstream lineStream(line1);  // Turn the line into a stream
        string cell;

        // Split the line by comma
        while (getline(lineStream, cell, ',')) {
            row.push_back(cell);  // Add each cell to the row
        }

        // Check if the row is not empty
        if (!row.empty()) {
            // Check if the row is a header
            if (row[0] == "OrderID") {
                continue;  // Skip the header
            }

            // Get the timestamp, company, price, buyer ID, and seller ID
            string companyID, userID, strike, amt, orderType, timestamp, premium;
            userID = row[1];
            companyID = row[2];
            timestamp = row[4];
            strike = row[8];
            orderType = row[6];
            premium = row[9];
            i++;
            

            
            if (orderType == "Call") {
                orderType = "0";
            } else {
                orderType = "1";
            }
            optionsMarket.addBid(stoi(userID), stoi(companyID), stod(strike), (i%10 + 1) ,stoi(orderType), stoi(premium));
            
        }
    }

    inputFile.close();  // Close the file after reading


    map<int, map<string, int>> futureMargin = futureMarket.closeMarket();
    map<int, map<string, int>> finalMargin = optionsMarket.closeMarket();
    

    // add data from finalMargin through dofnotransaction function 
    for (auto it = finalMargin.begin(); it != finalMargin.end(); ++it) {
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
            doFnOTransaction(it->first, it2->second + futureMargin[it->first][it2->first], it2->first);
        }
    }
    
    // print the confirmation of the fno
    cout << "Futures and Options trading successfull." << endl;

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