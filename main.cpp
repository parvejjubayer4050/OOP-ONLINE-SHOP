// Online Shop & Inventory Management System
// Object-Oriented Programming II, Dong-eui University, Fall 2026
//
// Build (from the project root, all on one line):
//   clang++ -std=c++17 -Wall -Wextra -Iinclude main.cpp src/*.cpp -o output/main
//
// Run from the project root, so the data/ folder is found:
//   ./output/main

#include <iostream>

#include "Exceptions.h"
#include "ShopApp.h"
using namespace std;

int main() {
    try {
        ShopApp app("data");    // loads products, orders and users
        app.run();
    }
    catch (const ShopException& e) {
        // Most likely a damaged data file: stop with a clear message
        cout << "Startup error: " << e.what() << endl;
        return 1;
    }
    catch (const exception& e) {
        cout << "Fatal error: " << e.what() << endl;
        return 1;
    }
    return 0;
}