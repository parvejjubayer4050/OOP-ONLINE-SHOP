#include "Inventory.h"
#include "Exceptions.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
using namespace std;

namespace {

// Lower-case copy of a string, for searching without caring about case.
string toLower(string text) {
    for (char& c : text)
        c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return text;
}

} // namespace


// ---------------------------------------------------------------
// Private helper
// ---------------------------------------------------------------
int Inventory::indexOf(const string& id) const {
    for (size_t i = 0; i < products.size(); i++) {
        if (products[i].getId() == id)
            return static_cast<int>(i);
    }
    return -1;
}

// ---------------------------------------------------------------
// Managing products
// ---------------------------------------------------------------
void Inventory::addProduct(const Product& product) {
    if (exists(product.getId()))
        throw ShopException("Product ID already exists: " + product.getId());
    products.push_back(product);
}

void Inventory::removeProduct(const string& id) {
    int index = indexOf(id);
    if (index < 0)
        throw ShopException("Product not found: " + id);
    products.erase(products.begin() + index);
}

void Inventory::updateStock(const string& id, int newStock) {
    findProduct(id).setStock(newStock);     // Product rejects a negative stock
}

void Inventory::updateProduct(const string& id, const string& newName, double newPrice) {
    Product& product = findProduct(id);

    // Validate both new values BEFORE changing anything, so a bad price
    // does not leave the product with a new name and an old price.
    Product check(id, newName, newPrice, product.getStock());   // throws if invalid
    (void)check;

    product.setName(newName);
    product.setPrice(newPrice);
}

string Inventory::generateNextId() const {
    int highest = 0;
    for (const Product& p : products) {
        const string& id = p.getId();
        // Only look at IDs shaped like "P" + up to 9 digits
        if (id.size() < 2 || id.size() > 10 || id[0] != 'P')
            continue;
        bool allDigits = all_of(id.begin() + 1, id.end(), [](char c) {
            return isdigit(static_cast<unsigned char>(c)) != 0;
        });
        if (!allDigits)
            continue;
        highest = max(highest, stoi(id.substr(1)));
    }

    string number = to_string(highest + 1);
    if (number.size() < 3)
        number = string(3 - number.size(), '0') + number;
    return "P" + number;
}

// ---------------------------------------------------------------
// Looking up products
// ---------------------------------------------------------------
bool Inventory::exists(const string& id) const {
    return indexOf(id) >= 0;
}

Product& Inventory::findProduct(const string& id) {
    int index = indexOf(id);
    if (index < 0)
        throw ShopException("Product not found: " + id);
    return products[index];
}

const Product& Inventory::findProduct(const string& id) const {
    int index = indexOf(id);
    if (index < 0)
        throw ShopException("Product not found: " + id);
    return products[index];
}

vector<Product> Inventory::searchByName(const string& keyword) const {
    vector<Product> matches;
    string wanted = toLower(keyword);
    for (const Product& p : products) {
        if (toLower(p.getName()).find(wanted) != string::npos)
            matches.push_back(p);
    }
    return matches;
}

// ---------------------------------------------------------------
// Stock checks and changes
// ---------------------------------------------------------------
void Inventory::checkStock(const string& id, int quantity) const {
    if (quantity <= 0)
        throw ShopException("Quantity must be positive");

    const Product& product = findProduct(id);       // throws if the ID is unknown
    if (!product.hasStock(quantity))
        throw OutOfStockException(product.getName(), quantity, product.getStock());
}

void Inventory::reduceStock(const string& id, int quantity) {
    findProduct(id).reduceStock(quantity);          // Product throws if not enough
}

// ---------------------------------------------------------------
// Display
// ---------------------------------------------------------------
void Inventory::displayProducts() const {
    if (products.empty()) {
        cout << "No products in the inventory." << endl;
        return;
    }

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