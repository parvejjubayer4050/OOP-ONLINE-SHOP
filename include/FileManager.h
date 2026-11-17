#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Inventory.h"
#include "Order.h"
#include "User.h"

// ---------------------------------------------------------------
// FileManager
//
// The only class that touches the data files. Every other class
// (Product, Inventory, User, Order, ...) knows how to turn ITSELF into
// one line of text (toLine) and back (fromLine), but it never opens a
// file. FileManager opens the files, reads or writes the lines, and
// reports problems with ShopException.
//
// Files live in one data folder (default "data/"):
//     data/products.txt    productID,name,price,stockQuantity
//     data/users.txt       userID,username,password,role
//     data/orders.txt      orderID,customerID,date,productID:quantity:unitPrice;...
//
// The paths are relative, so run the program from the project root.
// The data folder is created automatically when something is saved.
//
// Current state: products, users and orders are implemented.
// ---------------------------------------------------------------
class FileManager {
private:
    std::string dataFolder;     // e.g. "data"
    std::string productFile;    // e.g. "data/products.txt"
    std::string userFile;       // e.g. "data/users.txt"
    std::string orderFile;      // e.g. "data/orders.txt"

    // ----- shared helpers (reused by users and orders) -----

    // Read every line of a file. Throws ShopException if it cannot be opened.
    std::vector<std::string> readLines(const std::string& path) const;

    // Replace the whole file with these lines (each ends with '\n').
    // Writes to "<path>.tmp" first and renames it afterwards, so a failure
    // half-way through never destroys the old file.
    // Creates the data folder if needed. Throws ShopException on any failure.
    void writeLines(const std::string& path,
                    const std::vector<std::string>& lines) const;

    // Add ONE line at the end of a file, keeping everything already in it
    // (ios::app). Creates the file and the data folder if needed.
    void appendLine(const std::string& path, const std::string& line) const;

    // Create the folder that will contain `path`, if it does not exist.
    void ensureFolderFor(const std::string& path) const;

public:
    explicit FileManager(const std::string& dataFolder = "data");

    // ----- products -----

    // Save every product in the inventory (replaces the old file).
    void saveProducts(const Inventory& inventory) const;

    // Load products.txt into a new Inventory and return it.
    //   - File does not exist yet (first run) -> returns an EMPTY inventory.
    //   - File cannot be opened               -> ShopException.
    //   - A line is malformed or has a duplicate ID -> ShopException that
    //     names the line number. Nothing is returned in that case, so you
    //     never get a half-loaded inventory.
    Inventory loadProducts() const;

    // ----- users -----

    // Add ONE user at the end of users.txt. Old users stay (ios::app).
    void saveUser(const User& user) const;

    // Load every user from users.txt (Customers and Admins, created by
    // User::fromLine).
    //   - File does not exist yet -> returns an empty vector.
    //   - Malformed line, unknown role, duplicate ID or duplicate username
    //     -> ShopException naming the line number.
    std::vector<std::unique_ptr<User>> loadUsers() const;

    // ----- orders -----

    // Add ONE order at the end of orders.txt. Old orders stay (ios::app).
    void saveOrder(const Order& order) const;

    // Load every order from orders.txt.
    //   - File does not exist yet -> returns an empty vector.
    //   - Malformed line, bad item, or duplicate order ID
    //     -> ShopException naming the line number.
    std::vector<Order> loadOrders() const;

    const std::string& getDataFolder() const { return dataFolder; }
    const std::string& getProductFile() const { return productFile; }
    const std::string& getUserFile() const { return userFile; }
    const std::string& getOrderFile() const { return orderFile; }
};