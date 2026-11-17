#include "Admin.h"

#include <iostream>
using namespace std;

Admin::Admin(const string& id, const string& username, const string& password)
    : User(id, username, password) {}

string Admin::getRole() const {
    return "admin";
}

void Admin::displayMenu() const {
    cout << "\n===== Admin Menu (" << getUsername() << ") =====" << endl;
    cout << "1. View inventory" << endl;
    cout << "2. Add product" << endl;
    cout << "3. Update product" << endl;
    cout << "4. Remove product" << endl;
    cout << "5. Update stock" << endl;
    cout << "6. View all orders" << endl;
    cout << "7. Logout" << endl;
}

string Admin::addProduct(Inventory& inventory, const string& name, double price, int stock) {
    string id = inventory.generateNextId();
    inventory.addProduct(Product(id, name, price, stock));   // Product validates the values
    return id;
}

void Admin::updateProduct(Inventory& inventory, const string& productId,
                          const string& newName, double newPrice) {
    inventory.updateProduct(productId, newName, newPrice);
}

void Admin::removeProduct(Inventory& inventory, const string& productId) {
    inventory.removeProduct(productId);
}

void Admin::updateStock(Inventory& inventory, const string& productId, int newStock) {
    inventory.updateStock(productId, newStock);
}