#pragma once

#include <string>

#include "Cart.h"
#include "Inventory.h"
#include "Order.h"
#include "User.h"

// ---------------------------------------------------------------
// Customer
//
// A shopper. Inherits id/username/password from User, supplies the
// customer menu, and OWNS one Cart (composition: the cart is a member
// object, created with the customer and destroyed with it).
//
// checkout() is where the cart meets the inventory.
// ---------------------------------------------------------------
class Customer : public User {
private:
    Cart cart;

public:
    Customer(const std::string& id,
             const std::string& username,
             const std::string& password);

    void displayMenu() const override;
    std::string getRole() const override;       // "customer"

    Cart& getCart() { return cart; }
    const Cart& getCart() const { return cart; }

    // Buy everything in the cart. Steps, in this order:
    //   1. The cart must not be empty.                    (ShopException)
    //   2. EVERY item must be in stock.                   (OutOfStockException /
    //                                                       ShopException)
    //   3. Prices are checked against the inventory. If any price changed
    //      since the items were added, the cart is updated and the checkout
    //      stops so the customer can review the new total.   (ShopException)
    //   4. The Order is built (with the price snapshot).
    //   5. Only now is stock deducted, and the cart is emptied.
    //
    // Nothing is changed unless everything is valid: if any step before 5
    // throws, the inventory and the cart stay exactly as they were.
    //
    // The caller (ShopApp) provides the new order ID and the date, and
    // afterwards saves the inventory and the returned order with FileManager.
    Order checkout(Inventory& inventory,
                   const std::string& orderId,
                   const std::string& date);
};