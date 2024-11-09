---
# Investogram

Investogram is a C++-based trading simulation project that offers a comprehensive environment for Spot Trading, Futures & Options Trading, and Portfolio Management. The platform mimics real-life trading mechanisms, enabling instant asset transactions, leveraged trading, and real-time portfolio tracking.

---

## Table of Contents
- [Features](#features)
- [Code Structure](#code-structure)
- [Collaborators](#collaborators)
- [Setup](#setup)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

---

## Features

### Spot Trading
- Allows users to buy and sell assets instantly at the current market price.
- Facilitates basic trading operations for immediate transactions.

### Futures & Options Trading
- Enables users to trade futures and options, exploring leveraged positions.
- Includes mechanisms for managing risk through options.

### Portfolio Management
- Tracks user assets in real time, allowing users to monitor and manage their portfolio performance.
- Provides insights into investment returns and asset allocation.

---

## Code Structure

Here’s an overview of the main components of the codebase:

- **data/**: Stores data files related to trading and user interactions.
- **plots/**: Contains files for visualizing market and portfolio data.
- **trash/**: Auxiliary or temporary files.
- **fno.hpp**: Defines data structures and functions for Futures & Options trading.
- **Future.cpp**: Manages futures trading operations and data.
- **generate_userdata.cpp**: Generates sample user data for testing and simulations.
- **generating_function.cpp**: Implements functions for creating realistic trading data.
- **graph_plotter.cpp**: Plots real-time or historical market data for analysis.
- **main.cpp**: Initializes the application and controls program flow.
- **manage_user.cpp**: Handles user-related functionalities, including registration and profile management.
- **options.cpp**: Contains core logic for options trading.
- **spot.cpp**: Manages spot trading operations, including instant buy/sell functionalities.

### Data Structures & Algorithms Implemented
- Tries
- Box-Muller Transformation
- Random Walk with Drift
- Hashmaps
- Max Heap & Min Heap
- And many more for optimized trading simulation

---

## Collaborators

- **Udish Arora**: Developed spot trading functionality and data generation for spot trading.
- **Shivam Goel**: Data Generation functionality for futures and options trading data.
- **Lakshay**: Created the futures and options market simulator using the data generated.
- **Shubham Gupta**: Managed user portfolios and contributed to data generation for spot trading.

---

## Setup

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/investogram.git
   ```

2. **Navigate to the project directory:**
   ```bash
   cd investogram
   ```

3. **Compile the project:**
   ```bash
   g++ main.cpp -o investogram
   ```

---

## Usage

1. Run the main application to perform trades and access user data:
   ```bash
   g++ main.cpp -o main
   ./main
   ```

2. For visualizing stock market data, execute `graph_plotter.cpp`:
   ```bash
   g++ graph_plotter.cpp -o graph_plotter
   ./graph_plotter
   ```

3. Follow the on-screen instructions to explore different trading options, simulate trades, and manage portfolios.

---

## Contributing

We welcome contributions! Please fork the repository, create a branch, make your changes, and open a pull request. 

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

---
