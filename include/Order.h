#pragma once

#include <string>
#include <vector>

#include "Inventory.h"
#include "OrderItem.h"

// ---------------------------------------------------------------
// Order
//
// A completed purchase: who bought, when, and which items at which
// prices. An Order contains OrderItems (composition) and never
// changes after it is created.
//
// File record format (one order per line, used by FileManager):
//     orderID,customerID,date,productID:quantity:unitPrice;productID:...
//     O001,U002,2026-11-17,P001:1:750000;P002:2:15000
// ---------------------------------------------------------------
class Order {
private:
    std::string id;                 // e.g. "O001"
    std::string customerId;         // e.g. "U002"
    std::string date;               // "YYYY-MM-DD"
    std::vector<OrderItem> items;   // at least one

public:
    // Throws ShopException if id/customerId is empty or contains a comma,
    // the date is not YYYY-MM-DD, or there are no items.
    Order(const std::string& id,
          const std::string& customerId,
          const std::string& date,
          const std::vector<OrderItem>& items);

    // ----- getters -----
    const std::string& getId() const { return id; }
    const std::string& getCustomerId() const { return customerId; }
    const std::string& getDate() const { return date; }
    const std::vector<OrderItem>& getItems() const { return items; }

    double calculateTotal() const;

    // Prints the order as a receipt. The inventory is only used to show
    // product names (orders store IDs); a product that was deleted later
    // shows as "(removed product)".
    void printReceipt(const Inventory& inventory) const;

    // ----- file format -----
    std::string toLine() const;
    static Order fromLine(const std::string& line);     // ShopException if malformed

    // ----- helpers for creating orders -----
    static std::string todayDate();                                     // "2026-11-17"
    static std::string generateNextId(const std::vector<Order>& orders);   // "O001", "O002", ...
};