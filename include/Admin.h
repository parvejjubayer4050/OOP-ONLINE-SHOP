#pragma once

#include <string>

#include "Inventory.h"
#include "User.h"

// ---------------------------------------------------------------
// Admin
//
// The shop manager. Inherits id/username/password from User, supplies
// the admin menu, and manages products and stock in the shared
// Inventory.
//
// The methods below only do the work. Asking the admin for the values
// and saving the files afterwards is the job of ShopApp.
// All of them throw ShopException for invalid input (unknown product
// ID, negative price or stock, empty name, ...).
// ---------------------------------------------------------------
class Admin : public User {
public:
    Admin(const std::string& id,
          const std::string& username,
          const std::string& password);

    void displayMenu() const override;
    std::string getRole() const override;       // "admin"

    // Add a new product. The ID is generated ("P001", "P002", ...).
    // Returns the new product's ID.
    std::string addProduct(Inventory& inventory,
                           const std::string& name,
                           double price,
                           int stock);

    void updateProduct(Inventory& inventory,
                       const std::string& productId,
                       const std::string& newName,
                       double newPrice);

    void removeProduct(Inventory& inventory, const std::string& productId);

    void updateStock(Inventory& inventory, const std::string& productId, int newStock);
};