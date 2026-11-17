#include "ShopApp.h"
#include "Exceptions.h"
#include "StringUtils.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <system_error>
using namespace std;
namespace fs = std::filesystem;

namespace {

// ---------------------------------------------------------------
// Keyboard input helpers
// ---------------------------------------------------------------

// Thrown when the input ends (Ctrl+D, or a finished script). It is not a
// ShopException, so only run() catches it and the program ends politely.
struct InputEnded {};

string trim(const string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos)
        return "";
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

// Show a prompt and read one whole line (so names with spaces work).
string readText(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line))
        throw InputEnded();
    return trim(line);
}

// Reading a number: typing "abc" gives a ShopException, which the menu
// loop catches and prints, so the program never crashes on bad input.
int readInt(const string& prompt) {
    return parseInt(readText(prompt), "whole number");
}

double readDouble(const string& prompt) {
    return parseDouble(readText(prompt), "number");
}

bool askYesNo(const string& prompt) {
    string answer = readText(prompt + " (y/n): ");
    return answer == "y" || answer == "Y" || answer == "yes" || answer == "YES";
}

// ---------------------------------------------------------------
// Output helpers
// ---------------------------------------------------------------
string money(double amount) {
    ostringstream out;
    out << fixed << setprecision(2) << amount;
    return out.str();
}

void printProductTable(const vector<Product>& products) {
    cout << left
         << setw(8)  << "ID"
         << setw(22) << "Name"
         << right
         << setw(12) << "Price"
         << setw(8)  << "Stock"
         << endl;
    cout << string(50, '-') << endl;
    for (const Product& p : products)
        p.display();
}

} // namespace


// ---------------------------------------------------------------
// Startup
// ---------------------------------------------------------------
ShopApp::ShopApp(const string& dataFolder)
    : fileManager(dataFolder),
      inventory(fileManager.loadProducts()),
      orders(fileManager.loadOrders()),
      authService(fileManager) {

    // Very first run: no products file yet -> create sample products
    // so there is something to show.
    error_code ec;
    if (!fs::exists(fileManager.getProductFile(), ec)) { 
        inventory.addProduct(Product("P001", "Laptop", 750000, 10));
        inventory.addProduct(Product("P002", "Mouse", 15000, 25));
        inventory.addProduct(Product("P003", "Keyboard", 30000, 15));
        fileManager.saveProducts(inventory);
    }
};

// ---------------------------------------------------------------
// Main loop: signup / login / quit
// ---------------------------------------------------------------
void ShopApp::run() {
    cout << "===== Online Shop & Inventory Management System =====" << endl;

    try {
        while (true) {
            cout << "\n1. Signup  2. Login  3. Quit" << endl;
            string choice = readText("Choose: ");

            if (choice == "3") {
                cout << "Goodbye!" << endl;
                return;
            }

            try {
                if (choice == "1")
                    signupScreen();
                else if (choice == "2")
                    loginScreen();
                else
                    cout << "Please enter 1, 2 or 3." << endl;
            }
            // Catch the most specific exception first, the base class last
            catch (const OutOfStockException& e) {
                cout << "Out of stock: " << e.what() << endl;
            }
            catch (const ShopException& e) {
                cout << "Error: " << e.what() << endl;
            }
            catch (const exception& e) {
                cout << "Unexpected error: " << e.what() << endl;
            }
        }
    }
    catch (const InputEnded&) {
        cout << "\nInput ended. Goodbye!" << endl;
    }
};

void ShopApp::signupScreen() {
    string username = readText("Enter username: ");
    string password = readText("Enter password: ");

    authService.signup(username, password);     // throws ShopException if invalid
    cout << "Signup successful! You can now log in." << endl;
}

void ShopApp::loginScreen() {
    string username = readText("Enter username: ");
    string password = readText("Enter password: ");

    User* user = authService.login(username, password);
    if (user == nullptr) {
        cout << "Login failed." << endl;
        return;
    }
    runSession(user);
}

// ---------------------------------------------------------------
// One logged-in session
// ---------------------------------------------------------------
void ShopApp::runSession(User* user) {
    cout << "Welcome, " << user->getUsername() << " (" << user->getRole() << ")" << endl;

    bool loggedIn = true;
    while (loggedIn) {
        user->displayMenu();        // runtime polymorphism: Customer or Admin menu
        string choice = readText("Choose: ");

        try {
            // The menu was chosen by polymorphism. To handle the choices we
            // need the real type; dynamic_cast gives nullptr for the wrong one.
            if (Customer* customer = dynamic_cast<Customer*>(user))
                loggedIn = handleCustomerChoice(*customer, choice);
            else if (Admin* admin = dynamic_cast<Admin*>(user))
                loggedIn = handleAdminChoice(*admin, choice);
            else
                loggedIn = false;
        }
        catch (const OutOfStockException& e) {
            cout << "Out of stock: " << e.what() << endl;
        }
        catch (const ShopException& e) {
            cout << "Error: " << e.what() << endl;
        }
        catch (const exception& e) {
            cout << "Unexpected error: " << e.what() << endl;
        }
    }
    cout << "Logged out." << endl;
}

// ---------------------------------------------------------------
// Customer menu
// ---------------------------------------------------------------
bool ShopApp::handleCustomerChoice(Customer& customer, const string& choice) {
    if (choice == "1")
        inventory.displayProducts();
    else if (choice == "2")
        searchProducts();
    else if (choice == "3")
        addToCart(customer);
    else if (choice == "4")
        removeFromCart(customer);
    else if (choice == "5")
        customer.getCart().display();
    else if (choice == "6")
        checkout(customer);
    else if (choice == "7")
        viewMyOrders(customer);
    else if (choice == "8")
        return false;
    else
        cout << "Invalid choice. Please enter a number from 1 to 8." << endl;
    return true;
}

void ShopApp::searchProducts() const {
    string keyword = readText("Search for: ");
    vector<Product> matches = inventory.searchByName(keyword);

    if (matches.empty())
        cout << "No products match '" << keyword << "'." << endl;
    else
        printProductTable(matches);
}

void ShopApp::addToCart(Customer& customer) {
    string productId = readText("Product ID: ");
    int quantity = readInt("Quantity: ");

    customer.getCart().addItem(inventory, productId, quantity);

    cout << "Added to cart. Your cart has " << customer.getCart().getTotalQuantity()
         << " item(s), total " << money(customer.getCart().calculateTotal()) << endl;
}

void ShopApp::removeFromCart(Customer& customer) {
    if (customer.getCart().isEmpty()) {
        cout << "Your cart is empty." << endl;
        return;
    }
    customer.getCart().display();
    string productId = readText("Product ID to remove: ");

    customer.getCart().removeItem(productId);
    cout << "Removed " << productId << " from your cart." << endl;
}

void ShopApp::checkout(Customer& customer) {
    if (customer.getCart().isEmpty()) {
        cout << "Your cart is empty." << endl;
        return;
    }

    customer.getCart().display();
    if (!askYesNo("Confirm checkout?")) {
        cout << "Checkout cancelled." << endl;
        return;
    }

    // Customer::checkout validates everything first and throws if anything
    // is wrong, leaving the inventory and the cart untouched.
    string orderId = Order::generateNextId(orders);
    Order order = customer.checkout(inventory, orderId, Order::todayDate());
    orders.push_back(order);

    // The sale has happened in memory: now save it. Save the order first so
    // a sale is never lost.
    try {
        fileManager.saveOrder(order);
        fileManager.saveProducts(inventory);
    }
    catch (const ShopException& e) {
        cout << "Warning: your order was placed, but it could not be saved to the file: "
             << e.what() << endl;
    }

    cout << "Order placed. Thank you!" << endl;
    order.printReceipt(inventory);
}

void ShopApp::viewMyOrders(const Customer& customer) const {
    bool any = false;
    for (const Order& order : orders) {
        if (order.getCustomerId() == customer.getId()) {
            order.printReceipt(inventory);
            any = true;
        }
    }
    if (!any)
        cout << "You have no orders yet." << endl;
}

// ---------------------------------------------------------------
// Admin menu
// ---------------------------------------------------------------
bool ShopApp::handleAdminChoice(Admin& admin, const string& choice) {
    if (choice == "1")
        inventory.displayProducts();
    else if (choice == "2")
        adminAddProduct(admin);
    else if (choice == "3")
        adminUpdateProduct(admin);
    else if (choice == "4")
        adminRemoveProduct(admin);
    else if (choice == "5")
        adminUpdateStock(admin);
    else if (choice == "6")
        viewAllOrders();
    else if (choice == "7")
        return false;
    else
        cout << "Invalid choice. Please enter a number from 1 to 7." << endl;
    return true;
}

void ShopApp::adminAddProduct(Admin& admin) {
    string name = readText("Product name: ");
    double price = readDouble("Price: ");
    int stock = readInt("Stock quantity: ");

    string id = admin.addProduct(inventory, name, price, stock);
    saveProductsOrWarn();
    cout << "Product added with ID " << id << "." << endl;
}

void ShopApp::adminUpdateProduct(Admin& admin) {
    string id = readText("Product ID to update: ");
    const Product& current = inventory.findProduct(id);     // ShopException if unknown
    cout << "Current: " << current.getName() << ", price " << money(current.getPrice()) << endl;

    string name = readText("New name: ");
    double price = readDouble("New price: ");

    admin.updateProduct(inventory, id, name, price);
    saveProductsOrWarn();
    cout << "Product " << id << " updated." << endl;
}

void ShopApp::adminRemoveProduct(Admin& admin) {
    string id = readText("Product ID to remove: ");
    const Product& product = inventory.findProduct(id);     // ShopException if unknown

    if (!askYesNo("Remove " + id + " (" + product.getName() + ")?")) {
        cout << "Cancelled." << endl;
        return;
    }

    admin.removeProduct(inventory, id);
    saveProductsOrWarn();
    cout << "Product " << id << " removed." << endl;
}

void ShopApp::adminUpdateStock(Admin& admin) {
    string id = readText("Product ID: ");
    cout << "Current stock: " << inventory.findProduct(id).getStock() << endl;
    int newStock = readInt("New stock quantity: ");

    admin.updateStock(inventory, id, newStock);
    saveProductsOrWarn();
    cout << "Stock of " << id << " is now " << newStock << "." << endl;
}

void ShopApp::viewAllOrders() const {
    if (orders.empty()) {
        cout << "There are no orders yet." << endl;
        return;
    }
    for (const Order& order : orders)
        order.printReceipt(inventory);
    cout << "\nTotal orders: " << orders.size() << endl;
}

void ShopApp::saveProductsOrWarn() {
    try {
        fileManager.saveProducts(inventory);
    }
    catch (const ShopException& e) {
        cout << "Warning: the change was made but could not be saved to the file: "
             << e.what() << endl;
    }
}