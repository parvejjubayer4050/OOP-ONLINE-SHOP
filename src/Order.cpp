#include "Order.h"
#include "Exceptions.h"
#include "StringUtils.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <iostream>
using namespace std;

namespace {

void checkText(const string& value, const string& fieldName) {
    if (value.empty())
        throw ShopException("Order " + fieldName + " cannot be empty");
    if (value.find(',') != string::npos)
        throw ShopException("Order " + fieldName + " cannot contain a comma");
}

// "YYYY-MM-DD": 10 characters, '-' at positions 4 and 7, digits elsewhere
void checkDate(const string& date) {
    bool ok = date.size() == 10;
    for (size_t i = 0; ok && i < date.size(); i++) {
        if (i == 4 || i == 7)
            ok = (date[i] == '-');
        else
            ok = isdigit(static_cast<unsigned char>(date[i])) != 0;
    }
    if (!ok)
        throw ShopException("Order date must look like YYYY-MM-DD, not '" + date + "'");
}

} // namespace


Order::Order(const string& id, const string& customerId, const string& date,
             const vector<OrderItem>& items) {
    checkText(id, "ID");
    checkText(customerId, "customer ID");
    checkDate(date);
    if (items.empty())
        throw ShopException("An order needs at least one item");

    this->id = id;
    this->customerId = customerId;
    this->date = date;
    this->items = items;
}

double Order::calculateTotal() const {
    double total = 0;
    for (const OrderItem& item : items)
        total += item.calculateSubtotal();
    return total;
}

void Order::printReceipt(const Inventory& inventory) const {
    cout << "\n========== Order " << id << " ==========" << endl;
    cout << "Customer: " << customerId << "    Date: " << date << endl;
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
    for (const OrderItem& item : items) {
        string name = inventory.exists(item.getProductId())
                          ? inventory.findProduct(item.getProductId()).getName()
                          : "(removed product)";
        cout << left
             << setw(8)  << item.getProductId()
             << setw(22) << name
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

// ---------------------------------------------------------------
// File format
// ---------------------------------------------------------------
string Order::toLine() const {
    string itemsText;
    for (size_t i = 0; i < items.size(); i++) {
        if (i > 0)
            itemsText += ";";
        itemsText += items[i].toField();
    }
    return id + "," + customerId + "," + date + "," + itemsText;
}

Order Order::fromLine(const string& line) {
    string clean = line;
    // Ignore a trailing '\r' (files saved with Windows line endings)
    if (!clean.empty() && clean.back() == '\r')
        clean.pop_back();

    vector<string> parts = splitBy(clean, ',');
    if (parts.size() != 4)
        throw ShopException("Bad order record (expected 4 fields): '" + clean + "'");

    vector<OrderItem> items;
    for (const string& field : splitBy(parts[3], ';'))
        items.push_back(OrderItem::fromField(field));

    return Order(parts[0], parts[1], parts[2], items);
}

// ---------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------
string Order::todayDate() {
    time_t now = time(nullptr);
    char buffer[16];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtime(&now));
    return buffer;
}

string Order::generateNextId(const vector<Order>& orders) {
    int highest = 0;
    for (const Order& o : orders) {
        const string& id = o.getId();
        // Only look at IDs shaped like "O" + up to 9 digits
        if (id.size() < 2 || id.size() > 10 || id[0] != 'O')
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
    return "O" + number;
}