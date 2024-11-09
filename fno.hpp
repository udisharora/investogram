#ifndef FNO_CPP
#define FNO_CPP
#include "spot.cpp"
#include <bits/stdc++.h>
#include <tuple>
using namespace std;

class FutureMarket{
    public:
    struct Contract{
        string companyName;
        int companyID, lot, expiry;
        Contract(string companyName, int companyID, int lot, int expiry)
            :companyID(companyID), lot(lot), expiry(expiry), companyName(companyName) {}

        Contract() : companyName(""), companyID(0), lot(0), expiry(0) {}
    };

    struct Bid{
        int strike, amt;
        Bid(int strike, int amt)
            :strike(strike), amt(amt) {}

        // Add a default constructor
        Bid() : strike(0), amt(0) {}
    };

    struct User{
        int userID;
        // map from companyID -> bid
        map<int, Bid> buyOrders;
        map<int, Bid> sellOrders;

        // map from companyName -> margin
        map<string, int> margin;

        User(int ID):userID(ID) {}
        
        User() : userID(0) {}
    };

    // map from companyID -> contract
    map<int, Contract> contracts;
    // map from userID -> user
    map<int, User> users;
    
    void addContract(int companyID, string companyName, int lot, int expiry){
        contracts[companyID] = Contract(companyName, companyID, lot, expiry);
    }

    string generateTimestampWithDayPrefixFuture(int day_Future) {
        int hour_Future = rand() % 6 + 9; // Business hours from 9 AM to 3 PM
        int minute_Future = rand() % 60;
        int second_Future = rand() % 60;

        long long msSinceStart_Future = ((hour_Future - 9) * 3600 + minute_Future * 60 + second_Future) * 1000;
        char formattedTimestamp_Future[20];
        snprintf(formattedTimestamp_Future, sizeof(formattedTimestamp_Future), "%d%08lld", day_Future, msSinceStart_Future);

        return std::string(formattedTimestamp_Future);
    }
    // Helper function to find the closest timestamp in case an exact match isn't available
    long long getClosestTimestamp(const unordered_map<long long, vector<double>>& prices, long long target) {
        long long closestTimestamp = -1;
        long long minDifference = LLONG_MAX;

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



    void addBid(int userID, int companyID, int strike, int futurePrice, int amt, int orderType){
        // check if user exists
        if(users.find(userID) == users.end()){
            users[userID] = User(userID);
        }
        
        // add bid to user
        if(orderType == 0){
            // if order doesnt exist, add it
            if (users[userID].buyOrders.find(companyID) == users[userID].buyOrders.end()){
                users[userID].buyOrders[companyID] = Bid(strike, amt);
            }else{
                // if order exists, update it
                int newAmt = users[userID].buyOrders[companyID].amt + amt;
                int newStrike = (users[userID].buyOrders[companyID].strike * users[userID].buyOrders[companyID].amt + strike * amt) / newAmt;
                users[userID].buyOrders[companyID] = Bid(newStrike, newAmt);
            }
        }else{
            // if order doesnt exist, add it
            if (users[userID].sellOrders.find(companyID) == users[userID].sellOrders.end()){
                users[userID].sellOrders[companyID] = Bid(strike, amt);
            }else{
                // if order exists, update it
                int newAmt = users[userID].sellOrders[companyID].amt + amt;
                int newStrike = (users[userID].sellOrders[companyID].strike * users[userID].sellOrders[companyID].amt + strike * amt) / newAmt;
                users[userID].sellOrders[companyID] = Bid(newStrike, newAmt);
            }
        }
    }

    void cancelBid(int userID, int companyID, int orderType, int amt, int time){
        // check if user exists
        if(users.find(userID) == users.end()){
            return;
        }

        // check if order exists
        if(users[userID].buyOrders.find(companyID) == users[userID].buyOrders.end()){
            return;
        }

        if(orderType == 0){
            // update margin
            int spotPrice = getspotprice[getClosestTimestamp(getspotprice, time)][contracts[companyID].companyID];
            users[userID].margin[contracts[companyID].companyName] += (spotPrice - users[userID].buyOrders[companyID].strike) * amt * contracts[companyID].lot;

            users[userID].buyOrders[companyID].amt -= amt;
            // if order is empty, remove it
            if(users[userID].buyOrders[companyID].amt == 0){
                users[userID].buyOrders.erase(companyID);
            }
        } else {
            // update margin
            int spotPrice = getspotprice[getClosestTimestamp(getspotprice, time)][contracts[companyID].companyID];
            users[userID].margin[contracts[companyID].companyName] -= (spotPrice - users[userID].sellOrders[companyID].strike) * amt * contracts[companyID].lot;

            users[userID].sellOrders[companyID].amt -= amt;
            // if order is empty, remove it
            if(users[userID].sellOrders[companyID].amt == 0){
                users[userID].sellOrders.erase(companyID);
            }
        }
    }

    map<int, map<string, int>> closeMarket(){
        // calculate final margin for every bid users have
        for(auto it = users.begin(); it != users.end(); ++it){
            for(auto it2 = it->second.buyOrders.begin(); it2 != it->second.buyOrders.end(); ++it2){
                long long timestamp = stoll(generateTimestampWithDayPrefixFuture(contracts[it2->first].expiry));
                int spotPrice = getspotprice[getClosestTimestamp(getspotprice, timestamp)][contracts[it2->first].companyID];
                it->second.margin[contracts[it2->first].companyName] += (spotPrice - it2->second.strike) * it2->second.amt * contracts[it2->first].lot;
            }
            for(auto it2 = it->second.sellOrders.begin(); it2 != it->second.sellOrders.end(); ++it2){
                long long timestamp = stoll(generateTimestampWithDayPrefixFuture(contracts[it2->first].expiry));
                int spotPrice = getspotprice[getClosestTimestamp(getspotprice, timestamp)][contracts[it2->first].companyID];
                it->second.margin[contracts[it2->first].companyName] -= (spotPrice - it2->second.strike) * it2->second.amt * contracts[it2->first].lot;
            }
        }

        // copy margin to a finalMargin
        map<int, map<string, int>> finalMargin;
        for(auto it = users.begin(); it != users.end(); ++it){
            finalMargin[it->first] = it->second.margin;
        }

        return finalMargin;
    }
};


class OptionsMarket{
    public:
    struct Contract{
        string companyName;
        int companyID, lot, expiry;
        Contract(string companyName, int companyID, int lot, int expiry)
            :companyID(companyID), lot(lot), expiry(expiry), companyName(companyName) {}

        Contract() : companyName(""), companyID(0), lot(0), expiry(0) {}
    };

    struct Bid{
        int strike, amt, premium;
        Bid(int strike, int amt, int premium)
            :strike(strike), amt(amt), premium(premium){}

        // Add a default constructor
        Bid() : strike(0), amt(0) {}
    };

    struct User{
        int userID;
        // map from companyID -> bid
        map<int, Bid> callOrders;
        map<int, Bid> putOrders;

        // map from companyName -> margin
        map<string, int> margin;

        User(int ID):userID(ID) {}
        
        User() : userID(0) {}
    };

    // map from companyID -> contract
    map<int, Contract> contracts;
    // map from userID -> user
    map<int, User> users;
    
    void addContract(int companyID, string companyName, int lot, int expiry){
        contracts[companyID] = Contract(companyName, companyID, lot, expiry);
    }

    string generateTimestampWithDayPrefixFuture(int day_Future) {
        int hour_Future = rand() % 6 + 9; // Business hours from 9 AM to 3 PM
        int minute_Future = rand() % 60;
        int second_Future = rand() % 60;

        long long msSinceStart_Future = ((hour_Future - 9) * 3600 + minute_Future * 60 + second_Future) * 1000;
        char formattedTimestamp_Future[20];
        snprintf(formattedTimestamp_Future, sizeof(formattedTimestamp_Future), "%d%08lld", day_Future, msSinceStart_Future);

        return std::string(formattedTimestamp_Future);
    }
    // Helper function to find the closest timestamp in case an exact match isn't available
    long long getClosestTimestamp(const unordered_map<long long, vector<double>>& prices, long long target) {
        long long closestTimestamp = -1;
        long long minDifference = LLONG_MAX;

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



    void addBid(int userID, int companyID, int strike, int amt, int orderType, int premium){
        // check if user exists
        if(users.find(userID) == users.end()){
            users[userID] = User(userID);
        }
        
        // add bid to user
        if(orderType == 0){
            // if order doesnt exist, add it
            if (users[userID].callOrders.find(companyID) == users[userID].callOrders.end()){
                users[userID].callOrders[companyID] = Bid(strike, amt, premium);
            }else{
                // if order exists, update it
                int newAmt = users[userID].callOrders[companyID].amt + amt;
                int newStrike = (users[userID].callOrders[companyID].strike * users[userID].callOrders[companyID].amt + strike * amt) / newAmt;
                int newPremium = (users[userID].callOrders[companyID].premium * users[userID].callOrders[companyID].amt + premium * amt) / newAmt;
                users[userID].callOrders[companyID] = Bid(newStrike, newAmt, newPremium);
            }
        }else{
            // if order doesnt exist, add it
            if (users[userID].putOrders.find(companyID) == users[userID].putOrders.end()){
                users[userID].putOrders[companyID] = Bid(strike, amt, premium);
            }else{
                // if order exists, update it
                int newAmt = users[userID].putOrders[companyID].amt + amt;
                int newStrike = (users[userID].putOrders[companyID].strike * users[userID].putOrders[companyID].amt + strike * amt) / newAmt;
                int newPremium = (users[userID].putOrders[companyID].premium * users[userID].putOrders[companyID].amt + premium * amt) / newAmt;
                users[userID].putOrders[companyID] = Bid(newStrike, newAmt, newPremium);
            }
        }
    }

    map<int, map<string, int>> closeMarket(){
        // calculate final margin for every bid users have
        for(auto it = users.begin(); it != users.end(); ++it){
            for(auto it2 = it->second.callOrders.begin(); it2 != it->second.callOrders.end(); ++it2){
                long long timestamp = stoll(generateTimestampWithDayPrefixFuture(contracts[it2->first].expiry));
                int spotPrice = getspotprice[getClosestTimestamp(getspotprice, timestamp)][contracts[it2->first].companyID];
                it->second.margin[contracts[it2->first].companyName] += (spotPrice - it2->second.strike) * it2->second.amt * contracts[it2->first].lot - it2->second.premium * it2->second.amt * contracts[it2->first].lot;
            }
            for(auto it2 = it->second.putOrders.begin(); it2 != it->second.putOrders.end(); ++it2){
                long long timestamp = stoll(generateTimestampWithDayPrefixFuture(contracts[it2->first].expiry));
                int spotPrice = getspotprice[getClosestTimestamp(getspotprice, timestamp)][contracts[it2->first].companyID];
                it->second.margin[contracts[it2->first].companyName] -= (spotPrice - it2->second.strike) * it2->second.amt * contracts[it2->first].lot - it2->second.premium * it2->second.amt * contracts[it2->first].lot;
            }
        }

        // copy margin to a finalMargin
        map<int, map<string, int>> finalMargin;
        for(auto it = users.begin(); it != users.end(); ++it){
            finalMargin[it->first] = it->second.margin;
        }

        return finalMargin;
    }
};

#endif