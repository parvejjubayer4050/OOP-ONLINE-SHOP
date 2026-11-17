#include "OrderItem.h"
#include "Exceptions.h"
#include "StringUtils.h"

#include <iomanip>
#include <sstream>
using namespace std;

OrderItem::OrderItem(const string& productId, int quantity, double unitPrice) {
    if (productId.empty())
        throw ShopException("Order item needs a product ID");
    if (productId.find_first_of(",:;") != string::npos)
        throw ShopException("Product ID '" + productId +
                            "' cannot be saved in an order (it contains , : or ;)");
    if (quantity < 1)
        throw ShopException("Order item quantity must be at least 1");
    if (unitPrice < 0)
        throw ShopException("Order item price cannot be negative");

    this->productId = productId;
    this->quantity = quantity;
    this->unitPrice = unitPrice;
}

string OrderItem::toField() const {
    ostringstream out;      // setprecision(15): big prices must not become "1.2e+06"
    out << productId << ":" << quantity << ":" << setprecision(15) << unitPrice;
    return out.str();
}

OrderItem OrderItem::fromField(const string& field) {
    vector<string> parts = splitBy(field, ':');
    if (parts.size() != 3)
        throw ShopException("Bad order item (expected productID:quantity:price): '" + field + "'");

    int quantity = parseInt(parts[1], "quantity");
    double price = parseDouble(parts[2], "price");
    return OrderItem(parts[0], quantity, price);    // constructor validates the rest
}