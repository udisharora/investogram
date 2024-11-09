#ifndef MANAGE_USER_CPP
#define MANAGE_USER_CPP
#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
using namespace std;

// Struct to hold user-specific information like cashed profit, stock holdings, and portfolio
struct UserInfo {
    unordered_map<string, double> cashed_profit;  
    unordered_map<string, int> stocks;  
    unordered_map<string, double> portfolio;  
    unordered_map<string, double> futuresnoptions;  
    // Constructor to initialize the cashed_profit with default values for predefined companies
    UserInfo() {
        vector<string> symbols = {"AAPL", "GOOGL", "MSFT", "AMZN", "TSLA"};
        for (const string& symbol : symbols) {
            cashed_profit[symbol] = 0.0;
            stocks[symbol] = 0;
            portfolio[symbol] = 0.0;
            futuresnoptions[symbol] = 0.0;
        }
    }
};

// Maps to store user information and user IDs
unordered_map<int, UserInfo> user_data;
unordered_map<string, int> userMap;

// TrieNode class to represent each node in the Trie for auto-complete functionality
class TrieNode {
public:
    unordered_map<char, TrieNode*> children;  // Child nodes for storing characters
    bool isEndOfWord;  // Flag to mark the end of a word

    TrieNode() : isEndOfWord(false) {}
};

// Trie class to handle operations related to inserting words and auto-completing based on a prefix
class Trie {
public:
    Trie() {
        root = new TrieNode();
    }

    // Insert a word into the Trie
    void insert(const string& word) {
        TrieNode* node = root;
        for (char ch : word) {
            if (!node->children.count(ch)) {
                node->children[ch] = new TrieNode();
            }
            node = node->children[ch];
        }
        node->isEndOfWord = true;
    }

    // Helper function to collect all words starting with a given prefix
    void collectWords(TrieNode* node, const string& prefix, vector<string>& results) {
        if (node->isEndOfWord) {
            results.push_back(prefix);  // Add word to results if it's the end of a word
        }

        // Recursively collect all possible words from this point
        for (auto& child : node->children) {
            collectWords(child.second, prefix + child.first, results);
        }
    }

    // Get all words starting with the given prefix
    vector<string> autocomplete(const string& prefix) {
        TrieNode* node = root;
        vector<string> results;

        // Traverse the Trie following the prefix
        for (char ch : prefix) {
            if (!node->children.count(ch)) {
                return results;  // Return empty result if no match for the prefix
            }
            node = node->children[ch];
        }

        // Collect all words that start with the given prefix
        collectWords(node, prefix, results);
        return results;
    }

private:
    TrieNode* root;  // Root of the Trie
};

Trie trie;  // Global Trie object to handle username prefix matching

void initializeTrie();
void getUserInfo(unordered_map<string, double> last_trade_prices);
void doTransaction(int buyer, int seller, double price, double quantity, const string &company);
void printportfolio(int userID, const string &username, unordered_map<string, double> last_trade_prices);
void plotportfolio(const unordered_map<string, double>& portfolio, const string& username, const double total_profit);

// Function to initialize the Trie and load user data from file
void initializeTrie() {
    const string filename = "data/userinfo.txt";
    ifstream file(filename);

    if (!file) {
        cerr << "Error opening file!" << endl;
        return;  // Return if file can't be opened
    }

    string line;
    while (getline(file, line)) {  // Read each line from the file
        stringstream ss(line);      // Use stringstream to split the line
        string username;
        int userId;

        ss >> username >> userId;  // Extract username and userId from the line

        // Store the username and userId in the map
        userMap[username] = userId;
        user_data[userId] = UserInfo();  // Initialize UserInfo for the user
        trie.insert(username);  // Insert username into the Trie for autocompletion
    }

    file.close();  // Close the file after reading
    return;
}

// Function to fetch and display user information based on the username prefix
void getUserInfo(unordered_map<string, double> last_trade_prices) {
    string prefix;
    cout << "Enter Username: ";
    cin >> prefix;

    vector<string> suggestions = trie.autocomplete(prefix);  // Get autocomplete suggestions

    if (!suggestions.empty()) {
        cout << "suggestions for username '" << prefix << "':" << endl;
        for (const string& suggestion : suggestions) {
            cout << suggestion << endl;
        }
        
        // Handle cases for 1 or multiple suggestions
        if (suggestions.size() == 1) {
            cout << "User ID: " << userMap[suggestions[0]] << endl;
            printportfolio(userMap[suggestions[0]], suggestions[0], last_trade_prices);  // Fetch and display user portfolio
        } else if (suggestions.size() > 1) {
            cout << "Multiple users found. Please enter the full username to get the user ID." << endl;
            getUserInfo(last_trade_prices);  // Recursive call if multiple users are found
        } else {
            cout << "No username." << endl;
        }
    } else {
        cout << "No username." << endl;
    }

    return;
}

// Function to perform a stock transaction between a buyer and a seller
void doTransaction(int buyer, int seller, double price, double quantity, const string &company) {
    user_data[buyer].cashed_profit[company] -= quantity * price;  // Deduct cash for the buyer
    user_data[buyer].stocks[company] += quantity;  // Add stock to the buyer
    user_data[seller].cashed_profit[company] += quantity * price;  // Add cash to the seller
    user_data[seller].stocks[company] -= quantity;  // Deduct stock from the seller
}

void doFnOTransaction(int buyer_order_id, double revenue,const string& company){
    user_data[buyer_order_id].futuresnoptions[company] == revenue;
}

// Function to print the portfolio of a specific user
void printportfolio(int userID, const string &username, unordered_map<string, double> last_trade_prices) {
    cout << "cashed_profit for user " << username << " (userID: " << userID << "):" << endl;
    double total_profit = 0.0;
    for (const auto& [company, profit] : user_data[userID].cashed_profit) {
        cout << company << ": " << profit << endl;  // Print the profit for each company
        user_data[userID].portfolio[company] = (last_trade_prices[company] * user_data[userID].stocks[company]) + user_data[userID].cashed_profit[company]+user_data[userID].futuresnoptions[company];  // Calculate the portfolio value
        total_profit += user_data[userID].portfolio[company];  // Calculate total profit
    }
    
    // Print last trade prices
    cout << "Last trade prices: " << endl;
    for (const auto& [company, price] : last_trade_prices) {
        cout << company << ": " << user_data[userID].stocks[company] << " at price " << price << endl;
    }
    
    cout << "Total profit (stock balance): " << total_profit << endl;

    plotportfolio(user_data[userID].portfolio, username, total_profit);  // Plot the portfolio
}

// Function to generate and save the portfolio plot
void plotportfolio(const unordered_map<string, double>& portfolio, const string& username, const double total_profit) {
    //Write portfolio data to a file for plotting
    ofstream dataFile("trash/portfolio_data.txt");
    int index = 0;
    for (const auto& [name, value] : portfolio) {
        dataFile << index << " " << value << " " << name << "\n";  // Write each stock's value and name
        index++;
    }
    dataFile.close();

    //Create a gnuplot script for the bar chart
    ofstream plotScript("trash/plot_script.plt");
    plotScript << "set terminal pngcairo size 800,600 background rgb 'white'\n"; // Set the background to white
    plotScript << "set output 'plots/portfolio_bar_chart.png'\n";
    plotScript << "set boxwidth 0.5\n";
    plotScript << "set style fill solid\n";
    plotScript << "set xlabel 'Company'\n";
    plotScript << "set ylabel 'Profit (in USD)'\n";
    plotScript << "set title 'portfolio of "<<username<<" -Total Profit= "<<total_profit<<"$'\n";
    plotScript << "set grid ytics\n";
    plotScript << "set xtics format ''\n";
    plotScript << "set xtics rotate by -45\n";
    plotScript << "set xtics (";

    // Set xtics labels for each stock
    index = 0;
    for (const auto& [name, value] : portfolio) {
        plotScript << "\"" << name << "\" " << index;
        if (++index < portfolio.size()) plotScript << ", ";
    }
    plotScript << ")\n";
    // Adding a line at y=0 to represent the x-axis
    plotScript << "set arrow from 0,0 to 5,0 nohead lt 1 lw 2 linecolor rgb 'black'\n"; // Line at y=0

    // Plot each data point with a different color
    plotScript << "plot ";
    index = 0;
    for (const auto& [name, value] : portfolio) {
        string color;
        if (name == "TSLA") {
            color = "#E31937"; // Tesla Red
        } else if (name == "GOOGL") {
            color = "#34A853"; // Google's Green
        } else if (name == "AAPL") {
            color = "#A2AAAD"; // Apple Silver-Grey
        } else if (name == "AMZN") {
            color = "#FF9900"; // Amazon Orange
        } else if (name == "MSFT") {
            color = "#0078D4"; // Microsoft Blue
        }

        plotScript << "'trash/portfolio_data.txt' using ($1==" << index 
                   << " ? $2 : 1/0):xtic(3) with boxes lc rgb '" << color << "' notitle";

        if (++index < portfolio.size()) plotScript << ", ";
    }
    plotScript << "\n";
    plotScript.close();

    // Execute gnuplot to generate the plot
    system("gnuplot trash/plot_script.plt");
    cout << "portfolio bar chart saved as 'plots/portfolio_bar_chart.png'\n";
}

// // Main function to start the program
// int main() {
//     initializeTrie();  // Initialize the Trie with user data

//     // Example data (last trade prices for companies)
//     unordered_map<string, double> last_trade_prices = {{"AAPL", 175.25}, {"GOOGL", 2800.12}, {"MSFT", 298.45}, {"AMZN", 3455.99}, {"TSLA", 850.55}};
    
//     // Example: UserInfo request and handling
//     getUserInfo(last_trade_prices);

//     return 0;
// }

#endif