#include "Cart.h"
#include "Exceptions.h"

#include <iomanip>
#include <iostream>
using namespace std;

// ---------------------------------------------------------------
// Private helper
// ---------------------------------------------------------------
int Cart::indexOf(const string& productId) const {
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].getProductId() == productId)
            return static_cast<int>(i);
    }
    return -1;
}

// ---------------------------------------------------------------
// Changing the cart
// ---------------------------------------------------------------
void Cart::addItem(const Inventory& inventory, const string& productId, int quantity) {
    if (quantity < 1)
        throw ShopException("Quantity must be at least 1");

    // Throws ShopException if the product does not exist
    const Product& product = inventory.findProduct(productId);

    int index = indexOf(productId);
    int alreadyInCart = (index >= 0) ? items[index].getQuantity() : 0;

    // The customer would hold (alreadyInCart + quantity) units: check THAT
    // against the stock, otherwise adding 8 and then 8 more of a product
    // with 10 in stock would slip through.
    // Throws OutOfStockException if not enough.
    inventory.checkStock(productId, alreadyInCart + quantity);

    if (index >= 0) {
        items[index].addQuantity(quantity);
        items[index].setUnitPrice(product.getPrice());     // keep the latest price
    }
    else {
        items.push_back(CartItem(product.getId(), product.getName(),
                                 product.getPrice(), quantity));
    }
}

void Cart::removeItem(const string& productId) {
    int index = indexOf(productId);
    if (index < 0)
        throw ShopException("Product is not in the cart: " + productId);
    items.erase(items.begin() + index);
}

// ---------------------------------------------------------------
// Information
// ---------------------------------------------------------------
bool Cart::contains(const string& productId) const {
    return indexOf(productId) >= 0;
}

int Cart::getTotalQuantity() const {
    int total = 0;
    for (const CartItem& item : items)
        total += item.getQuantity();
    return total;
}

double Cart::calculateTotal() const {
    double total = 0;
    for (const CartItem& item : items)
        total += item.calculateSubtotal();
    return total;
}

// ---------------------------------------------------------------
// Checkout support
// ---------------------------------------------------------------
void Cart::refreshPrices(const Inventory& inventory) {
    for (CartItem& item : items)
        item.setUnitPrice(inventory.findProduct(item.getProductId()).getPrice());
}

void Cart::checkStock(const Inventory& inventory) const {
    for (const CartItem& item : items)
        inventory.checkStock(item.getProductId(), item.getQuantity());
}

// ---------------------------------------------------------------
// Display
// ---------------------------------------------------------------
void Cart::display() const {
    if (items.empty()) {
        cout << "Your cart is empty." << endl;
        return;
    }

    cout << left
         << setw(8)  << "ID"
         << setw(22) << "Name"
         << right
         << setw(12) << "Price"
         << setw(6)  << "Qty"
         << setw(16) << "Subtotal"
         << endl;
    cout << string(64, '-') << endl;

    cout << fixed << setprecision(2);
    for (const CartItem& item : items) {
        cout << left
             << setw(8)  << item.getProductId()
             << setw(22) << item.getProductName()
             << right
             << setw(12) << item.getUnitPrice()
             << setw(6)  << item.getQuantity()
             << setw(16) << item.calculateSubtotal()
             << endl;
    }
    cout << string(64, '-') << endl;
    cout << left << setw(42) << "Total"
         << right << setw(22) << calculateTotal() << endl;

    cout.unsetf(ios::floatfield);   // restore default number formatting
    cout << left;                   // restore default alignment
}