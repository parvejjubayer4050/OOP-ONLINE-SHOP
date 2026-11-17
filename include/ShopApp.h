#pragma once

#include <string>
#include <vector>

#include "Admin.h"
#include "AuthService.h"
#include "Customer.h"
#include "FileManager.h"
#include "Inventory.h"
#include "Order.h"
#include "User.h"

// ---------------------------------------------------------------
// ShopApp
//
// The coordinator of the whole program. It owns the main objects
// (FileManager, Inventory, the list of Orders, AuthService), talks to
// the person at the keyboard, and calls the other classes to do the work.
//
// Flow:
//   startup   : load products, orders and users from the data folder
//   run()     : signup / login / quit loop
//   session   : after login, user->displayMenu() shows the Customer or
//               Admin menu (runtime polymorphism) and the choices are
//               handled until the user logs out
//   every change to products or orders is saved right away
//
// Errors: invalid input and shop rules (not enough stock, unknown
// product, ...) are caught here, printed, and the program carries on.
// A damaged data file at startup stops the program with a clear message.
// ---------------------------------------------------------------
class ShopApp {
private:
    // The order of these four matters: each one is created from the ones above it.
    FileManager fileManager;
    Inventory inventory;
    std::vector<Order> orders;      // every order of every customer
    AuthService authService;

    // ----- login screen -----
    void signupScreen();
    void loginScreen();

    // ----- one logged-in session -----
    void runSession(User* user);
    bool handleCustomerChoice(Customer& customer, const std::string& choice);   // false = logout
    bool handleAdminChoice(Admin& admin, const std::string& choice);            // false = logout

    // ----- customer actions -----
    void searchProducts() const;
    void addToCart(Customer& customer);
    void removeFromCart(Customer& customer);
    void checkout(Customer& customer);
    void viewMyOrders(const Customer& customer) const;

    // ----- admin actions -----
    void adminAddProduct(Admin& admin);
    void adminUpdateProduct(Admin& admin);
    void adminRemoveProduct(Admin& admin);
    void adminUpdateStock(Admin& admin);
    void viewAllOrders() const;

    // Save the products; if that fails, print a warning and keep going.
    void saveProductsOrWarn();

public:
    // Loads everything from `dataFolder`. On the very first run (no
    // products file yet) three sample products are created.
    // Throws ShopException if a data file is damaged.
    explicit ShopApp(const std::string& dataFolder = "data");

    void run();
};