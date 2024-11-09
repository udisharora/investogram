#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_set>
#include <cstdlib>

using namespace std;

// Define a struct to hold the stock data
struct StockData {
    long long timestamp;
    string company;
    double price;
    int buyer_id;
    int seller_id;

    // Constructor to initialize StockData from a line in the file
    StockData(long long t, string c, double p, int b_id, int s_id)
        : timestamp(t), company(c), price(p), buyer_id(b_id), seller_id(s_id) {}
};

// Function to read data from file into a vector of StockData for specific companies
vector<StockData> readData(const string& filename, const unordered_set<string>& companies) {
    vector<StockData> data;
    ifstream file(filename);
    string line;

    // Read the file line by line
    while (getline(file, line)) {
        stringstream ss(line);
        long long timestamp;
        string company;
        double price;
        int buyer_id, seller_id;

        // Extract data from each line
        ss >> timestamp >> company >> price >> buyer_id >> seller_id;

        // Only add the data for the specified companies
        if (companies.find(company) != companies.end()) {
            data.push_back(StockData(timestamp, company, price, buyer_id, seller_id));
        }
    }
    return data;
}

// Function to write sorted data to a file for a specific company
void writeData(const string& filename, const vector<StockData>& data) {
    ofstream file(filename);
    int num = 0;

    // Write each StockData object into the file
    for (const auto& stock : data) {
        if(num%100==0){
        file << num << " " << stock.company << " " << stock.price << " "
             << stock.buyer_id << " " << stock.seller_id << endl;
        }
        num++;
    }
}

// Function to generate gnuplot script for each stock
void plotStockGraph(const string& stock, const string& dataFile) {
    // Output file for the plot image
    string outputFileName = "plots/stock_market/" + stock + "_stock_prices.png";

    // Create the gnuplot script
    string gnuplotScript = 
        "set terminal png size 800,600\n"
        "set output '" + outputFileName + "'\n"
        "set title \"" + stock + " Stock Prices Over Time\"\n"
        "set xlabel \"Time (s)\"\n"
        "set ylabel \"Price ($)\"\n"
        "set grid\n"
        "unset xtics\n"  // Hide x-axis labels
        "set xdata time\n"
        "set timefmt \"%s\"\n"
        "set yrange [0:500]\n"  // Limiting x-axis range to 0 to 500 seconds
        "plot '" + dataFile + "' using 1:3 with lines title '" + stock + "' linecolor rgb 'blue'\n";

    // Save the script to a temporary file
    string scriptFileName = "temp_gnuplot_script.gp";
    ofstream scriptFile(scriptFileName);
    scriptFile << gnuplotScript;
    scriptFile.close();

    // Call gnuplot to generate the plot
    string gnuplotCommand = "gnuplot " + scriptFileName;
    system(gnuplotCommand.c_str());

    // Optionally, remove the temporary script file
    remove(scriptFileName.c_str());
}


int main() {
    string inputFile = "data/stock_prices.dat";  // Input file name
    unordered_set<string> companies = {"AAPL", "TSLA", "AMZN", "GOOGL", "MSFT"};

    // Read data from the file, only for the specified companies
    vector<StockData> stockData = readData(inputFile, companies);

    // Sort the data based on the timestamp using a lambda function
    sort(stockData.begin(), stockData.end(), [](const StockData& a, const StockData& b) {
        return a.timestamp < b.timestamp;
    });

    // Write the sorted data for each company to a separate file
    for (const auto& company : companies) {
        // Filter data for the current company
        vector<StockData> companyData;
        copy_if(stockData.begin(), stockData.end(), back_inserter(companyData),
                [&company](const StockData& stock) {
                    return stock.company == company;
                });

        // Create the output file name dynamically based on the company
        string outputFile = "data/plot/" + company + "_stock_prices.dat";
        writeData(outputFile, companyData);

        cout << "Data for " << company << " sorted and written to " << outputFile << endl;

        // Generate the plot for the current company
        plotStockGraph(company, outputFile);
        cout << "Plot for " << company << " has been generated." << endl;
    }

    return 0;
}
