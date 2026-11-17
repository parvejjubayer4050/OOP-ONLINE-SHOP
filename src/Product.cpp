#include "Product.h"
#include "Exceptions.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

// ---------------------------------------------------------------
// Small helper functions, visible only inside this file
// ---------------------------------------------------------------
namespace {

// id and name are stored in a comma-separated file, so they must be
// non-empty and must not contain a comma.
void checkText(const string& value, const string& fieldName) {
    if (value.empty())
        throw ShopException("Product " + fieldName + " cannot be empty");
    if (value.find(',') != string::npos)
        throw ShopException("Product " + fieldName + " cannot contain a comma");
}

void checkPrice(double price) {
    if (price < 0)
        throw ShopException("Product price cannot be negative");
}

void checkStock(int stock) {
    if (stock < 0)
        throw ShopException("Product stock cannot be negative");
}

// Split "a,b,c,d" into {"a","b","c","d"}
vector<string> splitByComma(const string& line) {
    vector<string> parts;
    string part;
    stringstream ss(line);
    while (getline(ss, part, ','))
        parts.push_back(part);
    // getline drops an empty last field ("a,b," -> a, b), so add it back
    if (!line.empty() && line.back() == ',')
        parts.push_back("");
    return parts;
}

// Convert text to double / int, or throw ShopException with a clear message.
double toDouble(const string& text, const string& fieldName) {
    try {
        size_t used = 0;
        double value = stod(text, &used);
        if (used != text.size())
            throw invalid_argument("extra characters");
        return value;
    }
    catch (const exception&) {
        throw ShopException("Invalid " + fieldName + " in product record: '" + text + "'");
    }
}

int toInt(const string& text, const string& fieldName) {
    try {
        size_t used = 0;
        int value = stoi(text, &used);
        if (used != text.size())
            throw invalid_argument("extra characters");
        return value;
    }
    catch (const exception&) {
        throw ShopException("Invalid " + fieldName + " in product record: '" + text + "'");
    }
}

} // namespace


// ---------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------
Product::Product(const string& id, const string& name, double price, int stock) {
    checkText(id, "ID");
    checkText(name, "name");
    checkPrice(price);
    checkStock(stock);

    this->id = id;
    this->name = name;
    this->price = price;
    this->stock = stock;
}

// ---------------------------------------------------------------
// Setters
// ---------------------------------------------------------------
void Product::setName(const string& newName) {
    checkText(newName, "name");
    name = newName;
}

void Product::setPrice(double newPrice) {
    checkPrice(newPrice);
    price = newPrice;
}

void Product::setStock(int newStock) {
    checkStock(newStock);
    stock = newStock;
}

// ---------------------------------------------------------------
// Stock operations
// ---------------------------------------------------------------
bool Product::hasStock(int quantity) const {
    return stock >= quantity;
}

void Product::increaseStock(int quantity) {
    if (quantity <= 0)
        throw ShopException("Quantity to add must be positive");
    stock += quantity;
}

void Product::reduceStock(int quantity) {
    if (quantity <= 0)
        throw ShopException("Quantity to remove must be positive");
    if (quantity > stock)
        throw OutOfStockException(name, quantity, stock);
    stock -= quantity;
}

// ---------------------------------------------------------------
// Display: one aligned row, e.g.
// P001    Laptop               750000.00   10
// ---------------------------------------------------------------
void Product::display() const {
    cout << left
         << setw(8)  << id
         << setw(22) << name
         << right << fixed << setprecision(2)
         << setw(12) << price
         << setw(8)  << stock
         << endl;
    cout.unsetf(ios::floatfield);   // restore default number formatting
    cout << left;                   // restore default alignment
}

// ---------------------------------------------------------------
// File format
// ---------------------------------------------------------------
string Product::toLine() const {
    // Use a stringstream so a price like 750000 is written as "750000",
    // not "750000.000000" (which is what std::to_string would give).
    // setprecision(15) stops large prices turning into "1.23457e+06".
    ostringstream out;
    out << setprecision(15) << id << "," << name << "," << price << "," << stock;
    return out.str();
}

Product Product::fromLine(const string& line) {
    string clean = line;
    // Ignore a trailing '\r' (files saved with Windows line endings)
    if (!clean.empty() && clean.back() == '\r')
        clean.pop_back();

    vector<string> parts = splitByComma(clean);
    if (parts.size() != 4)
        throw ShopException("Bad product record (expected 4 fields): '" + clean + "'");

    double price = toDouble(parts[2], "price");
    int stock = toInt(parts[3], "stock");

    // The constructor does the final validation (empty text, negative values).
    return Product(parts[0], parts[1], price, stock);
}