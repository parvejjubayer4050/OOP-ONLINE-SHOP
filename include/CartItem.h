#pragma once

#include <string>

// ---------------------------------------------------------------
// CartItem
//
// One line of a shopping cart: "2 x Mouse at 15000 each".
// It stores the product's ID, name and price as they were when the
// item was put in the cart (a copy, not a pointer to the Product),
// plus the quantity.
//
// Quantity must always be at least 1 and the price cannot be negative;
// the constructor and methods throw ShopException otherwise.
// ---------------------------------------------------------------
class CartItem {
private:
    std::string productId;
    std::string productName;
    double unitPrice;       // price of ONE unit
    int quantity;           // always >= 1

public:
    // Throws ShopException if the ID/name is empty, the price is negative
    // or the quantity is less than 1.
    CartItem(const std::string& productId,
             const std::string& productName,
             double unitPrice,
             int quantity);

    // ----- getters -----
    const std::string& getProductId() const { return productId; }
    const std::string& getProductName() const { return productName; }
    double getUnitPrice() const { return unitPrice; }
    int getQuantity() const { return quantity; }

    // ----- changes -----
    void addQuantity(int extra);            // ShopException if extra < 1
    void setUnitPrice(double newPrice);     // ShopException if negative

    // ----- calculation -----
    double calculateSubtotal() const { return unitPrice * quantity; }
};