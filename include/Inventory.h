#pragma once

#include <string>
#include <vector>

#include "Product.h"

// ---------------------------------------------------------------
// Inventory
//
// Keeps the collection of products and is the SINGLE source of truth
// for stock. Customers (through Cart/checkout) and Admins both work
// with the same Inventory object, as shown on the workflow slide.
//
// Inventory does NOT read or write files. FileManager loads products
// from the file and hands them to Inventory, and later asks Inventory
// for its products (getProducts) so it can save them.
//
// Errors are reported with the exceptions from Exceptions.h:
//   - unknown product ID, duplicate ID, bad quantity -> ShopException
//   - not enough stock                               -> OutOfStockException
// ---------------------------------------------------------------
class Inventory {
private:
    std::vector<Product> products;

    // Position of the product with this ID, or -1 if it does not exist.
    int indexOf(const std::string& id) const;

public:
    Inventory() = default;

    // ----- managing products (Admin) -----
    void addProduct(const Product& product);          // ShopException if the ID already exists
    void removeProduct(const std::string& id);        // ShopException if the ID is not found
    void updateStock(const std::string& id, int newStock);   // set stock to an exact number
    void updateProduct(const std::string& id,
                       const std::string& newName,
                       double newPrice);              // change name and price

    // Next free ID in the style "P001", "P002", ... (so the admin does not
    // have to invent IDs by hand).
    std::string generateNextId() const;

    // ----- looking up products -----
    bool exists(const std::string& id) const;

    // Returns the product itself (not a copy). ShopException if not found.
    // Do not keep this reference after adding or removing products: the
    // vector may move its elements in memory.
    Product& findProduct(const std::string& id);
    const Product& findProduct(const std::string& id) const;

    // All products whose name contains the keyword (ignores upper/lower case).
    std::vector<Product> searchByName(const std::string& keyword) const;

    // ----- stock checks and changes (used by Cart and checkout) -----
    // Does nothing if the stock is enough; otherwise throws.
    //   ShopException       : unknown ID, or quantity <= 0
    //   OutOfStockException : not enough stock
    void checkStock(const std::string& id, int quantity) const;

    // Remove quantity units from stock. Call checkStock() for every cart
    // item first, then call this for every item.
    void reduceStock(const std::string& id, int quantity);

    // ----- display and access -----
    void displayProducts() const;                     // prints a table of all products
    const std::vector<Product>& getProducts() const { return products; }   // for FileManager
    int size() const { return static_cast<int>(products.size()); }
    bool isEmpty() const { return products.empty(); }
};