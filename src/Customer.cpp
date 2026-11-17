#include "Customer.h"
#include "Exceptions.h"

#include <iostream>
#include <vector>
using namespace std;

Customer::Customer(const string& id, const string& username, const string& password)
    : User(id, username, password) {}

string Customer::getRole() const {
    return "customer";
}

void Customer::displayMenu() const {
    cout << "\n===== Customer Menu (" << getUsername() << ") =====" << endl;
    cout << "1. View products" << endl;
    cout << "2. Search products" << endl;
    cout << "3. Add product to cart" << endl;
    cout << "4. Remove product from cart" << endl;
    cout << "5. View cart" << endl;
    cout << "6. Checkout" << endl;
    cout << "7. View my orders" << endl;
    cout << "8. Logout" << endl;
}

Order Customer::checkout(Inventory& inventory, const string& orderId, const string& date) {
    // 1. Empty cart
    if (cart.isEmpty())
        throw ShopException("Your cart is empty");

    // 2. Validate EVERY item before changing anything
    cart.checkStock(inventory);

    // 3. Price guard: the customer must pay what they were shown
    double shownTotal = cart.calculateTotal();
    cart.refreshPrices(inventory);
    if (cart.calculateTotal() != shownTotal)
        throw ShopException("Some prices changed since you added the items. "
                            "Your cart now shows the new prices; please review it and checkout again");

    // 4. Build the order (price snapshot). Anything that can throw happens
    //    here, BEFORE the inventory is touched.
    vector<OrderItem> orderItems;
    for (const CartItem& item : cart.getItems())
        orderItems.push_back(OrderItem(item.getProductId(), item.getQuantity(), item.getUnitPrice()));

    Order order(orderId, getId(), date, orderItems);

    // 5. Everything is valid: deduct stock and empty the cart
    for (const OrderItem& item : order.getItems())
        inventory.reduceStock(item.getProductId(), item.getQuantity());
    cart.clear();

    return order;
}