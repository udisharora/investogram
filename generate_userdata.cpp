#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// List of common first and last names
vector<string> first_names = {
    "James", "Mary", "John", "Patricia", "Robert", "Jennifer", "Michael", "Elizabeth", "William", "Linda",
    "David", "Barbara", "Richard", "Susan", "Joseph", "Jessica", "Charles", "Sarah", "Thomas", "Karen",
    "Christopher", "Nancy", "Daniel", "Lisa", "Matthew", "Betty", "Anthony", "Helen", "Mark", "Sandra",
    "Donald", "Donna", "Paul", "Carol", "Steven", "Ruth", "Andrew", "Sharon", "Kenneth", "Michelle",
    "George", "Laura", "Joshua", "Emily", "Kevin", "Dorothy", "Brian", "Amanda", "Edward", "Deborah",
    "Ronald", "Rebecca", "Timothy", "Shirley", "Jason", "Cynthia", "Jeffrey", "Kathleen", "Gary", "Amy"
};

vector<string> last_names = {
    "Smith", "Johnson", "Williams", "Jones", "Brown", "Davis", "Miller", "Wilson", "Moore", "Taylor",
    "Anderson", "Thomas", "Jackson", "White", "Harris", "Martin", "Thompson", "Garcia", "Martinez",
    "Roberts", "Clark", "Rodriguez", "Lewis", "Lee", "Walker", "Hall", "Allen", "Young", "King",
    "Wright", "Scott", "Torres", "Nguyen", "Adams", "Baker", "Gonzalez", "Nelson", "Carter", "Mitchell",
    "Perez", "Robinson", "Hernandez", "Jackson", "West", "Collins", "Stewart", "Sanchez", "Morris",
    "Rogers", "Reed", "Cook", "Morgan", "Bell", "Murphy"
};

// Function to generate a random username with user ID
string generate_username(int user_id) {
    // Randomly select a first and last name from the list
    string first_name = first_names[rand() % first_names.size()];
    string last_name = last_names[rand() % last_names.size()];

    // Return the formatted username
    return first_name + last_name + " " + to_string(user_id);
}

int main() {
    srand(time(0));  // Seed the random number generator

    // Open file to write usernames
    ofstream outfile("data/userinfo.txt");

    if (outfile.is_open()) {
        // Generate and write 1000 usernames to the file
        for (int i = 1; i <= 1000; i++) {
            outfile << generate_username(i) << endl;
        }

        // Close the file after writing
        outfile.close();
        cout << "Usernames have been written to userinfo.txt" << endl;
    } else {
        cout << "Unable to open file for writing." << endl;
    }

    return 0;
}
