#pragma once

#include <string>
#include <vector>

#include "CartItem.h"
#include "Inventory.h"

// ---------------------------------------------------------------
// Cart
//
// The products a customer has picked but not bought yet. A Customer
// owns one Cart (composition), and a Cart contains CartItems.
//
//     vector<CartItem> items
//
// The cart never changes the inventory: stock is only deducted at
// checkout. But addItem() already checks the stock, so a customer
// cannot put more in the cart than the shop has.
//
// Rules:
//   - Adding the same product again increases the quantity of the
//     existing line (one line per product).
//   - Unknown product ID, or quantity < 1        -> ShopException
//   - Wanted quantity above the available stock  -> OutOfStockException
//
// Note: the price in a cart line is the product's price when the item
// was last added. Checkout will check the inventory again.
// ---------------------------------------------------------------
class Cart {
private:
    std::vector<CartItem> items;

    // Position of the line with this product ID, or -1.
    int indexOf(const std::string& productId) const;

public:
    Cart() = default;

    // ----- changing the cart -----

    // Add `quantity` units of a product. If the product is already in the
    // cart, the quantity is added to the existing line.
    // The TOTAL quantity in the cart (old + new) must be available in stock.
    void addItem(const Inventory& inventory,
                 const std::string& productId,
                 int quantity);

    // Remove a product's whole line. ShopException if it is not in the cart.
    void removeItem(const std::string& productId);

    // Empty the cart (used after a successful checkout).
    void clear() { items.clear(); }

    // ----- information -----
    bool isEmpty() const { return items.empty(); }
    bool contains(const std::string& productId) const;
    int getItemCount() const { return static_cast<int>(items.size()); }   // number of lines
    int getTotalQuantity() const;                                         // all units together
    double calculateTotal() const;                                        // sum of all subtotals
    const std::vector<CartItem>& getItems() const { return items; }       // for checkout

    // ----- checkout support -----

    // Set every line's price to the inventory's CURRENT price.
    // ShopException if a product no longer exists.
    void refreshPrices(const Inventory& inventory);

    // Check EVERY line against the inventory without changing anything.
    // Throws ShopException (product no longer exists) or OutOfStockException
    // (not enough stock) for the first problem found. Call this before
    // deducting any stock, so checkout never half-succeeds.
    void checkStock(const Inventory& inventory) const;

    // ----- display -----
    void display() const;       // prints the cart as a table with the total
};