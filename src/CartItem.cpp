#include "CartItem.h"
#include "Exceptions.h"

using namespace std;

CartItem::CartItem(const string& productId,
                   const string& productName,
                   double unitPrice,
                   int quantity) {
    if (productId.empty() || productName.empty())
        throw ShopException("Cart item needs a product ID and a name");
    if (unitPrice < 0)
        throw ShopException("Cart item price cannot be negative");
    if (quantity < 1)
        throw ShopException("Cart item quantity must be at least 1");

    this->productId = productId;
    this->productName = productName;
    this->unitPrice = unitPrice;
    this->quantity = quantity;
}

void CartItem::addQuantity(int extra) {
    if (extra < 1)
        throw ShopException("Quantity to add must be at least 1");
    quantity += extra;
}

void CartItem::setUnitPrice(double newPrice) {
    if (newPrice < 0)
        throw ShopException("Cart item price cannot be negative");
    unitPrice = newPrice;
}