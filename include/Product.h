#pragma once

#include <string>

// ---------------------------------------------------------------
// Product
//
// One item the shop sells. All data members are private
// (encapsulation): the rest of the program can only change a product
// through the methods below, and those methods refuse bad values.
//
//   - A price can never be negative.
//   - Stock can never be negative.
//   - You cannot remove more stock than the product has.
//
// Errors are reported with the exceptions from Exceptions.h.
//
// File record format (one product per line, used by FileManager):
//     productID,name,price,stockQuantity
//     P001,Laptop,750000,10
// ---------------------------------------------------------------
class Product {
private:
    std::string id;     // e.g. "P001"
    std::string name;   // e.g. "Laptop"   (must not contain a comma)
    double price;       // price of one unit
    int stock;          // units available right now

public:
    // Throws ShopException if id/name is empty or contains a comma,
    // or if price or stock is negative.
    Product(const std::string& id,
            const std::string& name,
            double price,
            int stock);

    // ----- getters -----
    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    // ----- setters (validated) -----
    void setName(const std::string& newName);   // ShopException if invalid
    void setPrice(double newPrice);             // ShopException if negative
    void setStock(int newStock);                // ShopException if negative

    // ----- stock operations -----
    bool hasStock(int quantity) const;          // true if stock >= quantity
    void increaseStock(int quantity);           // ShopException if quantity <= 0
    void reduceStock(int quantity);             // ShopException if quantity <= 0,
                                                // OutOfStockException if not enough

    // ----- display -----
    void display() const;                       // prints one table row

    // ----- file format -----
    // Convert to / from one line of products.txt.
    // FileManager does the actual reading and writing of the file;
    // Product only knows how its own record looks.
    std::string toLine() const;                 // "P001,Laptop,750000,10"
    static Product fromLine(const std::string& line);   // ShopException if the
                                                        // line is malformed
};