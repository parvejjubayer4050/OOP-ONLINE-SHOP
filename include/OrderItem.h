#pragma once

#include <string>

// ---------------------------------------------------------------
// OrderItem
//
// One line of a COMPLETED order: product ID, quantity, and the unit
// price paid. The price is a snapshot taken at checkout, so changing a
// product's price later never changes the total of an old order.
//
// File format of one item (items of an order are joined with ';'):
//     productID:quantity:unitPrice        e.g.  P001:1:750000
// ---------------------------------------------------------------
class OrderItem {
private:
    std::string productId;
    int quantity;           // always >= 1
    double unitPrice;       // price of ONE unit when it was bought

public:
    // Throws ShopException if the ID is empty or contains ',' ':' ';',
    // the quantity is less than 1, or the price is negative.
    OrderItem(const std::string& productId, int quantity, double unitPrice);

    const std::string& getProductId() const { return productId; }
    int getQuantity() const { return quantity; }
    double getUnitPrice() const { return unitPrice; }
    double calculateSubtotal() const { return unitPrice * quantity; }

    // ----- file format -----
    std::string toField() const;                            // "P001:1:750000"
    static OrderItem fromField(const std::string& field);   // ShopException if malformed
};